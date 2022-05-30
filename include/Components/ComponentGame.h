
#ifndef BRAKEDA3D_COMPONENTGAME_H
#define BRAKEDA3D_COMPONENTGAME_H


#include "Component.h"
#include "../../darkheaz/Player.h"
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
#include "../../darkheaz/FaderToGameStates.h"
#include "../../darkheaz/LevelLoader.h"
#include "../Particles/ParticleEmissorFireworks.h"
#include "../../darkheaz/src/shaders/ShaderBackgroundGame.h"

class ComponentGame : public Component {
public:
    ComponentGame();

    void onStart() override;

    void preUpdate() override;

    void onUpdate() override;

    void postUpdate() override;

    void onEnd() override;

    void onSDLPollEvent(SDL_Event *event, bool &finish) override;

    [[nodiscard]] Player *getPlayer() const;

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

    ShaderBackgroundGame *shaderBackground;
    ShaderShockWave *shaderShockWave;

    void setGameState(EngineSetup::GameState state);
    EngineSetup::GameState getGameState();
    void selectClosestObject3DFromPlayer();
    Object3D *getClosesObject3DFromPosition(Vertex3D to, bool skipPlayer, bool skipCurrentSelected);

    [[nodiscard]] FaderToGameStates *getFadeToGameState() const;

    int Z_COORDINATE_GAMEPLAY = 10000;
private:

    EngineSetup::GameState gameState;
    std::vector<Weapon *> weapons;

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
    [[nodiscard]] LevelLoader *getLevelInfo() const;

    void checkForEndLevel();

    void removeProjectiles() const;

    void makeFadeToGameState(EngineSetup::GameState gameState) const;

    void setVisibleInGameObjects(bool value);
    void removeInGameObjects();
    void loadWeapons();

    [[nodiscard]] const std::vector<Weapon *> &getWeapons() const;

    void pressedKeyForNewGame();

    void pressedKeyForBeginLevel();

    void pressedKeyForFinishGameAndRestart();

    void pressedKeyByDead();

    int getLiveEnemiesCounter();

    void silenceInGameObjects();
};


#endif //BRAKEDA3D_COMPONENTGAME_H
