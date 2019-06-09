#ifndef SDL2_3D_ENGINE_ZBUFFER_H
#define SDL2_3D_ENGINE_ZBUFFER_H

#include <iostream>
#include <SDL_surface.h>
#include "Timer.h"

typedef struct {
    float As_x, As_y;
    float Bs_x, Bs_y;
    float Cs_x, Cs_y;

    float An_x, An_y, An_z;
    float Bn_x, Bn_y, Bn_z;
    float Cn_x, Cn_y, Cn_z;

    float persp_correct_Az;
    float persp_correct_Bz;
    float persp_correct_Cz;

    float tex_u1_Ac_z;
    float tex_u2_Bc_z;
    float tex_u3_Cc_z;
    float tex_v1_Ac_z;
    float tex_v2_Bc_z;
    float tex_v3_Cc_z;

    float light_u1_Ac_z;
    float light_u2_Bc_z;
    float light_u3_Cc_z;
    float light_v1_Ac_z;
    float light_v2_Bc_z;
    float light_v3_Cc_z;

    bool isAnimated;
    bool isLightmapped;
} OCLTriangle;

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

    SDL_Surface  *screenSurface;

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
