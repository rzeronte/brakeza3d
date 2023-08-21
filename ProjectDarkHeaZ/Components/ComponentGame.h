
#ifndef BRAKEDA3D_COMPONENTGAME_H
#define BRAKEDA3D_COMPONENTGAME_H


#include "../../include/Components/Component.h"
#include "../../include/Misc/cJSON.h"
#include "../../include/Physics/Mesh3DGhost.h"
#include "../../include/Misc/Octree.h"
#include "../../include/Misc/Grid3D.h"
#include "../../include/Misc/PathFinder.h"
#include "../../include/Physics/Mesh3DBody.h"
#include "../../include/Particles/ParticleEmitterFireworks.h"
#include "../../include/Physics/RayCollisionable.h"
#include "../../include/2D/TextWriter.h"
#include "../../include/Objects/Swarm.h"
#include "../FaderToGameStates.h"
#include "../Shaders/ShaderProjectiles.h"
#include "../Player.h"
#include "../Items/EnemyDialog.h"
#include "../../include/Shaders/ShaderColor.h"
#include "../../include/Shaders/ShaderImage.h"
#include "../LevelLoader.h"
#include "../StoreManager.h"
#include "../../include/Shaders/ShaderImageMask.h"
#include "../Shaders/ShaderCRT.h"
#include "../../include/Shaders/ShaderEdgeObject.h"
#include "../PaletteColors.h"
#include "../Items/SalvageSpaceship.h"
#include "../../include/2D/Sprite2D.h"

#define Z_COORDINATE_GAMEPLAY 10000
#define FREE_LOOK_ENABLED false
#define SPLASH_TIME 3.0f
#define FADE_SPEED_FADEOUT_TIME 0.04
#define FADE_SPEED_MENU_FIRST_TIME 0.003
#define COUNTDOWN_TO_START 3

class ComponentGame : public Component {
private:

    Vertex3D playerStartPosition;
    Vertex3D cameraCountDownPosition;
    Vertex3D cameraInGamePosition;

    TextWriter *textWriter;

    FaderToGameStates *fadeToGameState;
    Player *player;
    ShaderProjectiles *shaderProjectiles;

    TextureAnimated *radioWave;

    Sprite2D *fadeInSpriteRed;
    Sprite2D *fadeInSpriteGreen;
    Sprite2D *fadeInSpriteBlue;

    Sprite2D *spriteSparklesRed;
    Sprite2D *spriteSparklesGreen;
    Sprite2D *spriteSparklesBlue;

    std::vector<Sprite2D *> explosionSprites;

    TextureAnimated * test;

    Mesh3D *itemBoxFrame;

    Image *imageCredits;
    Image *imageHelp;
    Image *imageSplash;
    Image *imageCrossFire;
    Image *imageCablesVertical;
    Image *imageCablesHorizontal;
    Image *imageCablesStore;
    Image *imageStatistics;
    Image *imageDead;
    Image *imageEndGame;

    Counter splashCounter;

    LevelLoader *levelLoader;

    ShaderImage *shaderBackgroundImage;
    ShaderImage *shaderForegroundImage;

    ShaderColor *shaderColor;
    ShaderShockWave *shaderShockWave;

    Vertex3D spaceCrossFirePosition;
    Point2D imageCrossFireScreenPosition;

    std::vector<Weapon *> weapons;

    Swarm *swarm;

    TTF_Font *fontGame;

    StoreManager *storeManager;

    std::vector <Mesh3D*> spaceships;
    std::vector <Image*> spaceshipsInformation;
    unsigned int spaceshipSelectedIndex;


    EnemyDialog *currentEnemyDialog;

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

    void makeFadeToGameState(EngineSetup::GameState value, bool blockInput) const;

    void setVisibleInGameObjects(bool value);

    void setEnemyWeaponsEnabled(bool value);

    void removeInGameObjects();

    void loadWeapons();

    void pressedKeyForNewGameOrResumeGame();

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

    static void handlePressKeyCredits();

    void updateEnemyTargetedCrossFire();

    void handleSplash();

    void addProjectilesToShaderLasers();

    void pressedKeyForWin();

    TextWriter *getTextWriter();

    void updateFadeToGameState();

    void handlePressKeyByWin();

    void showLevelStatistics(float alpha);

    void drawMedalAlpha(int type, int x, int y, float alpha);

    [[nodiscard]] ShaderColor *getShaderColor() const;

    [[nodiscard]] ShaderImage *getShaderBackgroundImage() const;

    [[nodiscard]] ShaderProjectiles *getShaderLasers() const;

    [[nodiscard]] const std::vector<Weapon *> &getWeapons() const;

    [[nodiscard]] Object3D *getClosesObject3DDirection(Vertex3D from, Vertex3D direction, bool skipPlayer, bool skipCurrentSelected) const;

    [[nodiscard]] FaderToGameStates *getFadeToGameState() const;

    [[nodiscard]] LevelLoader *getLevelLoader() const;

    void loadShaders();

    void shaderBackgroundUpdate();

    void loadGameFonts();

    [[nodiscard]] TTF_Font *getFontGame() const;

    [[nodiscard]] TextureAnimated *getRadioWave() const;

    ShaderExplosion *shaderExplosion;

    void handleTutorialImages(float alpha);

    ShaderImageMask *dialogBackground;
    ShaderCRT *shaderCRT;
    Image *boxTutorial;
    Image *help;

    Image *boxStore;

    void handlePressKeyHelp();

    TTF_Font *fontGameAlternative;

    static void setEnemiesBehaviors(bool value);

    void setHelp(Image *help);

    [[nodiscard]] StoreManager *getStoreManager() const;

    EngineSetup::GameState gameState;

    [[nodiscard]] PlayerReflection *getClosestReflection(Vertex3D from) const;

    [[nodiscard]] Mesh3D *getItemBoxFrame() const;

    void handleEnableRadioMessage();

    void loadSpaceship(const std::string& fileNameModel, const std::string& fileNameInformation);

    void increaseSpaceshipSelected();

    void decreaseSpaceshipSelected();

    void selectSpaceshipAndStartGame();

    void loadSelectedSpaceshipModel();

    void setCurrentEnemyDialog(EnemyDialog *currentEnemyDialog);

    void handleSpaceShipSelector();

    [[nodiscard]] ShaderImage *getShaderForegroundImage() const;

    [[nodiscard]] Sprite2D *getExplosionSprite() const;

    [[nodiscard]] Sprite2D *getFadeInSpriteRed();

    [[nodiscard]] Sprite2D *getFadeInSpriteGreen();

    [[nodiscard]] Sprite2D *getFadeInSpriteBlue();

    Sprite2D *getSpriteSparklesRed() const;

    Sprite2D *getSpriteSparklesGreen() const;

    Sprite2D *getSpriteSparklesBlue() const;

    Swarm *getSwarm() const;

    ShaderEdgeObject *shaderEdgeObject;
};


#endif //BRAKEDA3D_COMPONENTGAME_H
