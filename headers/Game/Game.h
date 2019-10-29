#ifndef SDL2_3D_ENGINE_GAME_H
#define SDL2_3D_ENGINE_GAME_H

#include "../Render/Engine.h"
#include "../../src/Game/Player.h"
#include "GameInputController.h"

class Game: public Engine {
public:
    Player *player;
    GameInputController *controller;
    std::vector<Texture*> HUDTextures;
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

    void resolveCollisions();

    void drawHUD();
    void redScreen();
    void drawMenuScreen();

};


#endif //SDL2_3D_ENGINE_GAME_H
