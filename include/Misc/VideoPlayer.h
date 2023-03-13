//
// Created by eduardo on 12/10/22.
//

#ifndef BRAKEZA3D_VIDEOPLAYER_H
#define BRAKEZA3D_VIDEOPLAYER_H


#include <string>
#include <SDL_render.h>
#include <SDL_audio.h>
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
    AVCodecContext *videoCodecContext = nullptr;
    struct SwsContext *swsContext = nullptr;
    SDL_Texture* screenTexture = nullptr;

    bool finished;
    AVCodec *videoCodec;
    Counter *fps;

    AVCodec *audioCodec;
    AVCodecContext *audioContext;

    SDL_AudioDeviceID audioDeviceId;
    SDL_AudioSpec want, have;
    AVCodecParameters *audioCodecParameters;

    AVFrame *audioFrame;

public:
    explicit VideoPlayer(const std::string &filename);

    void findFirstStream(AVFormatContext *pFormatCtx);
    void onUpdate();
    void renderToScreen();
    void renderToScreenTexture();

    [[nodiscard]] bool isFinished() const;

    void playAudio(AVCodecContext *ctx, AVPacket *pkt, AVFrame *frame, SDL_AudioDeviceID auddev);

    void renderToScreenFromYUV();
};


#endif //BRAKEZA3D_VIDEOPLAYER_H
