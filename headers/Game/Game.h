#ifndef SDL2_3D_ENGINE_GAME_H
#define SDL2_3D_ENGINE_GAME_H

#include "../Render/Engine.h"
#include "../../src/Game/Player.h"
#include "GameInputController.h"
#include "../2D/TexturePackage.h"

class Game: public Engine {
public:
    Player *player;

    int kills = 0;

    GameInputController *controller;
    TexturePackage *HUDTextures;

    // JSON for maps and weapons
    cJSON *mapsJSONList;
    cJSON *weaponsJSONList;
    cJSON *enemiesJSONList;

    Game();

public:
    static Game* get();
    static Game* instance;

    void start();

    void loadHUDImages();

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

    // JSON setup Parsers
    void getMapsJSON();
    void getWeaponsJSON();
    void getEnemiesJSON();

    void initBSP();

};


#endif //SDL2_3D_ENGINE_GAME_H
