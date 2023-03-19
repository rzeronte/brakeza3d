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

    bool finished;
    double fpsrendering;
    bool foundVideo;

    AVCodec *videoCodec;

    double lastPts;

    AVCodec *audioCodec;

    SDL_AudioDeviceID audioDeviceId;
    SDL_AudioSpec want, have;
    AVCodecParameters *audioCodecParameters;

    AVFrame *audioFrame;

    double pts, delay;

    SDL_Thread *threadFunction;

public:
    explicit VideoPlayer(const std::string &filename);

    void findFirstStream();
    void onUpdate();
    void renderToScreen();
    void renderToScreenTexture(int frameFinished);

    [[nodiscard]] bool isFinished() const;

    static void playAudio(void *raw);
    void playVideo(void *raw);
    void waitSync(void *raw);

    void renderToScreenFromYUV();

    Counter *fps;
    Counter *frameCounter;

    void drawSecond() const;

    void play();

    bool completed;
};

class ThreadData {

    Counter *counter = nullptr;
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
        int audioStream,
        double lastPts,
        Counter *counter,
        bool &finished,
        bool &completed
    )
    :
        counter(counter),
        pFormatCtx(pFormatCtx),
        packet(packet),
        videoStream(videoStream),
        audioStream(audioStream),
        player(player),
        finished(finished),
        completed(completed),
        videoContext(videoContext),
        videoFrame(videoFrame),
        lastPts(0),
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
    bool &finished;
    bool &completed;
    AVCodecContext *videoContext = nullptr;
    AVFrame *videoFrame = nullptr;
    double lastPts = 0;
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
            double &lastPts
    )
            : videoContext(videoCodecContext), videoFrame(videoFrame), packet(packet), frameFinished(frameFinished), lastPts(lastPts)
    {
    }
    AVCodecContext *videoContext = nullptr;
    AVFrame *videoFrame = nullptr;
    AVPacket *packet = nullptr;
    int frameFinished = 0;
    double &lastPts; // pts of last decoded frame / predicted pts of next decoded frame
};

struct ThreadAudioData {
    ThreadAudioData(AVCodecContext *ctx, AVPacket *pkt, AVFrame *frame, SDL_AudioDeviceID auddev)
            : ctx(ctx), pkt(pkt), frame(frame), auddev(auddev)
    {
    }

    AVCodecContext *ctx = nullptr;
    AVPacket *pkt = nullptr;
    AVFrame *frame = nullptr;
    SDL_AudioDeviceID auddev = 0;
};

#endif //BRAKEZA3D_VIDEOPLAYER_H
