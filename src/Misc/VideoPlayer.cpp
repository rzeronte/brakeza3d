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

VideoPlayer::VideoPlayer(const std::string &filename) {
    pFormatCtx = NULL;

    if (avformat_open_input(&pFormatCtx, filename.c_str(), nullptr, nullptr) != 0) {
        Logging::Log("VideoPlayer Couldn't open file", "ERROR");
        return;
    }

    if (avformat_find_stream_info(pFormatCtx, NULL)<0) {
        Logging::Log("VideoPlayer Couldn't find stream information", "ERROR");
        return;
    }

    // Dump information about file onto standard error
    //av_dump_format(pFormatCtx, 0, filename.c_str(), 0);

    this->findFirstStream(pFormatCtx);

    bmp = SDL_CreateTexture(
        ComponentsManager::get()->getComponentWindow()->renderer,
        SDL_PIXELFORMAT_YV12,
        SDL_TEXTUREACCESS_STREAMING,
        pCodecCtx->width,
        pCodecCtx->height
    );
}

void VideoPlayer::findFirstStream(AVFormatContext *pFormatCtx)
{

    // Find the first video stream
    videoStream = -1;
    Logging::Log("VideoPlayer File Number Streams: (" + std::to_string(pFormatCtx->nb_streams) + ")", "VIDEO");
    for (unsigned int i = 0; i < pFormatCtx->nb_streams; i++) {
        Logging::Log("VideoPlayer AVI stream TypeCoded (" + std::to_string(pFormatCtx->streams[i]->codec->codec_type) + ")", "ERROR");
        if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoStream = (int) i;
            break;
        }
    }

    if (videoStream == -1) {
        Logging::Log("VideoPlayer didn't find a video stream", "ERROR");
        exit(-1);
    }

    Logging::Log("VideoPlayer stream found it (" + std::to_string(videoStream) + ")", "ERROR");

    pCodecCtx = NULL;

    // Get a pointer to the codec context for the video stream
    pCodecCtx = pFormatCtx->streams[videoStream]->codec;

    pCodec = nullptr;

    // Find the decoder for the video stream
    pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
    if (pCodec == nullptr) {
        Logging::Log("Unsupported codec!", "ERROR");
        exit(-1); // Codec not found
    }

    // Copy context
    /*pCodecCtx = avcodec_alloc_context3(pCodec);
    if (avcodec_copy_context(pCodecCtx, pCodecCtxOrig) != 0) {
        fprintf(stderr, "Couldn't copy codec context");
        exit(-1); // Error copying codec context
    }*/

    // Open codec
    if (avcodec_open2(pCodecCtx, pCodec, nullptr) < 0) {
        Logging::Log("Could not open codec!", "ERROR");
        exit(-1); //
    }

    pFrame = NULL;
    pFrameRGB = NULL;

    // Allocate video frame
    pFrame = av_frame_alloc();

    pFrameRGB = av_frame_alloc();
    if (pFrameRGB == NULL) {
        exit(-1);
    }

    uint8_t *buffer = NULL;
    int numBytes;
    // Determine required buffer size and allocate buffer
    numBytes = avpicture_get_size(AV_PIX_FMT_RGB24, pCodecCtx->width,pCodecCtx->height);
    buffer = (uint8_t *)av_malloc(numBytes*sizeof(uint8_t));

    // Assign appropriate parts of buffer to image planes in pFrameRGB
    // Note that pFrameRGB is an AVFrame, but AVFrame is a superset
    // of AVPicture
    avpicture_fill((AVPicture *)pFrameRGB, buffer, AV_PIX_FMT_RGB24,pCodecCtx->width, pCodecCtx->height);

    currentFrame = 0;


    // initialize SWS context for software scaling
    sws_ctx = sws_getContext(
            pCodecCtx->width,
            pCodecCtx->height,
            pCodecCtx->pix_fmt,
            pCodecCtx->width,
            pCodecCtx->height,
            AV_PIX_FMT_RGB24,
            SWS_BILINEAR,
            NULL,
            NULL,
            NULL
    );

}

void VideoPlayer::onUpdate()
{
    AVPacket packet;
    int frameFinished;

    if (av_read_frame(pFormatCtx, &packet) >= 0) {
        // Is this a packet from the video stream?
        if (packet.stream_index == videoStream) {
            // Decode video frame
            avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished, &packet);

            // Did we get a video frame?
            if (frameFinished) {
                // Convert the image from its native format to RGB
                sws_scale(
                    sws_ctx,
                    (uint8_t const * const *)pFrame->data,
                    pFrame->linesize,
                    0,
                    pCodecCtx->height,
                    pFrameRGB->data,
                    pFrameRGB->linesize
                );

                currentFrame++;

                SDL_UpdateYUVTexture(
                    bmp,
                    NULL,
                    pFrame->data[0],
                    pFrame->linesize[0],
                    pFrame->data[1],
                    pFrame->linesize[1],
                    pFrame->data[2],
                    pFrame->linesize[2]
                 );
                SDL_RenderCopy(ComponentsManager::get()->getComponentWindow()->renderer, bmp, NULL, NULL);
            }
        }

        // Free the packet that was allocated by av_read_frame
        av_free_packet(&packet);
    }
}

void VideoPlayer::renderToScreen(int width, int height, int iFrame)
{
    // Write pixel data
    auto data = pFrameRGB->data[0];


    uint8_t *p = pFrameRGB->data[0];
    for(int y = 0; y < height; y++) {
        for(int x = 0; x < width; x++) {
            auto r = *p++ = x % 255; // R
            auto g = *p++ = 0; // G
            auto b = *p++ = 0; // B

            auto color = new Color(r, g, b);

            EngineBuffers::getInstance()->setVideoBuffer(x, y, color->getColor());

        }
    }
}

void VideoPlayer::SaveFrame(int width, int height, int iFrame)
{
    Logging::Log("Guardo", "");
    FILE *pFile;
    char szFilename[32];
    int  y;

    // Open file
    std::string file = EngineSetup::get()->ASSETS_FOLDER + "frame%d.ppm";

    sprintf(szFilename, file.c_str(), iFrame);

    pFile=fopen(szFilename, "wb");
    if (pFile == NULL)
        return;

    // Write header
    fprintf(pFile, "P6\n%d %d\n255\n", width, height);

    // Write pixel data
    for(y=0; y<height; y++) {
        fwrite(pFrameRGB->data[0]+y*pFrameRGB->linesize[0], 1, width*3, pFile);
    }

    // Close file
    fclose(pFile);
}
