//
// Created by eduardo on 12/10/22.
//

#ifndef BRAKEZA3D_VIDEOPLAYER_H
#define BRAKEZA3D_VIDEOPLAYER_H


#include <string>
#include <SDL_render.h>
#include <SDL_audio.h>
#include <thread>
#include "Counter.h"

extern "C" {
#include <libavutil/dict.h>
#include <libavformat/avformat.h>
}

class VideoPlayer {

    int videoStream = -1;
    int audioStream = -1;

    AVFormatContext *pFormatCtx = nullptr;
    AVFrame *videoFrame = nullptr;
    AVFrame *videoFrameRGB = nullptr;

    AVCodecContext *videoContext = nullptr;
    AVCodecContext *audioContext = nullptr;

    struct SwsContext *swsContext = nullptr;
    SDL_Texture* screenTexture = nullptr;

    double fpsRendering;

    AVCodec *videoCodec;

    AVCodec *audioCodec;

    SDL_AudioDeviceID audioDeviceId;
    SDL_AudioSpec want, have;
    AVCodecParameters *audioCodecParameters;

    AVFrame *audioFrame;

    SDL_Thread *threadFunction;

public:
    explicit VideoPlayer(const std::string &filename);

    void findFirstStream();
    void onUpdate();
    void renderToScreen();
    void renderToScreenTexture();
    void renderToScreenFromYUV();

    [[nodiscard]] bool isFinished() const;
    void play();

    bool finished;
};

class ThreadData {

public:
    ThreadData(
        VideoPlayer *player,
        AVFormatContext *pFormatCtx,
        AVCodecContext *videoContext,
        AVCodecContext *audioContext,
        AVPacket *packet,
        AVFrame *videoFrame,
        AVFrame *audioFrame,
        SDL_AudioDeviceID audioDeviceId,
        int videoStream,
        int audioStream
    )
    :
        pFormatCtx(pFormatCtx),
        packet(packet),
        videoStream(videoStream),
        audioStream(audioStream),
        player(player),
        videoContext(videoContext),
        videoFrame(videoFrame),
        audioContext(audioContext),
        audioFrame(audioFrame),
        audioDeviceId(audioDeviceId)
    {
    }

    AVFormatContext *pFormatCtx = nullptr;
    AVPacket *packet = nullptr;
    int videoStream = -1;
    int audioStream = -1;
    VideoPlayer *player = nullptr;
    AVCodecContext *videoContext = nullptr;
    AVFrame *videoFrame = nullptr;
    AVCodecContext *audioContext = nullptr;
    AVFrame *audioFrame = nullptr;
    SDL_AudioDeviceID audioDeviceId = 0;
};

struct ThreadVideoData {
    ThreadVideoData(
            AVCodecContext *videoCodecContext,
            AVFrame *videoFrame,
            AVPacket *packet,
            int frameFinished,
            VideoPlayer *player
    )
    : videoContext(videoCodecContext), videoFrame(videoFrame), packet(packet), frameFinished(frameFinished), player(player)
    {
    }
    AVCodecContext *videoContext = nullptr;
    AVFrame *videoFrame = nullptr;
    AVPacket *packet = nullptr;
    int frameFinished = 0;
    VideoPlayer *player = nullptr;
};

struct ThreadAudioData {
    ThreadAudioData(AVCodecContext *audioContext, AVPacket *packet, AVFrame *audioFrame, SDL_AudioDeviceID audioDeviceId)
            : audioContext(audioContext), packet(packet), audioFrame(audioFrame), audioDeviceId(audioDeviceId)
    {
    }

    AVCodecContext *audioContext = nullptr;
    AVPacket *packet = nullptr;
    AVFrame *audioFrame = nullptr;
    SDL_AudioDeviceID audioDeviceId = 0;
};

#endif //BRAKEZA3D_VIDEOPLAYER_H
