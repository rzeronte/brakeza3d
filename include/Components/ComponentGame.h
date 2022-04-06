
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
#include "../Shaders/ShaderObjectSilhouette.h"
#include "../Physics/Mesh3DBody.h"
#include "../Game/FaderToGameStates.h"
#include "../Game/LevelLoader.h"
#include "../Particles/ParticleEmissorFireworks.h"

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

    void loadPlayer();

    void blockPlayerPositionInCamera();

    Vertex3D playerStartPosition;
    FaderToGameStates *fadeToGameState;
    Player *player;
    PathFinder *pathFinder;

    Image *imageCredits;
    Image *imageHelp;
    Image *imageSplash;
    Counter splashCounter;

    LevelLoader *levelInfo;
    Vertex3D shaderAutoScrollSpeed;

    void setGameState(EngineSetup::GameState state);
    EngineSetup::GameState getGameState();
    void selectClosestObject3DFromPlayer();
    Object3D *getClosesObject3DFromPosition(Vertex3D to, bool skipPlayer, bool skipCurrentSelected);

    FaderToGameStates *getFadeToGameState() const;

    int Z_COORDINATE_GAMEPLAY = 10000;
private:

    EngineSetup::GameState gameState;
    std::vector<Weapon *> weapons;

    void evalStatusMachine(EnemyGhost *pGhost) const;

    void loadLevels();

    void loadBackgroundImageShader();

    void stopBackgroundShader();
    void startBackgroundShader();
    void startTintScreenShader();
    void stopTintScreenShader();
    void startSilhouetteShader();
    void stopSilhouetteShader();
    void startWaterShader();
    void stopWaterShader();

public:
    LevelLoader *getLevelInfo() const;

    void checkForEndLevel() const;

    void removeProjectiles() const;

    void makeFadeToGameState(EngineSetup::GameState gameState) const;

    void setVisibleInGameObjects(bool value);
    void removeInGameObjects();
    void loadWeapons();
};


#endif //BRAKEDA3D_COMPONENTGAME_H
