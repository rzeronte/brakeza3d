#include <iostream>

//Using SDL and standard IO
#include <SDL.h>
#include <cstdio>

#include "headers/Camera.h"
#include "headers/Controller.h"
#include "headers/Mesh.h"
#include "headers/Color.h"
#include "headers/Engine.h"
#include "headers/Q3Loader.h"
#include "headers/EngineBuffers.h"
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