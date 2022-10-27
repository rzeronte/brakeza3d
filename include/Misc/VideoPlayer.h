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

class VideoPlayer {
    int videoStream = -1;
    AVFormatContext *pFormatCtx = nullptr;
    AVFrame *pFrame = nullptr;
    AVFrame *pFrameRGB = nullptr;
    AVCodecContext *pCodecCtx = nullptr;
    struct SwsContext *sws_ctx = NULL;
    SDL_Texture* bmp = NULL;

    AVCodec *pCodec;
    int currentFrame;
    Counter *fps;
public:
    VideoPlayer(const std::string &filename);

    void findFirstStream(AVFormatContext *pFormatCtx);
    void onUpdate();
    void flipToVideo(int frame);

    void save_texture(const char* file_name, SDL_Renderer* renderer, SDL_Texture* texture);

    void SaveFrame(int width, int height, int iFrame);

    void renderToScreen(int width, int height, int iFrame);
};


#endif //BRAKEZA3D_VIDEOPLAYER_H
