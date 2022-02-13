
#ifndef BRAKEDA3D_COMPONENTGAME_H
#define BRAKEDA3D_COMPONENTGAME_H


#include "Component.h"
#include "../../src/Game/Player.h"
#include "../Misc/cJSON.h"
#include "../Physics/Mesh3DGhost.h"
#include "../Misc/Octree.h"
#include "../Misc/Grid3D.h"
#include "../Misc/PathFinder.h"
#include "../Shaders/ShaderWater.h"
#include "../Shaders/ShaderFire.h"

class ComponentGame : public Component {
public:
    ComponentGame();

    void onStart();

    void preUpdate();

    void onUpdate();

    void postUpdate();

    void onEnd();

    void onSDLPollEvent(SDL_Event *event, bool &finish);

    Player *player;
    Mesh3DBody *plane;

    bool direction = true;

    PathFinder *pathFinder;

    int kills = 0;

    Player *getPlayer() const;

    void onUpdateIA() const;

    void resolveCollisions();

    void redScreen();

    void setKills(int kills);

    int getKills() const;

    void loadObjects3D();

    ShaderWater shaderWater;
    ShaderFire shaderFire;
};


#endif //BRAKEDA3D_COMPONENTGAME_H
