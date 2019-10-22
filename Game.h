#ifndef SDL2_3D_ENGINE_GAME_H
#define SDL2_3D_ENGINE_GAME_H

#include "headers/Render/Engine.h"
#include "src/Objects/Player3D.h"

class Game: public Engine {
public:
    Player3D *player;
    Game();
public:
    void run();
    void mainLoop();
    void onStart();
    void preUpdate();
    void onUpdate();
    void onEnd();

    void onUpdateIA();

    void loadDemoObjects();
};


#endif //SDL2_3D_ENGINE_GAME_H
