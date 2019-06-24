#ifndef SDL2_3D_ENGINE_ZBUFFER_H
#define SDL2_3D_ENGINE_ZBUFFER_H

#include <iostream>
#include <SDL_surface.h>
#include "Timer.h"

typedef struct {
    int id;

    float As_x, As_y;
    float Bs_x, Bs_y;
    float Cs_x, Cs_y;

    float An_z;
    float Bn_z;
    float Cn_z;
} OCLTriangle;

struct pixelFragment {
    int x;
    int y;
    int depth;
    int triangleId;
    int w0;
    int w1;
    int w2;
};

// Singleton
class EngineBuffers {

private:
    static EngineBuffers* instance;

    EngineBuffers();
    ~EngineBuffers();

public:

    float *depthBuffer;
    Uint32 *videoBuffer;
    int sizeBuffers;

    OCLTriangle *OCLTrianglesBuffer;
    int numOCLTriangles = 0;

    pixelFragment *fragmentsBuffer;
    int numFragments = 0;

    static EngineBuffers* getInstance();

    // Benchmark Meshes
    int pixelesDrawed = 0;
    int pixelesOutOfWindow = 0;
    int pixelesBehindOfCamera = 0;
    int trianglesDrawed = 0;
    int trianglesHidenByFaceCuling = 0;
    int trianglesOutFrustum = 0;
    int trianglesClippingCreated = 0;

    int trianglesHitted = 0;

    void clearDepthBuffer();
    float getDepthBuffer(int x, int y);
    float getDepthBuffer(int i);

    void setDepthBuffer(const int x, const int y, const float value);
    void setDepthBuffer(const int i, const float value);

    float getVideoBuffer(int x, int y);
    void clearVideoBuffer();
    void setVideoBuffer(const int x, const int y, Uint32 value);
    void setVideoBuffer(const int i, Uint32 value);
    void flipVideoBuffer(SDL_Surface *);

    void setNumOCLTriangles(int num);
    void addOCLTriangle(OCLTriangle);

    void resetBenchmarkValues();

    void consoleInfo();

};

#endif
