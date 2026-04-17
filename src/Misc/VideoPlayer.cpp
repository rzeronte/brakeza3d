//
// Created by eduardo on 12/10/22.
// Rewritten for FFmpeg 7.x / modern API
//

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

    // SwsContext: decoded pixel format → RGBA (OpenGL)
    swsRGBACtx = sws_getContext(
        width, height, videoCtx->pix_fmt,
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
void VideoPlayer::play()
{
    if (!ready) return;

    if (!yuvTexture) {
        yuvTexture = SDL_CreateTexture(
            Components::get()->Window()->getRenderer(),
            SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING,
            width, height
        );
        if (!yuvTexture) {
            LOG_ERROR("VideoPlayer: SDL_CreateTexture failed: %s", SDL_GetError());
            return;
        }
    }

    decoding = true;
    finished = false;
    decodeThread = std::thread(&VideoPlayer::decodeLoop, this);

    auto *renderer  = Components::get()->Window()->getRenderer();
    auto startTicks = SDL_GetTicks64();

    SDL_Event e;
    while (true) {
        // Events (only main thread can poll SDL events)
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT)
                Config::get()->EXIT = true;
            if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE)
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
            SDL_UpdateYUVTexture(yuvTexture, nullptr,
                frame->data[0], frame->linesize[0],
                frame->data[1], frame->linesize[1],
                frame->data[2], frame->linesize[2]);
            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, yuvTexture, nullptr, nullptr);
            SDL_RenderPresent(renderer);
            delete frame;
        } else {
            SDL_Delay(1);
        }
    }

    // Stop decode thread
    decoding = false;
    finished = true;
    queueCv.notify_all();
    if (decodeThread.joinable()) decodeThread.join();

    // Drain leftover frames
    std::lock_guard<std::mutex> lock(queueMutex);
    while (!frameQueue.empty()) {
        delete frameQueue.front();
        frameQueue.pop();
    }
}
