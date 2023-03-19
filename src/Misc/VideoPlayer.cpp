//
// Created by eduardo on 12/10/22.
//

#include "../../include/Misc/VideoPlayer.h"
#include <string>
#include "../../include/Brakeza3D.h"
#include <libavcodec/avcodec.h>
#include <thread>

extern "C" {
#include <libavutil/dict.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}


int threadPlayVideo(void *raw)
{
    auto *data = (ThreadVideoData *) raw;
    int frameFinished;
    avcodec_decode_video2(data->videoContext, data->videoFrame, &frameFinished, data->packet);

    if (frameFinished) {
        data->player->renderToScreenFromYUV();
    }

    SDL_Delay(41);

    return 0;
}

int threadPlayAudio(void *raw)
{
    auto data = (ThreadAudioData *) raw;

    if (avcodec_send_packet(data->audioContext, data->packet) < 0) {
        perror("send packet");
        return -1;
    }
    if (avcodec_receive_frame(data->audioContext, data->audioFrame) < 0) {
        perror("receive frame");
        return -1;
    }

    int size;

    av_samples_get_buffer_size(&size, data->audioContext->channels, data->audioFrame->nb_samples, (AVSampleFormat) data->audioFrame->format, 0);
    bool isPlanar = av_sample_fmt_is_planar((AVSampleFormat) data->audioFrame->format) == 1;
    for (int ch = 0; ch < data->audioContext->channels; ch++) {
        if (!isPlanar) {
            if (SDL_QueueAudio(data->audioDeviceId, data->audioFrame->data[ch], data->audioFrame->linesize[ch]) < 0) {
                perror("playAudio");
                printf(" %s\n", SDL_GetError());
                return 0;
            }
        } else {
            if (SDL_QueueAudio(data->audioDeviceId, data->audioFrame->data[0] + size*ch, size) < 0) {
                perror("playAudio");
                printf(" %s\n", SDL_GetError());
                return 0;
            }
        }
    }

    return 0;
}

int decode_thread(void *raw)
{
    auto *data = (ThreadData *) raw;

    Logging::Message("decode_thread START");

    SDL_Event e;
    bool &finished = Brakeza3D::get()->finish;
    for (;;) {
        while (SDL_PollEvent(&e)) {
            ComponentsManager::get()->getComponentInput()->onSDLPollEvent(&e, finished);
            ComponentsManager::get()->getComponentGameInput()->onSDLPollEvent(&e, finished);
        }

        if (finished || data->player->finished) {
            data->player->finished = true;
            return -1;
        }

        AVPacket packet;
        if (av_read_frame(data->pFormatCtx, &packet) >= 0) {
            data->packet = &packet;

            if (data->packet->stream_index == data->videoStream) {
                auto videoInfo = ThreadVideoData(data->videoContext, data->videoFrame, data->packet, 0, data->player);
                threadPlayVideo(&videoInfo);
            }

            if (data->packet->stream_index == data->audioStream) {
                auto audioInfo = ThreadAudioData(data->audioContext, &packet, data->audioFrame, data->audioDeviceId);
                threadPlayAudio(&audioInfo );
            }
            av_packet_unref(data->packet);
        } else {
            data->player->finished = true;
            break;
        }
    }


    return 0;
}

VideoPlayer::VideoPlayer(const std::string &filename)
:
    finished(false)
{
    pFormatCtx = NULL;

    if (avformat_open_input(&pFormatCtx, filename.c_str(), nullptr, nullptr) != 0) {
        Logging::Log("VideoPlayer Couldn't open file");
        return;
    }

    if (avformat_find_stream_info(pFormatCtx, NULL)<0) {
        Logging::Log("VideoPlayer Couldn't find stream information");
        return;
    }

    findFirstStream();

    screenTexture = SDL_CreateTexture(
            ComponentsManager::get()->getComponentWindow()->getRenderer(),
            SDL_PIXELFORMAT_YV12,
            SDL_TEXTUREACCESS_STREAMING,
            videoContext->width,
            videoContext->height
    );

}

void VideoPlayer::findFirstStream()
{
    // Find the first video stream
    videoStream = -1;
    Logging::Log("VideoPlayer File Number Streams: (%d)", pFormatCtx->nb_streams);

    for (unsigned int i = 0; i < pFormatCtx->nb_streams; i++) {
        Logging::Message("VideoPlayer AVI stream TypeCoded (%d)", pFormatCtx->streams[i]->codec->codec_type);
        if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoStream = (int) i;

            AVRational rational = pFormatCtx->streams[i]->avg_frame_rate;
            fpsRendering = 1.0 / ((double)rational.num / (double)(rational.den));
        }

        if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO) {
            audioCodec = avcodec_find_decoder(pFormatCtx->streams[i]->codecpar->codec_id);
            audioCodecParameters = pFormatCtx->streams[i]->codecpar;
            audioStream = (int) i;
        }
    }

    if (videoStream == -1) {
        Logging::Message("VideoPlayer didn't find a video stream");
        exit(-1);
    }

    if (audioStream == -1) {
        Logging::Message("VideoPlayer didn't find a audio stream");
        exit(-1);
    }

    videoContext = pFormatCtx->streams[videoStream]->codec;
    videoCodec = avcodec_find_decoder(videoContext->codec_id);
    if (videoCodec == nullptr) {
        Logging::Log("Unsupported video codec!");
        exit(-1); // Codec not found
    }

    audioContext = avcodec_alloc_context3(audioCodec);
    if (avcodec_parameters_to_context(audioContext, audioCodecParameters) < 0) {
        perror("audCtx");
        exit(-1);
    }

    // Open codec
    if (avcodec_open2(videoContext, videoCodec, nullptr) < 0) {
        Logging::Log("Could not open video codec!");
        exit(-1); //
    }

    if (avcodec_open2(audioContext, audioCodec, NULL) < 0) {
        Logging::Log("Could not open audio codec!");
        exit(-1); //
    }

    videoFrame = NULL;
    videoFrameRGB = NULL;

    // Allocate video frame
    videoFrame = av_frame_alloc();

    videoFrameRGB = av_frame_alloc();
    if (videoFrameRGB == NULL) {
        exit(-1);
    }

    uint8_t *buffer = NULL;
    int numBytes;
    // Determine required buffer size and allocate buffer
    numBytes = avpicture_get_size(AV_PIX_FMT_RGB32, videoContext->width, videoContext->height);
    buffer = (uint8_t *)av_malloc(numBytes*sizeof(uint8_t));

    // Assign appropriate parts of buffer to image planes in pFrameRGB
    // Note that pFrameRGB is an AVFrame, but AVFrame is a superset
    // of AVPicture
    avpicture_fill((AVPicture *)videoFrameRGB, buffer, AV_PIX_FMT_RGB24, videoContext->width, videoContext->height);

    // initialize SWS context for software scaling
    swsContext = sws_getContext(
            videoContext->width,
            videoContext->height,
            videoContext->pix_fmt,
            videoContext->width,
            videoContext->height,
            AV_PIX_FMT_BGR32,
            SWS_FAST_BILINEAR,
            NULL,
            NULL,
            NULL
    );

    //--
    audioFrame = av_frame_alloc();

    SDL_zero(want);
    SDL_zero(have);
    want.channels = audioContext->channels;
    want.format = AUDIO_F32SYS; // Sample format
    want.silence = 0;
    want.userdata = audioContext;
    want.freq = audioContext->sample_rate;


    audioDeviceId = SDL_OpenAudioDevice(NULL, 0, &want, &have, 0);
    SDL_PauseAudioDevice(audioDeviceId, 0);
    if (!audioDeviceId) {
        Logging::Message("Error audio device videoPlayer: %s", SDL_GetError());
        exit(-1);
    }
}

void VideoPlayer::play()
{
    AVPacket packet, *pkt = &packet;
    auto data = new ThreadData(
        this,
        this->pFormatCtx,
        videoContext,
        audioContext,
        pkt,
        videoFrame,
        audioFrame,
        audioDeviceId,
        videoStream,
        audioStream
    );

    threadFunction = SDL_CreateThread(decode_thread, "MainVideoThread", data);
    SDL_WaitThread( threadFunction, NULL );
    Logging::Message("exit decoding...");
}

void VideoPlayer::onUpdate()
{
}

void VideoPlayer::renderToScreenTexture()
{
    SDL_UpdateTexture(
        ComponentsManager::get()->getComponentWindow()->getScreenTexture(),
        nullptr,
        videoFrameRGB->data[0],
        videoFrameRGB->linesize[0]
    );
}

void VideoPlayer::renderToScreen()
{
    uint8_t *data = videoFrameRGB->data[0];

    for(int y = 0; y < videoContext->height; y++) {
        for(int x = 0; x < videoContext->width; x++) {

            int offset = 4 * x + y * videoFrameRGB->linesize[0];

            Uint8 r = data[offset];
            Uint8 g = data[offset + 1];
            Uint8 b = data[offset + 2];

            EngineBuffers::getInstance()->setVideoBuffer(x, y, Color(r, g, b).getColor());
        }
    }
}

bool VideoPlayer::isFinished() const
{
    return finished;
}


void VideoPlayer::renderToScreenFromYUV()
{
    SDL_UpdateYUVTexture(
        screenTexture,
        NULL,
        videoFrame->data[0],
        videoFrame->linesize[0],
        videoFrame->data[1],
        videoFrame->linesize[1],
        videoFrame->data[2],
        videoFrame->linesize[2]
    );

    auto renderer = ComponentsManager::get()->getComponentWindow()->getRenderer();
    SDL_RenderCopy(renderer, screenTexture, NULL, NULL);
    SDL_RenderPresent(renderer);
}
