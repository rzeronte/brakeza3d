//
// Created by eduardo on 12/10/22.
//

#include "../../include/Misc/VideoPlayer.h"
#include <string>
#include "../../include/Render/Logging.h"
#include "../../include/EngineSetup.h"
#include "../../include/ComponentsManager.h"
#include <libavcodec/avcodec.h>

extern "C" {
#include <libavutil/dict.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}

VideoPlayer::VideoPlayer(const std::string &filename) : finished(false)
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

    // Dump information about file onto standard error
    //av_dump_format(pFormatCtx, 0, filename.c_str(), 0);

    this->findFirstStream(pFormatCtx);

    screenTexture = SDL_CreateTexture(
        ComponentsManager::get()->getComponentWindow()->getRenderer(),
        SDL_PIXELFORMAT_YV12,
        SDL_TEXTUREACCESS_STREAMING,
        videoCodecContext->width,
        videoCodecContext->height
    );

}

void VideoPlayer::findFirstStream(AVFormatContext *pFormatCtx)
{
    // Find the first video stream
    videoStream = -1;
    Logging::Log("VideoPlayer File Number Streams: (%d)", pFormatCtx->nb_streams);

    for (unsigned int i = 0; i < pFormatCtx->nb_streams; i++) {
        Logging::Message("VideoPlayer AVI stream TypeCoded (%d)", pFormatCtx->streams[i]->codec->codec_type);
        if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoStream = (int) i;
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

    videoCodecContext = pFormatCtx->streams[videoStream]->codec;
    videoCodec = avcodec_find_decoder(videoCodecContext->codec_id);
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
    if (avcodec_open2(videoCodecContext, videoCodec, nullptr) < 0) {
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
    numBytes = avpicture_get_size(AV_PIX_FMT_RGB32, videoCodecContext->width, videoCodecContext->height);
    buffer = (uint8_t *)av_malloc(numBytes*sizeof(uint8_t));

    // Assign appropriate parts of buffer to image planes in pFrameRGB
    // Note that pFrameRGB is an AVFrame, but AVFrame is a superset
    // of AVPicture
    avpicture_fill((AVPicture *)videoFrameRGB, buffer, AV_PIX_FMT_RGB24, videoCodecContext->width, videoCodecContext->height);

    // initialize SWS context for software scaling
    swsContext = sws_getContext(
        videoCodecContext->width,
        videoCodecContext->height,
        videoCodecContext->pix_fmt,
        videoCodecContext->width,
        videoCodecContext->height,
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
    want.samples = 2048;
    want.channels = 2;
    want.format = AUDIO_F32SYS; // Sample format
    want.silence = 0;
    want.userdata = audioContext;

    audioDeviceId = SDL_OpenAudioDevice(NULL, 0, &want, &have, 0);
    SDL_PauseAudioDevice(audioDeviceId, 0);
    if (!audioDeviceId) {
        Logging::Message("Error audio device videoPlayer: %s", SDL_GetError());
        exit(-1);
    }
}

void VideoPlayer::onUpdate()
{
    AVPacket packet;
    int frameFinished;

    if (finished) return;

    if (av_read_frame(pFormatCtx, &packet) >= 0) {
        // Is this a packet from the video stream?
        if (packet.stream_index == videoStream) {
            // Decode video frame
            avcodec_decode_video2(videoCodecContext, videoFrame, &frameFinished, &packet);

            // Did we get a video frame?
            if (frameFinished) {
                // Convert the image from its native format to RGB
                sws_scale(
                        swsContext,
                        (uint8_t const * const *)videoFrame->data,
                        videoFrame->linesize,
                        0,
                        videoCodecContext->height,
                        videoFrameRGB->data,
                        videoFrameRGB->linesize
                );
            }
        }
        if (packet.stream_index == audioStream) {
            playAudio(audioContext, &packet, audioFrame, audioDeviceId);
        }
        this->renderToScreenFromYUV();
        av_packet_unref(&packet);
    } else {
        Logging::Log("Finished");
        finished = true;
    }
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

    for(int y = 0; y < videoCodecContext->height; y++) {
        for(int x = 0; x < videoCodecContext->width; x++) {

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

void VideoPlayer::playAudio(AVCodecContext *ctx, AVPacket *pkt, AVFrame *frame, SDL_AudioDeviceID auddev)
{
    if (avcodec_send_packet(ctx, pkt) < 0) {
        perror("send packet");
        return;
    }
    if (avcodec_receive_frame(ctx, frame) < 0) {
        perror("receive frame");
        return;
    }
    int size;
    int bufsize = av_samples_get_buffer_size(&size, ctx->channels, frame->nb_samples, (AVSampleFormat) frame->format, 0);
    bool isplanar = av_sample_fmt_is_planar((AVSampleFormat) frame->format) == 1;
    for (int ch = 0; ch < ctx->channels; ch++) {
        if (!isplanar) {
            if (SDL_QueueAudio(auddev, frame->data[ch], frame->linesize[ch]) < 0) {
                perror("playAudio");
                printf(" %s\n", SDL_GetError());
                return;
            }
        } else {
            if (SDL_QueueAudio(auddev, frame->data[0] + size*ch, size) < 0) {
                perror("playAudio");
                printf(" %s\n", SDL_GetError());
                return;
            }
        }
    }
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
    SDL_RenderCopy(ComponentsManager::get()->getComponentWindow()->getRenderer(), screenTexture, NULL, NULL);
}