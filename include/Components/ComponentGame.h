
#ifndef BRAKEDA3D_COMPONENTGAME_H
#define BRAKEDA3D_COMPONENTGAME_H


#include "Component.h"
#include "../../darkheaz/Player.h"
#include "../Misc/cJSON.h"
#include "../Physics/Mesh3DGhost.h"
#include "../Misc/Octree.h"
#include "../Misc/Grid3D.h"
#include "../Misc/PathFinder.h"
#include "../Physics/Mesh3DBody.h"
#include "../../darkheaz/FaderToGameStates.h"
#include "../../darkheaz/LevelLoader.h"
#include "../Particles/ParticleEmitterFireworks.h"
#include "../Misc/VideoPlayer.h"
#include "../../darkheaz/src/shaders/ShaderImage.h"
#include "../../darkheaz/src/shaders/ShaderEdgeObject.h"
#include "../../darkheaz/src/shaders/RayLight.h"
#include "../Physics/RayCollisionable.h"
#include "../Physics/ProjectileRay.h"
#include "../../darkheaz/src/shaders/ShaderProjectiles.h"
#include "../2D/TextWriter.h"
#include "../../darkheaz/src/shaders/ShaderParticles.h"
#include "../../darkheaz/src/shaders/ShaderBilinear.h"
#include "../Objects/Swarm.h"
#include "../../darkheaz/src/shaders/ShaderSwarm.h"
#include "../../darkheaz/src/shaders/ShaderExplosion.h"

#define Z_COORDINATE_GAMEPLAY 10000

class ComponentGame : public Component {
private:
    Vertex3D playerStartPosition;
    Vertex3D cameraCountDownPosition;
    Vertex3D cameraInGamePosition;

    TextWriter *textWriter;

    FaderToGameStates *fadeToGameState;
    Player *player;
    ShaderProjectiles *shaderLasers;

    Sprite3D *explosionSpriteTemplate;

    Image *imageCredits;
    Image *imageHelp;
    Image *imageSplash;
    Image *imageCrossFire;

    Counter splashCounter;

    LevelLoader *levelLoader;

    ShaderImage *shaderBackgroundImage;
    ShaderColor *shaderColor;
    ShaderShockWave *shaderShockWave;

    ShaderExplosion *shaderParticles;

    Vertex3D spaceCrossFirePosition;
    Point2D imageCrossFireScreenPosition;

    EngineSetup::GameState gameState;
    std::vector<Weapon *> weapons;

    Color primaryColor;
    Color secondaryColor;
    Color thirdColor;

    Swarm *swarm;
    ShaderSwarm *shaderSwarm;

    //PathFinder *pathFinder;
public:
    ComponentGame();

    virtual ~ComponentGame();

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

    void setGameState(EngineSetup::GameState state);

    EngineSetup::GameState getGameState();

    void selectClosestObject3DFromPlayer();

    Object3D *getClosesObject3DFromPosition(Vertex3D to, bool skipPlayer, bool skipCurrentSelected);

    void loadLevels();

    void checkForEndLevel();

    void removeProjectiles() const;

    void makeFadeToGameState(EngineSetup::GameState value) const;

    void setVisibleInGameObjects(bool value);

    void setEnemyWeaponsEnabled(bool value);

    void removeInGameObjects();

    void loadWeapons();

    void pressedKeyForNewGame();

    void pressedKeyForBeginLevel();

    void pressedKeyForFinishGameAndRestart();

    void pressedKeyByDead();

    int getLiveEnemiesCounter();

    void silenceInGameObjects();

    void updateShaders();

    void handleMenuGameState();

    void handleGamingGameState();

    void handleCountDownGameState();

    void handlePressNewLevelKeyGameState();

    void handlePressKeyByDead();

    void handlePressKeyPreviousLevel();

    void handlePressKeyGameOver();

    void handlePressKeyCredits();

    void updateEnemyTargetedCrossFire();

    void handleSplash();

    void addProjectilesToShaderLasers();

    void pressedKeyForWin();

    TextWriter *getTextWriter();

    void updateFadeToGameState();

    void handlePressKeyByWin();

    void showLevelStatistics();

    void drawMedalAlpha(int type, int x, int y, float alpha);

    [[nodiscard]] const Color &getPrimaryColor() const;

    [[nodiscard]] const Color &getSecondaryColor() const;

    [[nodiscard]] Sprite3D *getExplosionSpriteTemplate() const;

    [[nodiscard]] ShaderColor *getShaderColor() const;

    [[nodiscard]] ShaderImage *getShaderBackgroundImage() const;

    [[nodiscard]] ShaderProjectiles *getShaderLasers() const;

    [[nodiscard]] const std::vector<Weapon *> &getWeapons() const;

    [[nodiscard]] Object3D *getClosesObject3DDirection(Vertex3D from, Vertex3D direction, bool skipPlayer, bool skipCurrentSelected) const;

    [[nodiscard]] FaderToGameStates *getFadeToGameState() const;

    [[nodiscard]] LevelLoader *getLevelLoader() const;

    void loadShaders();

    VideoPlayer *getVideoPlayer();

    VideoPlayer* videoPlayer;

    void shaderBackgroundUpdate();

    ShaderExplosion *shaderExplosion;
};


#endif //BRAKEDA3D_COMPONENTGAME_H
