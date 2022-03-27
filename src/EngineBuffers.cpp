
#include <SDL2/SDL_surface.h>
#include "../include/EngineBuffers.h"

EngineBuffers *EngineBuffers::instance = nullptr;

EngineBuffers *EngineBuffers::getInstance() {
    if (instance == nullptr) {
        instance = new EngineBuffers();
    }

    return instance;
}

EngineBuffers::EngineBuffers() {
    EngineSetup *setup = EngineSetup::get();

    sizeBuffers = setup->RESOLUTION;

    depthBuffer = new float[sizeBuffers];
    videoBuffer = new Uint32[sizeBuffers];

    soundPackage = new SoundPackage();
}

void EngineBuffers::clearDepthBuffer() const {
    std::fill(depthBuffer, depthBuffer + sizeBuffers, 90000);
}

float EngineBuffers::getDepthBuffer(int x, int y) const {
    return depthBuffer[y * this->widthVideoBuffer + x];
}

float EngineBuffers::getDepthBuffer(int i) const {
    return depthBuffer[i];
}

void EngineBuffers::setDepthBuffer(int x, int y, float value) const {
    depthBuffer[y * this->widthVideoBuffer + x] = value;
}

void EngineBuffers::setDepthBuffer(const int i, const float value) const {
    depthBuffer[i] = value;
}

void EngineBuffers::setVideoBuffer(const int x, const int y, Uint32 value) const {
    videoBuffer[y * this->widthVideoBuffer + x] = value;
}

void EngineBuffers::setVideoBuffer(const int i, Uint32 value) const {
    videoBuffer[i] = value;
}

uint32_t EngineBuffers::getVideoBuffer(int x, int y) const {
    return videoBuffer[y * this->widthVideoBuffer + x];
}

uint32_t EngineBuffers::getVideoBuffer(int bufferIndex) const
{
    return videoBuffer[bufferIndex];
}

void EngineBuffers::clearVideoBuffer() const {
    if (EngineSetup::get()->ENABLE_FOG) {
        std::fill(videoBuffer, videoBuffer + sizeBuffers, EngineSetup::get()->FOG_COLOR.getColor());
        return;
    }

    std::fill(videoBuffer, videoBuffer + sizeBuffers, NULL);
}

void EngineBuffers::flipVideoBufferToSurface(SDL_Surface *surface) {
    // buffer -> surface
    memcpy(&surface->pixels, &videoBuffer, sizeof(surface->pixels));
}