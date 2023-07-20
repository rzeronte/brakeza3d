#ifndef SDL2_3D_ENGINE_ZBUFFER_H
#define SDL2_3D_ENGINE_ZBUFFER_H

#include <iostream>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_audio.h>
#include <SDL2/SDL_mixer.h>
#include <CL/cl.h>
#include "Misc/Timer.h"
#include "Objects/Sprite3D.h"
#include "Misc/SoundPackage.h"

// Singleton
class EngineBuffers {

private:
    static EngineBuffers *instance;
    std::vector<OCParticle> particles;

    EngineBuffers();

public:
    static EngineBuffers *get();

    float *depthBuffer;
    Uint32 *videoBuffer;
    int sizeBuffers;

    char text[256];

    int widthVideoBuffer = EngineSetup::get()->screenWidth;

    cl_mem videoBufferOCL;
    cl_mem depthBufferOCL;

    void clearDepthBuffer() const;

    [[nodiscard]] float getDepthBuffer(int x, int y) const;

    [[nodiscard]] float getDepthBuffer(int i) const;

    void setDepthBuffer(int x, int y, float value) const;

    void setDepthBuffer(int i, float value) const;

    [[nodiscard]] uint32_t getVideoBuffer(int x, int y) const;

    [[nodiscard]] uint32_t getVideoBuffer(int bufferIndex) const;

    void clearVideoBuffer() const;

    void setVideoBuffer(const int x, const int y, Uint32 value) const;

    void setVideoBuffer(const int i, Uint32 value) const;

    void flipVideoBufferToSurface(SDL_Surface *);

    void createOpenCLBuffers(_cl_context *context, cl_command_queue &queue);

    void loadParticlesEmptyBuffer();

    const std::vector<OCParticle> &getParticles() const;
};

#endif
