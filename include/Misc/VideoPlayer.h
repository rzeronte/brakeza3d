//
// Created by eduardo on 12/10/22.
//

#ifndef BRAKEZA3D_VIDEOPLAYER_H
#define BRAKEZA3D_VIDEOPLAYER_H


#include <string>
#include <SDL_render.h>
#include "Counter.h"

extern "C" {
    #include <libavutil/dict.h>
    #include <libavformat/avformat.h>
}

#define SDL_AUDIO_BUFFER_SIZE 1024

class VideoPlayer {
    int videoStream = -1;
    int audioStream = -1;

    AVFormatContext *pFormatCtx = nullptr;
    AVFrame *pFrame = nullptr;
    AVFrame *pFrameRGB = nullptr;
    AVCodecContext *pCodecCtx = nullptr;
    struct SwsContext *sws_ctx = nullptr;
    SDL_Texture* bmp = nullptr;
    SDL_Surface* surf = nullptr;

    AVCodec *pCodec;
    Counter *fps;

    bool finished;
public:
    VideoPlayer(const std::string &filename);

    void findFirstStream(AVFormatContext *pFormatCtx);
    void onUpdate();
    void renderToScreen();
    void renderToScreenTexture();

    bool isFinished() const;
};


#endif //BRAKEZA3D_VIDEOPLAYER_H
