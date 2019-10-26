#include <iostream>

//Using SDL and standard IO
#include <SDL.h>
#include <cstdio>


#include "Game.h"
#include "headers/Brakeza3D.h"

#pragma pack(push, MAIN)
#include <cstdlib>
#pragma pack(pop, MAIN)

typedef float vec3_t[3];

int main( int argc, const char* args[] ) {

    Brakeza3D::get()->start();
    Game *engine = new Game();

    if (Brakeza3D::get()->initWindow() ) {
        engine->run();
    } else {
        std::cout << "Error iniciando el game" << std::endl;
    }

    printf("\r\nBrakeza3D exit, good bye ;)");
    return 0;
}