#include <cstdio>
#include "headers/Game/Game.h"
#include "headers/Brakeza3D.h"
#include "headers/Demo.h"

#pragma pack(push, MAIN)
#include <cstdlib>
#pragma pack(pop, MAIN)

int main( int argc, const char* args[] ) {

    Brakeza3D::get()->start();
    Game::get()->start();
    //Demo *demo = new Demo();

    return 0;
}