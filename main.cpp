#include <iostream>

//Using SDL and standard IO
#include <SDL.h>
#include <cstdio>

#include "headers/Objects/Camera3D.h"
#include "headers/Render/Controller.h"
#include "headers/Objects/Mesh3D.h"
#include "headers/Render/Color.h"
#include "headers/Render/Engine.h"
#include "headers/Render/EngineBuffers.h"

#include "Game.h"

#pragma pack(push, MAIN)
#include <cstdlib>
#pragma pack(pop, MAIN)

typedef float vec3_t[3];

int main( int argc, const char* args[] ) {

    Game *engine = new Game();

    if (engine->initWindow() ) {
        engine->run();
    } else {
        std::cout << "Error iniciando el game" << std::endl;
    }

    printf("\r\nBrakeza3D exit, good bye ;)");
    return 0;
}