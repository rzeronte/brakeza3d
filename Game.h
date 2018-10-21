//
// Created by darkhead on 13/8/18.
//

#ifndef SDL2_3D_ENGINE_GAME_H
#define SDL2_3D_ENGINE_GAME_H


#include "headers/Render/Engine.h"

class Game : public Engine {
public:
    void run();
    void mainLoop();
    void onStart();
    void onUpdate();
    void onUpdateEvent();
    void onEnd();
};


#endif //SDL2_3D_ENGINE_GAME_H
