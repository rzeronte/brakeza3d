//
// Created by eduardo on 12/10/22.
// Rewritten for FFmpeg 7.x / modern API
//

#ifndef BRAKEZA3D_VIDEOPLAYER_H
#define BRAKEZA3D_VIDEOPLAYER_H

#include <string>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <atomic>
#include <SDL_render.h>
#include <SDL_audio.h>
#include <GL/glew.h>
#include "../Render/Image.h"

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
#include <libavutil/imgutils.h>
#include <libavutil/channel_layout.h>
#include <libavutil/opt.h>
}

struct VideoFrame {
    uint8_t *data[4]{};
    int      linesize[4]{};
    int64_t  pts = 0;

    VideoFrame(int w, int h) {
        av_image_alloc(data, linesize, w, h, AV_PIX_FMT_YUV420P, 1);
    }
    ~VideoFrame() { av_freep(&data[0]); }
};

class VideoPlayer {
public:
    explicit VideoPlayer(const std::string &filename);
    ~VideoPlayer();

    // Blocking mode: runs its own event+render loop (cutscenes).
    void play();

    // Non-blocking overlay mode: call start() once, then onUpdate() each engine frame.
    void start();
    void onUpdate();
    void stop();

    [[nodiscard]] bool isFinished()    const { return finished; }
    [[nodiscard]] Image *getImage()    const { return overlayImage; }
    [[nodiscard]] int getWidth()       const { return width; }
    [[nodiscard]] int getHeight()      const { return height; }

private:
    void decodeLoop();

    AVFormatContext *formatCtx   = nullptr;
    AVCodecContext  *videoCtx    = nullptr;
    AVCodecContext  *audioCtx    = nullptr;
    SwsContext      *swsYUVCtx   = nullptr;   // YUV420P — used by play()
    SwsContext      *swsRGBACtx  = nullptr;   // RGBA    — used by start()/onUpdate()
    SwrContext      *swrCtx      = nullptr;

    int    videoStreamIdx = -1;
    int    audioStreamIdx = -1;
    double timeBase       = 0.0;
    int    width          = 0;
    int    height         = 0;

    // Blocking mode (play)
    SDL_Texture       *yuvTexture = nullptr;

    // Non-blocking overlay mode (start/onUpdate)
    GLuint  oglTexture   = 0;
    uint8_t *rgbaBuffer  = nullptr;
    Image   *overlayImage = nullptr;

    SDL_AudioDeviceID  audioDevId = 0;
    Uint64             playStartTicks = 0;

    std::thread             decodeThread;
    std::mutex              queueMutex;
    std::condition_variable queueCv;
    std::queue<VideoFrame*> frameQueue;
    std::atomic<bool>       finished{false};
    std::atomic<bool>       decoding{false};

    bool ready = false;

    static constexpr int MAX_QUEUE = 8;
};

#endif //BRAKEZA3D_VIDEOPLAYER_H
