
#include <SDL_surface.h>
#include "../../headers/Render/EngineBuffers.h"
#include "../../headers/Render/EngineSetup.h"
#include "../../headers/Render/Tools.h"
#include "../../headers/Render/Timer.h"
#include "../../headers/Render/Logging.h"
#include "../../headers/Render/Engine.h"

extern Engine *brakeza3D;

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
    sizeBuffers = EngineSetup::getInstance()->screenWidth * EngineSetup::getInstance()->screenHeight;

    Logging::getInstance()->Log("EngineBuffers size: " + std::to_string(sizeBuffers), "INFO");

    depthBuffer = new float[sizeBuffers];
    videoBuffer = new Uint32[sizeBuffers];
    //fragmentsBuffer = new pixelFragment[EngineSetup::getInstance()->ENGINE_MAX_FRAGMENTS];
    OCLTrianglesBuffer = new OCLTriangle[EngineSetup::getInstance()->ENGINE_MAX_OCLTRIANGLES];

}

void EngineBuffers::clearDepthBuffer()
{
    std::fill(depthBuffer, depthBuffer + sizeBuffers, 10000);
}

float EngineBuffers::getDepthBuffer(int x, int y)
{
    return depthBuffer[( y * EngineSetup::getInstance()->screenWidth ) + x];
}

float EngineBuffers::getDepthBuffer(int i)
{
    return depthBuffer[i];
}

void EngineBuffers::setDepthBuffer(int x, int y, float value)
{
    depthBuffer[( y * EngineSetup::getInstance()->screenWidth ) + x] = value;
}

void EngineBuffers::setDepthBuffer(const int i, const float value)
{
    depthBuffer[i] = value;
}

void EngineBuffers::setVideoBuffer(const int x, const int y, Uint32 value)
{
    videoBuffer[( y * EngineSetup::getInstance()->screenWidth ) + x ] = value;
}

void EngineBuffers::setVideoBuffer(const int i, Uint32 value)
{
    videoBuffer[ i ] = value;
}

void EngineBuffers::addOCLTriangle(OCLTriangle oclt)
{
    OCLTrianglesBuffer[numOCLTriangles] = oclt;
    numOCLTriangles++;
}

float EngineBuffers::getVideoBuffer(int x, int y)
{
    return videoBuffer[( y * EngineSetup::getInstance()->screenWidth ) + x ];
}

void EngineBuffers::clearVideoBuffer()
{
    std::fill(videoBuffer, videoBuffer + sizeBuffers, NULL);
}

void EngineBuffers::flipVideoBuffer(SDL_Surface *surface)
{
    memcpy (&surface->pixels, &videoBuffer, sizeof(surface->pixels));
}

