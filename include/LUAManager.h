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
#include "Misc/SharedLUAContext.h"
#include "Misc/TileManager.h"

// Singleton
class LUAManager {

private:
    sol::state lua;
    static LUAManager *instance;

    LUAManager();
    SharedLUAContext *sharedLuaContext;

public:

    static LUAManager *get();

    char text[256];

    sol::state &getLua();

    void initLUATypes();
};

#endif
