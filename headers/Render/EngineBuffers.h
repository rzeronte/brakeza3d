#ifndef SDL2_3D_ENGINE_ZBUFFER_H
#define SDL2_3D_ENGINE_ZBUFFER_H

#include <iostream>
#include <SDL_surface.h>
#include <SDL_audio.h>
#include <SDL_mixer.h>
#include "Timer.h"
#include "../Objects/SpriteDirectional3D.h"

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

    float normal[3];

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

    int *firePixelsBuffer;

    OCLTriangle *OCLTrianglesBuffer;
    int numOCLTriangles = 0;

    // sounds
    Mix_Music *snd_base_menu;       // menu
    Mix_Chunk *snd_weapon_1;    // weapon 1 fire

    Mix_Music *snd_base_level_0;     // base sound level 0

    // Fire colors for menu fire effect
    Uint32 fireColors[37];
    int rgbs[111] = {
        0x07, 0x07, 0x07,
        0x1F, 0x07, 0x07,
        0x2F, 0x0F, 0x07,
        0x47, 0x0F, 0x07,
        0x57, 0x17, 0x07,
        0x67, 0x1F, 0x07,
        0x77, 0x1F, 0x07,
        0x8F, 0x27, 0x07,
        0x9F, 0x2F, 0x07,
        0xAF, 0x3F, 0x07,
        0xBF, 0x47, 0x07,
        0xC7, 0x47, 0x07,
        0xDF, 0x4F, 0x07,
        0xDF, 0x57, 0x07,
        0xDF, 0x57, 0x07,
        0xD7, 0x5F, 0x07,
        0xD7, 0x5F, 0x07,
        0xD7, 0x67, 0x0F,
        0xCF, 0x6F, 0x0F,
        0xCF, 0x77, 0x0F,
        0xCF, 0x7F, 0x0F,
        0xCF, 0x87, 0x17,
        0xC7, 0x87, 0x17,
        0xC7, 0x8F, 0x17,
        0xC7, 0x97, 0x1F,
        0xBF, 0x9F, 0x1F,
        0xBF, 0x9F, 0x1F,
        0xBF, 0xA7, 0x27,
        0xBF, 0xA7, 0x27,
        0xBF, 0xAF, 0x2F,
        0xB7, 0xAF, 0x2F,
        0xB7, 0xB7, 0x2F,
        0xB7, 0xB7, 0x37,
        0xCF, 0xCF, 0x6F,
        0xDF, 0xDF, 0x9F,
        0xEF, 0xEF, 0xC7,
        0xFF, 0xFF, 0xFF
    };

    static EngineBuffers* getInstance();

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

    void makeFireColors();
    void fireShaderSetup();

    void loadWAVs();
};

#endif
