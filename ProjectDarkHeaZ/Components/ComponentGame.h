
#ifndef BRAKEDA3D_COMPONENTGAME_H
#define BRAKEDA3D_COMPONENTGAME_H


#include "../../include/Components/Component.h"
#include "../../include/Misc/cJSON.h"
#include "../../include/Physics/Mesh3DGhost.h"
#include "../../include/Misc/Octree.h"
#include "../../include/Misc/Grid3D.h"
#include "../../include/Misc/PathFinder.h"
#include "../../include/Physics/Mesh3DBody.h"
#include "../../include/Objects/ParticleEmitterFireworks.h"
#include "../../include/Physics/RayCollisionable.h"
#include "../../include/2D/TextWriter.h"
#include "../../include/Objects/Swarm.h"
#include "../FaderToGameStates.h"
#include "../FX/FXLaser.h"
#include "../Player.h"
#include "../Items/EnemyDialog.h"
#include "../../include/FXEffect/FXColorTint.h"
#include "../FX/FXOffsetImage.h"
#include "../LevelLoader.h"
#include "../StoreManager.h"
#include "../../include/FXEffect/FXOutliner.h"
#include "../Common/PaletteColors.h"
#include "../Items/SalvageSpaceship.h"
#include "../../include/2D/Sprite2D.h"
#include "../OpenGLShaders/ShaderOpenGLLineLaser.h"
#include "../OpenGLShaders/ShaderOpenGLImageOffset.h"
#include "../../include/Misc/TexturePackage.h"

#define Z_COORDINATE_GAMEPLAY 20
#define FREE_LOOK_ENABLED false
#define SPLASH_TIME 3.0f
#define FADE_SPEED_FADEOUT_TIME 0.04
#define FADE_SPEED_MENU_FIRST_TIME 0.003
#define COUNTDOWN_TO_START 3

#define PROJECT_DARKHEAZ_DEMO 1

struct SpaceshipAttributes {
    float power;
    float stamina;
    float energy;
};

enum MeshTemplates {
    TEMPLATE_HEALTH = 0,
    TEMPLATE_ENERGY = 1
};

class ComponentGame : public Component {
private:
    TexturePackage images;

    Vertex3D playerStartPosition;
    Vertex3D cameraCountDownPosition;
    Vertex3D cameraInGamePosition;

    TextWriter *textWriter;

    std::vector<Mesh3D*> meshTemplates;

    FaderToGameStates *fadeToGameState;
    Player *player;
    FXLaser *shaderProjectiles;

    Sprite2D *fadeInSpriteRed;
    Sprite2D *fadeInSpriteGreen;
    Sprite2D *fadeInSpriteBlue;

    Sprite2D *spriteSparklesRed;
    Sprite2D *spriteSparklesGreen;
    Sprite2D *spriteSparklesBlue;

    Sprite2D *spriteStuck;

    std::vector<Sprite2D *> explosionSprites;

    Image imageCredits;
    Image imageSplash;
    Image imageCrossFire;
    Image imageStatistics;
    Image backgroundStoreKeyboard;
    Image backgroundStoreController;
    Image imageBlack;
    Image help;
    Image backgroundSpaceshipSelection;
    Image border;
    Image pressAContinue;
    Image pressAReturn;
    Image pressEnterContinue;
    Image pressESCReturn;

    std::vector<Image*> helps;
    int currentHelpIndex;

    Counter splashCounter;
    Counter introCounter;
    Counter endgameCounter;

    LevelLoader *levelLoader;

    FXOffsetImage *shaderBackgroundImage;
    FXOffsetImage *shaderForegroundImage;

    FXColorTint *shaderColor;

    Vertex3D spaceCrossFirePosition;
    Point2D imageCrossFireScreenPosition;

    StoreManager *storeManager;

    std::vector <Mesh3D*> spaceships;
    std::vector <SpaceshipAttributes> spaceshipsAttributes;
    std::vector <Image*> spaceshipsInformation;
    std::vector <Image*> difficultyInformation;

    EnemyDialog *currentEnemyDialog;

    ShaderOpenGLLineLaser *shaderOGLLineLaser;
    ShaderOpenGLImageOffset *shaderOGLImageOffset;

    std::vector<Image*> imagesIntro;
    int currentIndexIntro;

    std::vector <Image*> imagesDead;
    std::vector <Image*> imagesEndGame;
    int currentIndexEndGameImage;

    int currentIndexDeadImage;

    //PathFinder *pathFinder;
    std::string version;
public:
    ComponentGame();

    ~ComponentGame() override;

    void onStart() override;

    void preUpdate() override;

    void onUpdate() override;

    void postUpdate() override;

    void onEnd() override;

    void onSDLPollEvent(SDL_Event *event, bool &finish) override;

    [[nodiscard]] Player *getPlayer() const;

    void initPlayer();

    void blockPlayerPositionInCamera();

    void zoomCameraCountDown();

    void reloadLevel(int level) const;

    void setGameState(EngineSetup::GameState state);

    EngineSetup::GameState getGameState() const;

    void selectClosestObject3DFromPlayer();

    Object3D *getClosesObject3DFromPosition(Vertex3D to, bool skipPlayer, bool skipCurrentSelected);

    void LoadLevels();

    void checkForEndLevel();

    static void removeProjectiles() ;

    void makeFadeToGameState(EngineSetup::GameState value, bool blockInput) const;

    void setVisibleInGameObjects(bool value);

    static void setEnemyWeaponsEnabled(bool value);

    static void removeInGameObjects() ;

    void pressedKeyForBeginLevel();

    void pressedKeyByDead();

    static int getLiveEnemiesCounter();

    static void silenceInGameObjects();

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

    void addRayLightsToShaderLaserLine();

    void pressedKeyForWin() const;

    TextWriter *getTextWriter();

    void updateFadeToGameState();

    void handlePressKeyByWin() const;

    void showLevelStatistics(float alpha);

    static void drawMedalAlpha(int type, int x, int y, float alpha);

    [[nodiscard]] FXColorTint *getShaderColor() const;

    [[nodiscard]] FXOffsetImage *getShaderBackgroundImage() const;

    [[nodiscard]] FXLaser *getShaderLasers() const;

    [[nodiscard]] Object3D *getClosesObject3DDirection(Vertex3D from, Vertex3D direction, bool skipPlayer, bool skipCurrentSelected) const;

    [[nodiscard]] FaderToGameStates *getFadeToGameState() const;

    [[nodiscard]] LevelLoader *getLevelLoader() const;

    [[nodiscard]] StoreManager *getStoreManager() const;

    EngineSetup::GameState gameState;

    [[nodiscard]] static PlayerReflection *getClosestReflection(Vertex3D from) ;

    [[nodiscard]] FXOffsetImage *getShaderForegroundImage() const;

    [[nodiscard]] Sprite2D *getRandomExplosionSprite() const;

    [[nodiscard]] Sprite2D *getFadeInSpriteRed();

    [[nodiscard]] Sprite2D *getFadeInSpriteGreen();

    [[nodiscard]] Sprite2D *getFadeInSpriteBlue();

    [[nodiscard]] Sprite2D *getSpriteSparklesRed() const;

    [[nodiscard]] Sprite2D *getSpriteSparklesGreen() const;

    [[nodiscard]] Sprite2D *getSpriteSparklesBlue() const;

    [[nodiscard]] Sprite2D *getSpriteStuck() const;

    void LoadFXAndShaders();

    void shaderBackgroundUpdate();

    void handleOnUpdateTutorialImages(float alpha);

    static void handlePressKeyHelp();

    static void setEnemiesBehaviors(bool value);

    void setHelp(Image *help);

    void loadSpaceship(const std::string& fileNameModel, const std::string& fileNameInformation, SpaceshipAttributes attr);

    void increaseSpaceshipSelected();

    void decreaseSpaceshipSelected();

    void selectSpaceshipAndStartGame();

    void loadSelectedSpaceshipModel();

    void setCurrentEnemyDialog(EnemyDialog *currentEnemyDialog);

    void handleSpaceShipSelector();

    void handleOnUpdateMessageRadio();

    void increaseHelpImage();

    void decreaseHelpImage();

    void writeDialogTextToContinue(const char *string);

    void handleOnUpdateCountDown();

    void handleOnUpdateGamingTutorial(float alpha);

    void handleOnUpdateHelp(float alpha);

    void handleOnUpdateStore(float alpha);

    void handleOnUpdateSpaceshipSelector(float alpha);

    void handleOnUpdatePressKeyGameOver(float alpha);

    void handleOnUpdateCredits(float alpha);

    void handleOnUpdatePressKeyByDead(float alpha);

    void handleOnUpdateSplash(float alpha);

    ShaderOpenGLLineLaser *getShaderOGLLineLaser();

    [[nodiscard]] ShaderOpenGLImageOffset *getShaderOGLImageOffset() const;

    TexturePackage *getImages();

    Image boxTutorial;
    FXOutliner *shaderEdgeObject;

    void handleOnUpdateIntro(float alpha);

    void decreaseIntroImage();

    void increaseIntroImage();

    void handleDifficultySelector();

    void handleOnUpdateDifficultySelector(const float alpha);

    void launchSpaceshipSelector() const;

    unsigned int spaceshipSelectedIndex;

    void resetGame();

    static EnemyGhost *getEnemyByName(const std::string& name);

    void increaseEndGameImage();

    Image* getPressEnterContinue();

    Image* getPressAContinue();

    void LoadMeshTemplates();

    [[nodiscard]] const std::vector<Mesh3D *> &getMeshTemplates() const;

    [[nodiscard]] const std::string &getVersion() const;
};


#endif //BRAKEDA3D_COMPONENTGAME_H
