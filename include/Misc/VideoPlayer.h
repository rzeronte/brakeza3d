//
// Created by eduardo on 12/10/22.
//

#ifndef BRAKEZA3D_VIDEOPLAYER_H
#define BRAKEZA3D_VIDEOPLAYER_H


#include <string>

extern "C" {
    #include <libavutil/dict.h>
    #include <libavformat/avformat.h>
}
class VideoPlayer {
public:
    VideoPlayer(const std::string &filename);

    void findFirstStream(AVFormatContext *pFormatCtx);
    void SaveFrame(AVFrame *pFrame, int width, int height, int iFrame);
};


#endif //BRAKEZA3D_VIDEOPLAYER_H
