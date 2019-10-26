#ifndef SDL2_3D_ENGINE_GAME_H
#define SDL2_3D_ENGINE_GAME_H

#include "Render/Engine.h"
#include "../src/Objects/Player3D.h"

class Game: public Engine {
public:
    Player3D *player;
    Game();
public:
    static Game* get();
    static Game* instance;

    void start();

    void mainLoop();
    void onStart();
    void preUpdate();
    void onUpdate();
    void onEnd();

    void onUpdateInputController();
    void onUpdateIA();
};


#endif //SDL2_3D_ENGINE_GAME_H
