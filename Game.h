#ifndef SDL2_3D_ENGINE_GAME_H
#define SDL2_3D_ENGINE_GAME_H


#include "headers/Render/Engine.h"

class Game: public Engine {
public:
    Game();
public:
    void run();
    void mainLoop();
    void onStart();
    void preUpdate();
    void onUpdate();
    void onEnd();

    void loadDemoObjects();
};


#endif //SDL2_3D_ENGINE_GAME_H
