
#include <SDL_surface.h>
#include "../headers/EngineBuffers.h"
#include "../headers/EngineSetup.h"
#include "../headers/Tools.h"
#include "../headers/LTimer.h"
#include "../headers/Core/Logging.h"

EngineBuffers* EngineBuffers::instance = 0;

EngineBuffers* EngineBuffers::getInstance()
{
    if (instance == 0) {
        instance = new EngineBuffers();
    }

    return instance;
}

EngineBuffers::EngineBuffers()
{
    sizeBuffers = EngineSetup::getInstance()->SCREEN_WIDTH * EngineSetup::getInstance()->SCREEN_HEIGHT;

    Logging::getInstance()->Log("EngineBuffers size: " + std::to_string(sizeBuffers), "INFO");

    depthBuffer = new float[sizeBuffers];
    videoBuffer = new Uint32[sizeBuffers];
}

void EngineBuffers::clearDepthBuffer()
{
    std::fill(depthBuffer, depthBuffer + sizeBuffers, NULL);
}

float EngineBuffers::getDepthBuffer(int x, int y)
{
    return depthBuffer[( y * EngineSetup::getInstance()->SCREEN_WIDTH ) + x];
}

float EngineBuffers::setDepthBuffer(int x, int y, float value)
{
    depthBuffer[( y * EngineSetup::getInstance()->SCREEN_WIDTH ) + x] = value;
}

void EngineBuffers::setVideoBuffer(int x, int y, Uint32 value)
{
    int index = ( y * EngineSetup::getInstance()->SCREEN_WIDTH ) + x;
    videoBuffer[ index ] = value;
}

float EngineBuffers::getVideoBuffer(int x, int y)
{
    return videoBuffer[( y * EngineSetup::getInstance()->SCREEN_WIDTH ) + x ];
}

void EngineBuffers::clearVideoBuffer()
{
    std::fill(videoBuffer, videoBuffer + sizeBuffers, NULL);
}

void EngineBuffers::flipVideoBuffer(SDL_Surface *surface)
{
    Uint32 *pixels = (Uint32 *)surface->pixels;

    for (int i = 0 ; i < sizeBuffers ; i++) {
        pixels[i] = videoBuffer[i];
    }
}

void EngineBuffers::resetBenchmarkValues()
{
    EngineBuffers::getInstance()->pixelesDrawed = 0;
    EngineBuffers::getInstance()->pixelesOutOfWindow = 0;
    EngineBuffers::getInstance()->trianglesOutFrustum = 0;
    EngineBuffers::getInstance()->trianglesHidenByFaceCuling = 0;
    EngineBuffers::getInstance()->pixelesBehindOfCamera = 0;
    EngineBuffers::getInstance()->trianglesClippingCreated = 0;
}

void EngineBuffers::consoleInfo()
{
    std::string info;
    info+= "pxDrawed: " + std::to_string(EngineBuffers::getInstance()->pixelesDrawed);
    info+= "| pxOutOfCamera: " + std::to_string(EngineBuffers::getInstance()->pixelesOutOfWindow);
    info+= "| pxBehindOfCamera: " + std::to_string(EngineBuffers::getInstance()->pixelesBehindOfCamera);
    info+= "| trOutFrustum: " + std::to_string(EngineBuffers::getInstance()->trianglesOutFrustum);
    info+= "| trHByFaceCuling: " + std::to_string(EngineBuffers::getInstance()->trianglesHidenByFaceCuling);
    info+= "| trClippingCreated: " + std::to_string(EngineBuffers::getInstance()->trianglesClippingCreated);

    Logging::getInstance()->Log( info, "INFO" );
}
