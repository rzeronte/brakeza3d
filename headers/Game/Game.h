#ifndef SDL2_3D_ENGINE_GAME_H
#define SDL2_3D_ENGINE_GAME_H

#include "../Render/Engine.h"
#include "../../src/Game/Player.h"
#include "GameInputController.h"
#include "../Misc/TexturePackage.h"

class Game: public Engine {
public:
    Player *player;

    int kills = 0;
    Counter counterWalkOnWater;
    int gameLeafType = 0;
    bool changeLeafType;

    GameInputController *controller;


    Game();

public:
    static Game* get();
    static Game* instance;

    void start();

    GameInputController *getController() const;

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

    void initBSP();

};


#endif //SDL2_3D_ENGINE_GAME_H
