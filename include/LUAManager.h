#ifndef SDL2_3D_ENGINE_ZBUFFER_H
#define SDL2_3D_ENGINE_ZBUFFER_H

#include "Misc/SharedLUAContext.h"
#include "../sol/sol.hpp"

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
