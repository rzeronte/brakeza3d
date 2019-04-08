
#include <SDL_surface.h>
#include "../../headers/Render/EngineBuffers.h"
#include "../../headers/Render/EngineSetup.h"
#include "../../headers/Render/Tools.h"
#include "../../headers/Render/Timer.h"
#include "../../headers/Render/Logging.h"

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
    std::fill(depthBuffer, depthBuffer + sizeBuffers, 999999);
}

float EngineBuffers::getDepthBuffer(int x, int y)
{
    return depthBuffer[( y * EngineSetup::getInstance()->SCREEN_WIDTH ) + x];
}

float EngineBuffers::getDepthBuffer(int i)
{
    return depthBuffer[i];
}

void EngineBuffers::setDepthBuffer(int x, int y, float value)
{
    depthBuffer[( y * EngineSetup::getInstance()->SCREEN_WIDTH ) + x] = value;
}

void EngineBuffers::setDepthBuffer(const int i, const float value)
{
    depthBuffer[i] = value;
}

void EngineBuffers::setVideoBuffer(const int x, const int y, Uint32 value)
{
    videoBuffer[ ( y * EngineSetup::getInstance()->SCREEN_WIDTH ) + x ] = value;
}

void EngineBuffers::setVideoBuffer(const int i, Uint32 value)
{
    videoBuffer[ i ] = value;
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
    memcpy (&surface->pixels, &videoBuffer, sizeof(surface->pixels));
}

void EngineBuffers::resetBenchmarkValues()
{
    EngineBuffers::getInstance()->trianglesDrawed = 0;
    EngineBuffers::getInstance()->pixelesDrawed = 0;
    EngineBuffers::getInstance()->pixelesOutOfWindow = 0;
    EngineBuffers::getInstance()->trianglesOutFrustum = 0;
    EngineBuffers::getInstance()->trianglesHidenByFaceCuling = 0;
    EngineBuffers::getInstance()->pixelesBehindOfCamera = 0;
    EngineBuffers::getInstance()->trianglesClippingCreated = 0;
    EngineBuffers::getInstance()->trianglesHitted = 0;
}

void EngineBuffers::consoleInfo()
{
    std::string info;
    info+= "TrDrawed: " + std::to_string(EngineBuffers::getInstance()->trianglesDrawed);
    info+= "| TrHByFaceCuling: " + std::to_string(EngineBuffers::getInstance()->trianglesHidenByFaceCuling);
    info+= "| TrOutFrustum: " + std::to_string(EngineBuffers::getInstance()->trianglesOutFrustum);
    info+= "| TrClipCreated: " + std::to_string(EngineBuffers::getInstance()->trianglesClippingCreated);
    info+= "| pxDrawed: " + std::to_string(EngineBuffers::getInstance()->pixelesDrawed);
    info+= "| pxOutOfWin: " + std::to_string(EngineBuffers::getInstance()->pixelesOutOfWindow);
    info+= "| pxBehindOfCam: " + std::to_string(EngineBuffers::getInstance()->pixelesBehindOfCamera);
    info+= "| trHit: " + std::to_string(EngineBuffers::getInstance()->trianglesHitted);

    Logging::getInstance()->Log( info, "INFO" );
}

SDL_Surface *EngineBuffers::getScreenSurface() const
{
    return screenSurface;
}

void EngineBuffers::setScreenSurface(SDL_Surface *screenSurface)
{
    EngineBuffers::screenSurface = screenSurface;
}
