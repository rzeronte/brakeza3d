//
// Created by eduardo on 12/10/22.
// Rewritten for FFmpeg 7.x / modern API
//

#include <algorithm>
#include <vector>
#include "../../include/Misc/VideoPlayer.h"
#include "../../include/Misc/Logging.h"
#include "../../include/Components/Components.h"
#include "../../include/Config.h"

extern "C" {
#include <libavutil/time.h>
}

VideoPlayer::VideoPlayer(const std::string &filename)
{
    if (avformat_open_input(&formatCtx, filename.c_str(), nullptr, nullptr) < 0) {
        LOG_ERROR("VideoPlayer: cannot open '%s'", filename.c_str());
        return;
    }
    if (avformat_find_stream_info(formatCtx, nullptr) < 0) {
        LOG_ERROR("VideoPlayer: cannot find stream info");
        return;
    }

    for (unsigned i = 0; i < formatCtx->nb_streams; i++) {
        auto type = formatCtx->streams[i]->codecpar->codec_type;
        if (type == AVMEDIA_TYPE_VIDEO && videoStreamIdx < 0) videoStreamIdx = (int)i;
        if (type == AVMEDIA_TYPE_AUDIO && audioStreamIdx < 0) audioStreamIdx = (int)i;
    }

    if (videoStreamIdx < 0) {
        LOG_ERROR("VideoPlayer: no video stream found");
        return;
    }

    // ── Video codec ──────────────────────────────────────────────────────────
    AVStream *vst       = formatCtx->streams[videoStreamIdx];
    const AVCodec *vc   = avcodec_find_decoder(vst->codecpar->codec_id);
    if (!vc) { LOG_ERROR("VideoPlayer: unsupported video codec"); return; }

    videoCtx = avcodec_alloc_context3(vc);
    avcodec_parameters_to_context(videoCtx, vst->codecpar);
    if (avcodec_open2(videoCtx, vc, nullptr) < 0) {
        LOG_ERROR("VideoPlayer: cannot open video codec");
        return;
    }

    width    = videoCtx->width;
    height   = videoCtx->height;
    timeBase = av_q2d(vst->time_base);

    // ── Audio codec ──────────────────────────────────────────────────────────
    if (audioStreamIdx >= 0) {
        AVStream *ast     = formatCtx->streams[audioStreamIdx];
        const AVCodec *ac = avcodec_find_decoder(ast->codecpar->codec_id);
        if (ac) {
            audioCtx = avcodec_alloc_context3(ac);
            avcodec_parameters_to_context(audioCtx, ast->codecpar);
            avcodec_open2(audioCtx, ac, nullptr);

            // SwrContext: resample to S16 stereo (SDL-compatible)
            AVChannelLayout outLayout = AV_CHANNEL_LAYOUT_STEREO;
            if (swr_alloc_set_opts2(
                    &swrCtx,
                    &outLayout,           AV_SAMPLE_FMT_S16, audioCtx->sample_rate,
                    &audioCtx->ch_layout, audioCtx->sample_fmt, audioCtx->sample_rate,
                    0, nullptr) < 0 || swr_init(swrCtx) < 0)
            {
                LOG_WARNING("VideoPlayer: SwrContext init failed — audio disabled");
                avcodec_free_context(&audioCtx);
                audioCtx = nullptr;
            }
        }

        if (audioCtx) {
            SDL_AudioSpec want{};
            want.freq     = audioCtx->sample_rate;
            want.format   = AUDIO_S16SYS;
            want.channels = 2;
            want.samples  = 1024;

            SDL_AudioSpec have{};
            audioDevId = SDL_OpenAudioDevice(nullptr, 0, &want, &have, 0);
            if (!audioDevId) {
                LOG_WARNING("VideoPlayer: SDL_OpenAudioDevice failed: %s", SDL_GetError());
            } else {
                SDL_PauseAudioDevice(audioDevId, 0);
            }
        }
    }

    // SwsContext: any pixel format → YUV420P (used by decode thread for both modes)
    swsYUVCtx = sws_getContext(
        width, height, videoCtx->pix_fmt,
        width, height, AV_PIX_FMT_YUV420P,
        SWS_BILINEAR, nullptr, nullptr, nullptr
    );

    ready = true;
    LOG_MESSAGE("VideoPlayer: ready — %dx%d @ %.2f fps, audio=%s",
        width, height, 1.0 / (timeBase > 0 ? timeBase : 1),
        audioCtx ? "yes" : "no");
}

VideoPlayer::~VideoPlayer()
{
    finished = true;
    decoding = false;
    queueCv.notify_all();
    if (decodeThread.joinable()) decodeThread.join();

    // Drain remaining frames
    std::lock_guard<std::mutex> lock(queueMutex);
    while (!frameQueue.empty()) {
        delete frameQueue.front();
        frameQueue.pop();
    }

    if (yuvTexture)   SDL_DestroyTexture(yuvTexture);
    if (oglTexture)   glDeleteTextures(1, &oglTexture);
    if (overlayImage) { delete overlayImage; overlayImage = nullptr; }
    if (rgbaBuffer)   { delete[] rgbaBuffer; rgbaBuffer = nullptr; }
    if (audioDevId)   SDL_CloseAudioDevice(audioDevId);
    if (swrCtx)       swr_free(&swrCtx);
    if (swsYUVCtx)    sws_freeContext(swsYUVCtx);
    if (swsRGBACtx)   sws_freeContext(swsRGBACtx);
    if (videoCtx)     avcodec_free_context(&videoCtx);
    if (audioCtx)     avcodec_free_context(&audioCtx);
    if (formatCtx)    avformat_close_input(&formatCtx);
}

// ─────────────────────────────────────────────────────────────────────────────
// Decode thread: reads packets, fills frameQueue (video) and SDL audio queue
// ─────────────────────────────────────────────────────────────────────────────
void VideoPlayer::decodeLoop()
{
    AVPacket *packet = av_packet_alloc();
    AVFrame  *frame  = av_frame_alloc();

    auto flushVideo = [&]() {
        avcodec_send_packet(videoCtx, nullptr);
        while (avcodec_receive_frame(videoCtx, frame) == 0) {
            auto *vf = new VideoFrame(width, height);
            vf->pts  = frame->pts;
            sws_scale(swsYUVCtx, frame->data, frame->linesize, 0, height, vf->data, vf->linesize);
            std::unique_lock<std::mutex> lock(queueMutex);
            queueCv.wait(lock, [this]{ return frameQueue.size() < MAX_QUEUE || !decoding; });
            if (!decoding) { delete vf; break; }
            frameQueue.push(vf);
            queueCv.notify_one();
        }
    };

    while (av_read_frame(formatCtx, packet) >= 0 && decoding) {
        if (packet->stream_index == videoStreamIdx) {
            avcodec_send_packet(videoCtx, packet);
            while (avcodec_receive_frame(videoCtx, frame) == 0) {
                auto *vf = new VideoFrame(width, height);
                vf->pts  = frame->pts;
                sws_scale(swsYUVCtx, frame->data, frame->linesize, 0, height, vf->data, vf->linesize);

                std::unique_lock<std::mutex> lock(queueMutex);
                queueCv.wait(lock, [this]{ return frameQueue.size() < MAX_QUEUE || !decoding; });
                if (!decoding) { delete vf; break; }
                frameQueue.push(vf);
                queueCv.notify_one();
            }
        } else if (packet->stream_index == audioStreamIdx && audioCtx && swrCtx && audioDevId) {
            avcodec_send_packet(audioCtx, packet);
            while (avcodec_receive_frame(audioCtx, frame) == 0) {
                int dstSamples = swr_get_out_samples(swrCtx, frame->nb_samples);
                uint8_t *outBuf = nullptr;
                int outLinesize = 0;
                av_samples_alloc(&outBuf, &outLinesize, 2, dstSamples, AV_SAMPLE_FMT_S16, 0);
                int converted = swr_convert(swrCtx, &outBuf, dstSamples,
                                            (const uint8_t **)frame->data, frame->nb_samples);
                if (converted > 0) {
                    SDL_QueueAudio(audioDevId, outBuf,
                                   (Uint32)(converted * 2 * sizeof(int16_t)));
                }
                av_freep(&outBuf);
            }
        }
        av_packet_unref(packet);
    }

    if (decoding) flushVideo();

    av_frame_free(&frame);
    av_packet_free(&packet);
    decoding = false;
    finished = true;
    queueCv.notify_all();
}

// ─────────────────────────────────────────────────────────────────────────────
// Non-blocking overlay mode
// ─────────────────────────────────────────────────────────────────────────────
void VideoPlayer::start()
{
    if (!ready || decoding) return;

    // SwsContext: YUV420P → RGBA (OpenGL). Los frames de la cola ya vienen en YUV420P (decodeLoop
    // los convierte con swsYUVCtx), no en el formato original del códec.
    swsRGBACtx = sws_getContext(
        width, height, AV_PIX_FMT_YUV420P,
        width, height, AV_PIX_FMT_RGBA,
        SWS_BILINEAR, nullptr, nullptr, nullptr
    );

    rgbaBuffer = new uint8_t[width * height * 4];

    // GL texture (RGBA, streaming)
    glGenTextures(1, &oglTexture);
    glBindTexture(GL_TEXTURE_2D, oglTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);

    overlayImage = new Image(oglTexture, width, height);

    playStartTicks = SDL_GetTicks64();
    decoding = true;
    finished = false;
    decodeThread = std::thread(&VideoPlayer::decodeLoop, this);
}

void VideoPlayer::onUpdate()
{
    if (!ready || !swsRGBACtx || !oglTexture) return;

    VideoFrame *frame = nullptr;
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        if (frameQueue.empty()) {
            if (!decoding) finished = true;
            return;
        }

        double elapsed  = (double)(SDL_GetTicks64() - playStartTicks) / 1000.0;
        double framePts = (double)frameQueue.front()->pts * timeBase;
        if (framePts > elapsed) return;

        frame = frameQueue.front();
        frameQueue.pop();
        queueCv.notify_one();
    }

    // Convert YUV420P → RGBA on main thread
    uint8_t *dst[4]   = { rgbaBuffer, nullptr, nullptr, nullptr };
    int dstStride[4]  = { width * 4,  0,       0,       0       };
    sws_scale(swsRGBACtx, frame->data, frame->linesize, 0, height, dst, dstStride);
    delete frame;

    glBindTexture(GL_TEXTURE_2D, oglTexture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, rgbaBuffer);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void VideoPlayer::stop()
{
    decoding = false;
    finished = true;
    queueCv.notify_all();
    if (decodeThread.joinable()) decodeThread.join();

    std::lock_guard<std::mutex> lock(queueMutex);
    while (!frameQueue.empty()) {
        delete frameQueue.front();
        frameQueue.pop();
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// Blocking playback loop (main thread): handles events, timing, SDL render
// ─────────────────────────────────────────────────────────────────────────────
void VideoPlayer::play(float fadeOutSeconds)
{
    if (!ready) return;

    // Se pinta con el OpenGL del propio motor, no con un SDL_Renderer: la ventana la presenta
    // OpenGL (ver ComponentWindow::InitWindow). Un renderer direct3d creado sobre ella no se ve (se
    // oía el vídeo pero no se veía) y el driver "opengl" de SDL pide un contexto 2.1 de
    // compatibilidad: con el 4.3 del motor llama a SDL_RecreateWindow y rompe su contexto.
    // Cada frame: YUV420P -> RGBA (swscale) -> textura -> glBlitFramebuffer a la ventana (escalado
    // con bandas negras) -> SDL_GL_SwapWindow.
    auto *window = Components::get()->Window()->getWindow();
    SDL_GL_MakeCurrent(window, Components::get()->Window()->getGLContext());

    SwsContext *toRGBA = sws_getContext(
        width, height, AV_PIX_FMT_YUV420P,
        width, height, AV_PIX_FMT_RGBA,
        SWS_BILINEAR, nullptr, nullptr, nullptr
    );
    if (!toRGBA) {
        LOG_ERROR("VideoPlayer: sws_getContext (RGBA) failed");
        return;
    }
    std::vector<uint8_t> rgba(static_cast<size_t>(width) * height * 4);

    // Estado GL que se toca aquí, para dejarlo como estaba al acabar
    GLint prevRead = 0, prevDraw = 0, prevTex = 0, prevUnpackAlign = 4, prevUnpackRow = 0;
    glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &prevRead);
    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &prevDraw);
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &prevTex);
    glGetIntegerv(GL_UNPACK_ALIGNMENT, &prevUnpackAlign);
    glGetIntegerv(GL_UNPACK_ROW_LENGTH, &prevUnpackRow);
    GLfloat prevClear[4];
    glGetFloatv(GL_COLOR_CLEAR_VALUE, prevClear);
    GLboolean prevMask[4];
    glGetBooleanv(GL_COLOR_WRITEMASK, prevMask);
    const GLboolean prevScissor = glIsEnabled(GL_SCISSOR_TEST);

    glDisable(GL_SCISSOR_TEST);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);

    GLuint tex = 0, fbo = 0;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);

    auto presentFrame = [&]() {
        int dw = 0, dh = 0;
        SDL_GL_GetDrawableSize(window, &dw, &dh);
        // Escalado que conserva la proporción, centrado
        const float scale = std::min(static_cast<float>(dw) / width, static_cast<float>(dh) / height);
        const int w  = static_cast<int>(width * scale);
        const int h  = static_cast<int>(height * scale);
        const int x0 = (dw - w) / 2;
        const int y0 = (dh - h) / 2;

        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
        // Las filas del vídeo van de arriba abajo y las de GL de abajo arriba: destino invertido en Y
        glBlitFramebuffer(0, 0, width, height, x0, y0 + h, x0 + w, y0, GL_COLOR_BUFFER_BIT, GL_LINEAR);
        SDL_GL_SwapWindow(window);
    };

    decoding = true;
    finished = false;
    decodeThread = std::thread(&VideoPlayer::decodeLoop, this);

    auto startTicks = SDL_GetTicks64();
    bool anyFrameShown = false;

    SDL_Event e;
    while (true) {
        // Events (only main thread can poll SDL events)
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT)
                Config::get()->EXIT = true;
            // Cualquier tecla o botón del ratón salta el vídeo
            if ((e.type == SDL_KEYDOWN && e.key.repeat == 0) || e.type == SDL_MOUSEBUTTONDOWN)
                finished = true;
        }

        if (Config::get()->EXIT || finished) break;

        VideoFrame *frame = nullptr;
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            if (!frameQueue.empty()) {
                double elapsed  = (double)(SDL_GetTicks64() - startTicks) / 1000.0;
                double framePts = (double)frameQueue.front()->pts * timeBase;
                if (framePts <= elapsed) {
                    frame = frameQueue.front();
                    frameQueue.pop();
                    queueCv.notify_one();
                }
            } else if (!decoding) {
                // Decode finished and queue drained
                break;
            }
        }

        if (frame) {
            uint8_t *dst[4]  = { rgba.data(), nullptr, nullptr, nullptr };
            int dstStride[4] = { width * 4, 0, 0, 0 };
            sws_scale(toRGBA, frame->data, frame->linesize, 0, height, dst, dstStride);
            delete frame;

            glBindTexture(GL_TEXTURE_2D, tex);
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, rgba.data());
            presentFrame();
            anyFrameShown = true;
        } else {
            SDL_Delay(1);
        }
    }

    // Stop decode thread
    decoding = false;
    finished = true;
    queueCv.notify_all();
    if (decodeThread.joinable()) decodeThread.join();

    // Fundido a negro sobre el último frame (al acabar o al saltarlo). El audio sigue con lo que ya
    // tenía en cola y se corta al final del fundido.
    if (fadeOutSeconds > 0.0f && anyFrameShown && !Config::get()->EXIT) {
        const std::vector<uint8_t> lastFrame = rgba;
        const Uint64 fadeStart = SDL_GetTicks64();
        while (true) {
            while (SDL_PollEvent(&e)) {
                if (e.type == SDL_QUIT) Config::get()->EXIT = true;
            }
            if (Config::get()->EXIT) break;
            const float t = static_cast<float>(SDL_GetTicks64() - fadeStart) / 1000.0f / fadeOutSeconds;
            if (t >= 1.0f) break;
            const float k = 1.0f - t;
            for (size_t i = 0; i < rgba.size(); i += 4) {
                rgba[i]     = static_cast<uint8_t>(lastFrame[i]     * k);
                rgba[i + 1] = static_cast<uint8_t>(lastFrame[i + 1] * k);
                rgba[i + 2] = static_cast<uint8_t>(lastFrame[i + 2] * k);
            }
            glBindTexture(GL_TEXTURE_2D, tex);
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, rgba.data());
            presentFrame();
        }
        // Último frame totalmente negro: el siguiente contenido (p. ej. el menú) puede entrar con
        // su propio fundido desde negro
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        SDL_GL_SwapWindow(window);
    }
    if (audioDevId) {
        SDL_ClearQueuedAudio(audioDevId);
        SDL_PauseAudioDevice(audioDevId, 1);
    }

    // Recursos del vídeo fuera y estado GL restaurado
    glBindFramebuffer(GL_READ_FRAMEBUFFER, static_cast<GLuint>(prevRead));
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, static_cast<GLuint>(prevDraw));
    glDeleteFramebuffers(1, &fbo);
    glDeleteTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, static_cast<GLuint>(prevTex));
    glPixelStorei(GL_UNPACK_ALIGNMENT, prevUnpackAlign);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, prevUnpackRow);
    glClearColor(prevClear[0], prevClear[1], prevClear[2], prevClear[3]);
    glColorMask(prevMask[0], prevMask[1], prevMask[2], prevMask[3]);
    if (prevScissor) glEnable(GL_SCISSOR_TEST);
    sws_freeContext(toRGBA);

    // Drain leftover frames
    std::lock_guard<std::mutex> lock(queueMutex);
    while (!frameQueue.empty()) {
        delete frameQueue.front();
        frameQueue.pop();
    }
}
