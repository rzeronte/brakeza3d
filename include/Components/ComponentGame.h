
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
#include "../Shaders/ShaderImageBackground.h"
#include "../Shaders/ShaderTintScreen.h"

class ComponentGame : public Component {
public:
    ComponentGame();

    void onStart();

    void preUpdate();

    void onUpdate();

    void postUpdate();

    void onEnd();

    void onSDLPollEvent(SDL_Event *event, bool &finish);

    Player *getPlayer() const;

    void onUpdateIA() const;

    void resolveCollisions();

    void loadObjects3D();

    void autoScroll();

    ShaderWater shaderWater;
    ShaderFire shaderFire;
    ShaderImageBackground shaderImageBackground;
    ShaderTintScreen shaderTintScreen;

    float shaderYScroll = 0;

    Player *player;
    Mesh3DBody *plane;

    bool direction = true;

    PathFinder *pathFinder;

private:
};


#endif //BRAKEDA3D_COMPONENTGAME_H
