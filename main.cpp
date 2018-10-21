#include <iostream>

//Using SDL and standard IO
#include <SDL.h>
#include <cstdio>

#include "headers/Objects/Camera3D.h"
#include "headers/Render/Controller.h"
#include "headers/Objects/Mesh3D.h"
#include "headers/Render/Color.h"
#include "headers/Render/Engine.h"
#include "headers/Q3MapLoader/Q3Loader.h"
#include "headers/Render/EngineBuffers.h"
#include "Game.h"

#pragma pack(push, MAIN)
#include <cstdlib>
#pragma pack(pop, MAIN)

int main( int argc, const char* args[] ) {

    Game *engine = new Game();

    if (engine->initWindow() ) {
        engine->run();
    } else {
        std::cout << "Error iniciando el game" << std::endl;
    }

    printf("\r\nEnds");
    return 0;
}