
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
#include "../Shaders/ShaderTintScreen.h"
#include "../Physics/Mesh3DBody.h"
#include "../../darkheaz/FaderToGameStates.h"
#include "../../darkheaz/LevelLoader.h"
#include "../Particles/ParticleEmissorFireworks.h"
#include "../Misc/VideoPlayer.h"
#include "../../darkheaz/src/shaders/ShaderClouds.h"
#include "../../darkheaz/src/shaders/ShaderHorizontalBar.h"
#include "../../darkheaz/src/shaders/ShaderBackgroundImage.h"
#include "../../darkheaz/src/shaders/ShaderEdgeObject.h"
#include "../../darkheaz/src/shaders/ShaderTrailObject.h"
#include "../../darkheaz/src/shaders/ShaderTrailBuffer.h"
#include "../../darkheaz/src/shaders/ShaderLaser.h"
#include "../Physics/RayCollisionable.h"
#include "../Physics/ProjectileRay.h"
#include "../../darkheaz/src/shaders/ShaderProjectiles.h"

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
    void zoomCameraCountDown();

    void reloadLevel(int level);

    Vertex3D playerStartPosition;
    Vertex3D cameraCountDownPosition;
    Vertex3D cameraInGamePosition;

    FaderToGameStates *fadeToGameState;
    Player *player;
    ShaderProjectiles *shaderLasers;

    Sprite3D *explosion;
    PathFinder *pathFinder;

    Image *imageCredits;
    Image *imageHelp;
    Image *imageSplash;
    Counter splashCounter;

    LevelLoader *levelInfo;

    ShaderClouds *shaderClouds;
    ShaderImage *shaderBackgroundImage;
    ShaderColor *shaderColor;
    ShaderTrailBuffer *shaderTrailBuffer;
    ShaderEdgeObject *shaderEdge;
    Image *imageCrossFire;
    Vertex3D spaceCrossFirePosition;
    Point2D imageCrossFireScreenPosition;

    void setGameState(EngineSetup::GameState state);
    EngineSetup::GameState getGameState();
    void selectClosestObject3DFromPlayer();
    Object3D *getClosesObject3DFromPosition(Vertex3D to, bool skipPlayer, bool skipCurrentSelected);
    Object3D *getClosesObject3DDirection(Vertex3D from, Vertex3D direction, bool skipPlayer, bool skipCurrentSelected) const;

    [[nodiscard]] FaderToGameStates *getFadeToGameState() const;

    int Z_COORDINATE_GAMEPLAY = 10000;
private:

    EngineSetup::GameState gameState;
    std::vector<Weapon *> weapons;

    void loadLevels();

public:
    [[nodiscard]] LevelLoader *getLevelInfo() const;

    void checkForEndLevel();

    void removeProjectiles() const;

    void makeFadeToGameState(EngineSetup::GameState value) const;

    void setVisibleInGameObjects(bool value);
    void setEnemyWeaponsEnabled(bool value);

    void removeInGameObjects();
    void loadWeapons();

    [[nodiscard]] const std::vector<Weapon *> &getWeapons() const;

    void pressedKeyForNewGame();

    void pressedKeyForBeginLevel();

    void pressedKeyForFinishGameAndRestart();

    void pressedKeyByDead();

    int getLiveEnemiesCounter();

    void silenceInGameObjects();

    void addObjectsToStencilBuffer();

    void updateShaders();

    void handleMenuGameState();

    void handleGamingGameState();

    void handleCountDownGameState();

    void handlePressNewLevelKeyGameState();

    void handlePressKeyByDead();

    void handlePressKeyPreviousLevel();

    void handlePressKeyGameOver();

    void handlePressKeyCredits();

    Color primaryColor = Color(179, 0, 40);
    Color secondaryColor = Color(0, 179, 52);
    Color thirdColor = Color(0, 0, 255);

    void updateCrossFire();
};


#endif //BRAKEDA3D_COMPONENTGAME_H
