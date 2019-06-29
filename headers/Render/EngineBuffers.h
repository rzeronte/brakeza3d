#ifndef SDL2_3D_ENGINE_ZBUFFER_H
#define SDL2_3D_ENGINE_ZBUFFER_H

#include <iostream>
#include <SDL_surface.h>
#include "Timer.h"

typedef struct {
    int id;

    float A_x, A_y, A_z;
    float B_x, B_y, B_z;
    float C_x, C_y, C_z;

    float Ao_x, Ao_y, Ao_z;
    float Bo_x, Bo_y, Bo_z;
    float Co_x, Co_y, Co_z;

    float Ac_x, Ac_y, Ac_z;
    float Bc_x, Bc_y, Bc_z;
    float Cc_x, Cc_y, Cc_z;

    float An_x, An_y, An_z;
    float Bn_x, Bn_y, Bn_z;
    float Cn_x, Cn_y, Cn_z;

    float As_x, As_y;
    float Bs_x, Bs_y;
    float Cs_x, Cs_y;

    float o_x;
    float o_y;
    float o_z;

    float o_scale;

    float oPitch;
    float oYaw;
    float oRoll;

    int maxX,  minX, maxY,minY;

    bool is_clipping;
} OCLTriangle;

struct OCLPlane {
    float A[3];
    float B[3];
    float C[3];
    float normal[3];
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
