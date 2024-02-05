#include "ComponentGame.h"
#include "../../include/Components/ComponentCollisions.h"
#include "../../include/Brakeza3D.h"
#include "../Items/ItemBombGhost.h"
#include "../Bosses/BossEnemy.h"

ComponentGame::ComponentGame()
:
    currentIndexDeadImage(0),
    currentIndexIntro(0),
    cameraCountDownPosition(Vertex3D(0, 0, 5)),
    cameraInGamePosition(Vertex3D(0, 0, 0)),
    playerStartPosition(Vertex3D(0, 4, Z_COORDINATE_GAMEPLAY)),
    textWriter(nullptr),
    fadeToGameState(nullptr),
    player(nullptr),
    shaderProjectiles(nullptr),
    backgroundSpaceshipSelection(Image(SETUP->IMAGES_FOLDER + "backgroundSpaceshipSelection.png")),
    boxStore(Image(SETUP->IMAGES_FOLDER + "store.png")),
    imageBlack(Image(SETUP->IMAGES_FOLDER + "black.png")),
    boxTutorial(Image(SETUP->IMAGES_FOLDER + "tutorial_box.png")),
    imageEndGame(Image(SETUP->IMAGES_FOLDER + "end_game.png")),
    imageStatistics(Image(SETUP->IMAGES_FOLDER + "statistics_screen.png")),
    imageCredits(Image(SETUP->IMAGES_FOLDER + "credits.png")),
    imageSplash(Image(SETUP->IMAGES_FOLDER + SETUP->LOGO_BRAKEZA)),
    imageCrossFire(Image(SETUP->IMAGES_FOLDER + "crossfire.png")),
    border(Image(SETUP->IMAGES_FOLDER + "hud_background.png")),
    currentHelpIndex(0),
    levelLoader(new LevelLoader()),
    shaderBackgroundImage(nullptr),
    shaderForegroundImage(nullptr),
    shaderColor(nullptr),
    spaceshipSelectedIndex(0),
    currentEnemyDialog(nullptr),
    gameState(EngineSetup::GameState::NONE)
{
}

void ComponentGame::onStart()
{
    Logging::Message("ComponentGame onStart");

    images.addItem(EngineSetup::get()->IMAGES_FOLDER + "particle.png", "particle01");
    images.addItem(EngineSetup::get()->IMAGES_FOLDER + "particle02.png", "particle02");

    player = new Player();

    setGameState(EngineSetup::GameState::NONE);

    auto window = ComponentsManager::get()->getComponentWindow();
    textWriter = new TextWriter(window->getRenderer(), window->getFontDefault());

    fadeToGameState = new FaderToGameStates(
        Color::black(),
        0.01f,
        EngineSetup::GameState::SPLASH,
        false
    );

    imagesIntro.push_back(new Image(SETUP->IMAGES_FOLDER + "intro/01.png"));
    imagesIntro.push_back(new Image(SETUP->IMAGES_FOLDER + "intro/02.png"));
    imagesIntro.push_back(new Image(SETUP->IMAGES_FOLDER + "intro/03.png"));
    imagesIntro.push_back(new Image(SETUP->IMAGES_FOLDER + "intro/04.png"));
    imagesIntro.push_back(new Image(SETUP->IMAGES_FOLDER + "intro/05.png"));
    imagesIntro.push_back(new Image(SETUP->IMAGES_FOLDER + "intro/06.png"));
    imagesIntro.push_back(new Image(SETUP->IMAGES_FOLDER + "intro/07.png"));
    imagesIntro.push_back(new Image(SETUP->IMAGES_FOLDER + "intro/08.png"));
    imagesIntro.push_back(new Image(SETUP->IMAGES_FOLDER + "intro/09.png"));
    imagesIntro.push_back(new Image(SETUP->IMAGES_FOLDER + "intro/10.png"));
    imagesIntro.push_back(new Image(SETUP->IMAGES_FOLDER + "intro/11.png"));
    imagesIntro.push_back(new Image(SETUP->IMAGES_FOLDER + "intro/12.png"));
    imagesIntro.push_back(new Image(SETUP->IMAGES_FOLDER + "intro/13.png"));
    imagesIntro.push_back(new Image(SETUP->IMAGES_FOLDER + "intro/14.png"));
    imagesIntro.push_back(new Image(SETUP->IMAGES_FOLDER + "intro/15.png"));

    imagesDead.push_back(new Image(SETUP->IMAGES_FOLDER + "gameover/01.png"));
    imagesDead.push_back(new Image(SETUP->IMAGES_FOLDER + "gameover/02.png"));
    imagesDead.push_back(new Image(SETUP->IMAGES_FOLDER + "gameover/03.png"));
    imagesDead.push_back(new Image(SETUP->IMAGES_FOLDER + "gameover/04.png"));
    imagesDead.push_back(new Image(SETUP->IMAGES_FOLDER + "gameover/05.png"));
    imagesDead.push_back(new Image(SETUP->IMAGES_FOLDER + "gameover/06.png"));

    helps.push_back(new Image(SETUP->IMAGES_FOLDER + SETUP->DEFAULT_HELP_IMAGE));
    helps.push_back(new Image(SETUP->IMAGES_FOLDER + "keyboard.png"));

    splashCounter.setStep(SPLASH_TIME);
    introCounter.setStep(5.0f);
    introCounter.setEnabled(false);

    ComponentsManager::get()->getComponentCollisions()->initBulletSystem();

    auto camera = ComponentsManager::get()->getComponentCamera();
    camera->getCamera()->setPosition(cameraInGamePosition);

    camera->setAutoScroll(false);
    camera->setAutoScrollSpeed(Vertex3D(0, -0.0, 0));
    camera->setFreeLook(FREE_LOOK_ENABLED);

    ComponentsManager::get()->getComponentInput()->setEnabled(FREE_LOOK_ENABLED);
    ComponentsManager::get()->getComponentMenu()->setEnabled(false);

    loadSpaceship("spaceships/player.fbx", "spaceships/spaceship_01.png", {0.3, 100, 100});
    loadSpaceship("spaceships/player02.fbx", "spaceships/spaceship_02.png", {0.5, 130, 70});
    loadSpaceship("spaceships/player03.fbx", "spaceships/spaceship_03.png", {0.2, 130, 130});

    initPlayer();
    LoadLevels();
    LoadFXAndShaders();

    getLevelLoader()->LoadJSONWeapons();
    getLevelLoader()->LoadConfig();

    if (getLevelLoader()->getIndexSpaceshipSelected() >= 0) {
        spaceshipSelectedIndex = getLevelLoader()->getIndexSpaceshipSelected();
        loadSelectedSpaceshipModel();
    }

    ComponentSound::fadeInMusic(
        ComponentsManager::get()->getComponentSound()->getSoundPackage().getMusicByLabel("musicMainMenu"),
        -1,
        SPLASH_TIME * 1000
    );

    storeManager = new StoreManager(player, textWriter);

    difficultyInformation.push_back(new Image(SETUP->IMAGES_FOLDER + "spaceships/difficulty_01.png"));
    difficultyInformation.push_back(new Image(SETUP->IMAGES_FOLDER + "spaceships/difficulty_02.png"));
    difficultyInformation.push_back(new Image(SETUP->IMAGES_FOLDER + "spaceships/difficulty_03.png"));
    difficultyInformation.push_back(new Image(SETUP->IMAGES_FOLDER + "spaceships/difficulty_04.png"));

    fadeInSpriteRed = new Sprite2D(0, 0, false, new TextureAnimated(std::string(EngineSetup::get()->SPRITES_FOLDER + "flash01.png"), 96, 96, 37, 45));
    fadeInSpriteGreen = new Sprite2D(0, 0, false, new TextureAnimated(std::string(EngineSetup::get()->SPRITES_FOLDER + "flash02.png"), 96, 96, 37, 45));
    fadeInSpriteBlue = new Sprite2D(0, 0, false, new TextureAnimated(std::string(EngineSetup::get()->SPRITES_FOLDER + "flash03.png"), 96, 96, 37, 45));

    spriteSparklesRed = new Sprite2D(0, 0, false, new TextureAnimated(std::string(EngineSetup::get()->SPRITES_FOLDER + "explosions/05.png"), 64, 64, 16, 40));
    spriteSparklesGreen = new Sprite2D(0, 0, false, new TextureAnimated(std::string(EngineSetup::get()->SPRITES_FOLDER + "explosions/02.png"), 64, 64, 16, 40));
    spriteSparklesBlue = new Sprite2D(0, 0, false, new TextureAnimated(std::string(EngineSetup::get()->SPRITES_FOLDER + "explosions/05.png"), 64, 64, 16, 40));

    spriteStuck = new Sprite2D(0, 0, false, new TextureAnimated(std::string(EngineSetup::get()->SPRITES_FOLDER + "Smoke45Frames.png"), 128, 128, 45, 24));

    explosionSprites.push_back(new Sprite2D(0, 0, false, new TextureAnimated(std::string(EngineSetup::get()->SPRITES_FOLDER + "explosion_d.png"), 160, 120, 90, 60)));
}

void ComponentGame::LoadFXAndShaders()
{
    shaderBackgroundImage = new FXOffsetImage(EngineSetup::get()->IMAGES_FOLDER + "empty.png");
    shaderForegroundImage = new FXOffsetImage(EngineSetup::get()->IMAGES_FOLDER + "empty.png");
    shaderColor = new FXColorTint(false, PaletteColors::getStamina(), 0.75f);
    shaderProjectiles = new FXLaser(new Image(EngineSetup::get()->IMAGES_FOLDER + "noiseCloud.png"));
    shaderEdgeObject = new FXOutliner(false, nullptr, PaletteColors::getEnergy(), 1.0f);

    shaderOGLLineLaser = new ShaderOpenGLLineLaser();
    shaderOGLImageOffset = new ShaderOpenGLImageOffset();
}

ComponentGame::~ComponentGame()
{
    delete player;

    for (auto sprite: explosionSprites) {
        delete sprite;
    }

    delete fadeToGameState;
    delete levelLoader;

    delete shaderProjectiles;
    delete shaderBackgroundImage;
    delete shaderForegroundImage;
    delete shaderColor;

    delete storeManager;

}

void ComponentGame::preUpdate()
{
    updateEnemyTargetedCrossFire();
    shaderBackgroundUpdate();
}

void ComponentGame::onUpdate()
{
    updateFadeToGameState();
    updateShaders();

    const float alpha = 1 - getFadeToGameState()->getProgress();
    textWriter->setAlpha(alpha);

    switch(gameState) {
        case EngineSetup::INTRO: {
            handleOnUpdateIntro(alpha);
            break;
        }
        case EngineSetup::COUNTDOWN: {
            handleOnUpdateCountDown();
            break;
        }
        case EngineSetup::PRESS_KEY_NEW_LEVEL:
        case EngineSetup::PRESS_KEY_PREVIOUS_LEVEL: {
            handleOnUpdateTutorialImages(alpha);
            break;
        }
        case EngineSetup::GAMING_TUTORIAL: {
            handleOnUpdateGamingTutorial(alpha);
            break;
        }
        case EngineSetup::SPLASH: {
            handleOnUpdateSplash(alpha);
            break;
        }
        case EngineSetup::GAMING: {
            blockPlayerPositionInCamera();
            checkForEndLevel();
            handleOnUpdateMessageRadio();
            break;
        }
        case EngineSetup::PRESS_KEY_BY_WIN: {
            showLevelStatistics(alpha);
            break;
        }
        case EngineSetup::HELP: {
            handleOnUpdateHelp(alpha);
            break;
        }
        case EngineSetup::STORE: {
            handleOnUpdateStore(alpha);
            break;
        }
        case EngineSetup::CREDITS: {
            handleOnUpdateCredits(alpha);
            break;
        }
        case EngineSetup::PRESS_KEY_BY_DEAD: {
            handleOnUpdatePressKeyByDead(alpha);
            break;
        }
        case EngineSetup::SPACESHIP_SELECTOR: {
            handleOnUpdateSpaceshipSelector(alpha);
            break;
        }
        case EngineSetup::PRESS_KEY_GAMEOVER: {
            handleOnUpdatePressKeyGameOver(alpha);
            break;
        }
        case EngineSetup::DIFFICULT_SELECTOR: {
            handleOnUpdateDifficultySelector(alpha);
            break;
        }
    }
}

void ComponentGame::postUpdate()
{
    addRayLightsToShaderLaserLine();

    if (getPlayer()->isAutoTargetOnNextFrame()) {
        ComponentsManager::get()->getComponentGame()->selectClosestObject3DFromPlayer();
        getPlayer()->setAutoTargetOnNextFrame(false);
    }
}

void ComponentGame::handleOnUpdateTutorialImages(float alpha)
{
    if (!getLevelLoader()->getTutorials().empty()) {
        auto window = ComponentsManager::get()->getComponentWindow();
        float oldAlpha = textWriter->getAlpha();
        textWriter->setAlpha(alpha);
        textWriter->setFont(window->getFontAlternative());
        std::string message = std::to_string(getLevelLoader()->getCurrentTutorialIndex() + 1) + " of " + std::to_string((int)getLevelLoader()->getTutorials().size());

        getLevelLoader()->drawCurrentTutorialImage(alpha);
        writeDialogTextToContinue("Press ENTER to start...");
        if (getLevelLoader()->getTutorials().size() > 1) {
            textWriter->writeTTFCenterHorizontal(520, message.c_str(), PaletteColors::getMenuOptions(), 0.5f);
        }
        boxTutorial.drawFlatAlpha(0, 0, alpha, window->getForegroundFramebuffer());
        textWriter->setAlpha(oldAlpha);
    } else {
        float oldAlpha = textWriter->getAlpha();
        textWriter->setAlpha(alpha);
        writeDialogTextToContinue("Press ENTER to start...");
        textWriter->setAlpha(oldAlpha);

    }
}

void ComponentGame::handleOnUpdateGamingTutorial(float alpha)
{
    auto fb = ComponentsManager::get()->getComponentWindow()->getForegroundFramebuffer();
    boxTutorial.drawFlatAlpha(0, 0, alpha, fb);
    help.drawFlatAlpha(0, 0, alpha, fb);
    writeDialogTextToContinue("Press ENTER to continue...");

    /*keep show message radio so that it keep consuming his time*/
    handleOnUpdateMessageRadio();
}

void ComponentGame::drawMedalAlpha(int type, int x, int y, float alpha)
{
    auto hudTexturePackage = ComponentsManager::get()->getComponentHUD()->getHudTextures();
    switch(type) {
        case 0: { hudTexturePackage->getTextureByLabel("medalBronze")->drawFlatAlpha(x, y, alpha, ComponentsManager::get()->getComponentWindow()->getForegroundFramebuffer()); break; }
        case 1: { hudTexturePackage->getTextureByLabel("medalSilver")->drawFlatAlpha(x, y, alpha, ComponentsManager::get()->getComponentWindow()->getForegroundFramebuffer()); break; }
        case 2: { hudTexturePackage->getTextureByLabel("medalGold")->drawFlatAlpha(x, y, alpha, ComponentsManager::get()->getComponentWindow()->getForegroundFramebuffer()); break; }
        default: break;
    }
}

void ComponentGame::showLevelStatistics(float alpha)
{
    textWriter->setFont(ComponentsManager::get()->getComponentWindow()->getFontAlternative());
    boxTutorial.drawFlatAlpha(0, 0, alpha, ComponentsManager::get()->getComponentWindow()->getForegroundFramebuffer());

    int offsetX = 450;
    int offsetY = 100;
    const int space = 100;

    auto window = ComponentsManager::get()->getComponentWindow();
    auto stats = getLevelLoader()->getStats();
    auto c = PaletteColors::getMenuOptions();
    auto fb = window->getForegroundFramebuffer();

    imageStatistics.drawFlatAlpha(0, 0, alpha, fb);

    player->getWeaponTypeByLabel("projectile")->getIcon()->drawFlatAlpha(offsetX, offsetY + 125, alpha, fb);
    textWriter->writeTextTTFAutoSize(offsetX, offsetY + 195, stats->stats(WEAPON_PROJECTILE).c_str(), c, 0.3);
    textWriter->writeTextTTFAutoSize(offsetX, offsetY + 220, stats->accuracyPercentFormatted(WEAPON_PROJECTILE).c_str(), c, 0.3);
    drawMedalAlpha(getLevelLoader()->getStats()->medalType(WEAPON_PROJECTILE), offsetX, offsetY + 250, alpha);

    offsetX += space;
    player->getWeaponTypeByLabel("laser")->getIcon()->drawFlatAlpha(offsetX, offsetY + 125, alpha, fb);
    textWriter->writeTextTTFAutoSize(offsetX, offsetY + 195, stats->stats(WEAPON_LASER).c_str(), c, 0.3);
    textWriter->writeTextTTFAutoSize(offsetX, offsetY + 220, stats->accuracyPercentFormatted(WEAPON_LASER).c_str(), c, 0.3);
    drawMedalAlpha(getLevelLoader()->getStats()->medalType(WEAPON_LASER), offsetX, offsetY + 250, alpha);

    offsetX += space;
    player->getWeaponTypeByLabel("ray")->getIcon()->drawFlatAlpha(offsetX, offsetY + 125, alpha, fb);
    textWriter->writeTextTTFAutoSize(offsetX, offsetY + 195, stats->stats(WEAPON_RAYLIGHT).c_str(), c, 0.3);
    textWriter->writeTextTTFAutoSize(offsetX, offsetY + 220, stats->accuracyPercentFormatted(WEAPON_RAYLIGHT).c_str(), c, 0.3);
    drawMedalAlpha(getLevelLoader()->getStats()->medalType(WEAPON_RAYLIGHT), offsetX, offsetY + 250, alpha);

    offsetX += space;
    player->getWeaponTypeByLabel("bomb")->getIcon()->drawFlatAlpha(offsetX, offsetY + 125, alpha, fb);
    textWriter->writeTextTTFAutoSize(offsetX, offsetY + 195, stats->stats(WEAPON_BOMB).c_str(), c, 0.3);
    textWriter->writeTextTTFAutoSize(offsetX, offsetY + 220, stats->accuracyPercentFormatted(WEAPON_BOMB).c_str(), c, 0.3);
    drawMedalAlpha(getLevelLoader()->getStats()->medalType(WEAPON_BOMB), offsetX, offsetY + 250, alpha);

    textWriter->setFont(window->getFontAlternative());

    writeDialogTextToContinue("Press ENTER to continue...");

    ComponentsManager::get()->getComponentHUD()->getHudTextures()->getTextureByLabel("coinIcon")->drawFlatAlpha(
        EngineSetup::get()->screenWidth/2-20 ,
        offsetY + 295,
        alpha,
        fb
    );

    textWriter->writeTTFCenterHorizontal(
        offsetY + 347,
        std::to_string(getLevelLoader()->getStats()->coinsGained).c_str(),
        Color::green(),
        0.75f
    );
}

void ComponentGame::updateFadeToGameState()
{
    fadeToGameState->onUpdate();
    if (fadeToGameState->isEndFadeOut()) {
        fadeToGameState->setEndFadeOut(false);
        setGameState(fadeToGameState->getGameStateWhenEnds());
    }

    if (fadeToGameState->isFinished()) {
        ComponentsManager::get()->getComponentGameInput()->setEnabled(true);
    }
}

void ComponentGame::updateEnemyTargetedCrossFire()
{
    if (gameState != EngineSetup::GAMING) return;

    auto selected = ComponentsManager::get()->getComponentRender()->getSelectedObject();

    if (selected == nullptr) return;

    const auto direction = selected->getPosition() - spaceCrossFirePosition;

    const float speed = 0.1f;
    spaceCrossFirePosition = spaceCrossFirePosition + direction.getScaled(speed);

    imageCrossFireScreenPosition = Transforms::WorldToPoint(spaceCrossFirePosition);

    imageCrossFireScreenPosition.x -= imageCrossFire.width() / 2;
    imageCrossFireScreenPosition.y -= imageCrossFire.height() / 2;

    imageCrossFire.drawFlatAlpha(
        imageCrossFireScreenPosition.x,
        imageCrossFireScreenPosition.y,
        1.0,
        ComponentsManager::get()->getComponentWindow()->getForegroundFramebuffer()
    );
}

int ComponentGame::getLiveEnemiesCounter()
{
    int cont = 0 ;
    for (auto object : Brakeza3D::get()->getSceneObjects()) {
        auto *enemy = dynamic_cast<EnemyGhost *> (object);

        if (enemy != nullptr) {
            cont++;
        }
    }

    return cont;
}

void ComponentGame::checkForEndLevel()
{
    if (getLiveEnemiesCounter() == 0 && !getLevelLoader()->isLevelFinished()) {

        auto waiting = getLevelLoader()->getWaitingToWin();
        waiting->update();
        if (!waiting->isEnabled()) {
            waiting->setEnabled(true);
        }

        if (waiting->isFinished()) {
            getLevelLoader()->setLevelFinished(true);
            getLevelLoader()->setLevelStartedToPlay(false);
            removeProjectiles();
            getFadeToGameState()->setSpeed(FADE_SPEED_FADEOUT_TIME);
            ComponentsManager::get()->getComponentSound()->sound("levelCompleted", EngineSetup::SoundChannels::SND_GLOBAL, 0);

            if (getLevelLoader()->isEndLevel()) {
                makeFadeToGameState(EngineSetup::PRESS_KEY_GAMEOVER, true);
                return;
            }

            setGameState(EngineSetup::PRESS_KEY_BY_WIN);
        }
    }
}

void ComponentGame::setGameState(EngineSetup::GameState state)
{
    switch(state) {
        case EngineSetup::NONE:
            break;
        case EngineSetup::SPLASH:
            handleSplash();
            break;
        case EngineSetup::MENU:
            handleMenuGameState();
            break;
        case EngineSetup::PRESS_KEY_NEW_LEVEL:
            handlePressNewLevelKeyGameState();
            break;
        case EngineSetup::PRESS_KEY_PREVIOUS_LEVEL:
            handlePressKeyPreviousLevel();
            break;
        case EngineSetup::PRESS_KEY_BY_DEAD:
            handlePressKeyByDead();
            break;
        case EngineSetup::COUNTDOWN:
            handleCountDownGameState();
            break;
        case EngineSetup::GAMING:
            handleGamingGameState();
            break;
        case EngineSetup::HELP:
            handlePressKeyHelp();
            break;
        case EngineSetup::CREDITS:
            handlePressKeyCredits();
            break;
        case EngineSetup::PRESS_KEY_GAMEOVER:
            handlePressKeyGameOver();
            break;
        case EngineSetup::PRESS_KEY_BY_WIN:
            handlePressKeyByWin();
            break;
        case EngineSetup::GAMING_TUTORIAL:
            setEnemiesBehaviors(false);
            break;
        case EngineSetup::STORE: {
            break;
        }
        case EngineSetup::SPACESHIP_SELECTOR: {
            handleSpaceShipSelector();
            break;
        }
        case EngineSetup::INTRO: {
            ComponentsManager::get()->getComponentMenu()->setEnabled(false);
            introCounter.setEnabled(true);
            break;
        }
        case EngineSetup::DIFFICULT_SELECTOR: {
            handleDifficultySelector();
            break;
        }
    }

    this->gameState = state;

    Logging::Message("GameState changed to: %d", state);
}

void ComponentGame::onEnd() {
}

void ComponentGame::onSDLPollEvent(SDL_Event *event, bool &finish) {
}

Player *ComponentGame::getPlayer() const {
    return player;
}

void ComponentGame::blockPlayerPositionInCamera()
{
    auto window = ComponentsManager::get()->getComponentWindow();

    Vertex3D destinyPoint = player->getPosition() + player->getVelocity();
    Point2D homogeneousPosition = Transforms::WorldToPoint(destinyPoint);

    float sX = (float) homogeneousPosition.x / (float) window->getWidth();
    float sY = (float) homogeneousPosition.y / (float) window->getHeight();

    if (sY >= 1) {
        if (player->getVelocity().y > 0) {
            Vertex3D newVelocity = player->getVelocity();
            newVelocity.y = 0;
            player->setVelocity(newVelocity);
        }
    }

    // top
    if (sY <= 0) {
        if (player->getVelocity().y < 0) {
            Vertex3D newVelocity = player->getVelocity();
            newVelocity.y = 0;
            player->setVelocity(newVelocity);
        }
    }

    //right
    if (sX > 1) {
        if (player->getVelocity().x > 0) {
            Vertex3D newVelocity = player->getVelocity();
            newVelocity.x = 0;
            player->setVelocity(newVelocity);
        }
    }

    //left;
    if (sX <= 0) {
        if (player->getVelocity().x < 0) {
            Vertex3D newVelocity = player->getVelocity();
            newVelocity.x = 0;
            player->setVelocity(newVelocity);
        }
    }
}

EngineSetup::GameState ComponentGame::getGameState() const {
    return gameState;
}

void ComponentGame::loadSelectedSpaceshipModel()
{
    player->clone(spaceships[spaceshipSelectedIndex]);
    player->setPower(spaceshipsAttributes[spaceshipSelectedIndex].power);

    player->setStamina(spaceshipsAttributes[spaceshipSelectedIndex].stamina);
    player->setStartStamina(spaceshipsAttributes[spaceshipSelectedIndex].stamina);

    player->setEnergy(spaceshipsAttributes[spaceshipSelectedIndex].energy);
    player->setStartEnergy(spaceshipsAttributes[spaceshipSelectedIndex].energy);
    player->setScale(1);

    Logging::Message("Player attributes: Power: %f, Stamina: %f, Energy: %f", player->power, player->getStamina(), player->getEnergy());

    player->makeSimpleGhostBody(
        Vertex3D(1, 1, 1),
        ComponentsManager::get()->getComponentCollisions()->getDynamicsWorld(),
        EngineSetup::collisionGroups::Player,
        EngineSetup::collisionGroups::AllFilter
    );
}

void ComponentGame::initPlayer()
{
    player->setRotation(0, 0, 0);
    player->setEnabled(false);
    player->setAlpha(1);
    player->setEnableLights(true);
    player->setPosition(playerStartPosition);
    player->setStamina(100);
    player->setStencilBufferEnabled(true);
    Brakeza3D::get()->addObject3D(player, "Player");

    player->onStartSetup();
}

Object3D *ComponentGame::getClosesObject3DDirection(Vertex3D from, Vertex3D direction, bool skipPlayer, bool skipCurrentSelected) const
{
    Object3D *currentClosestObject = nullptr;
    float currentMinDistance = 0;

    for (auto object : Brakeza3D::get()->getSceneObjects()) {
        if (!object->isEnabled() || player == object) {
            continue;
        }

        if (skipPlayer && player == object) {
            continue;
        }

        if (skipCurrentSelected && ComponentsManager::get()->getComponentRender()->getSelectedObject() == object) {
            continue;
        }

        auto enemy = dynamic_cast<EnemyGhost *>(object);
        if (enemy == nullptr) {
            continue;
        }

        const Vertex3D to = (enemy->getPosition() - from);

        if (direction * to > 0) {
            const float distance = to.getSquaredLength();
            if (currentClosestObject == nullptr) {
                currentMinDistance = distance;
                currentClosestObject = object;
            } else {
                if (distance < currentMinDistance) {
                    currentMinDistance = distance;
                    currentClosestObject = object;
                }
            }
        }
    }

    return currentClosestObject;
}

PlayerReflection *ComponentGame::getClosestReflection(Vertex3D from)
{
    PlayerReflection *currentClosestObject = nullptr;
    float currentMinDistance = 0;

    for (auto object : Brakeza3D::get()->getSceneObjects()) {
        auto reflection = dynamic_cast<PlayerReflection *>(object);
        if (reflection == nullptr) {
            continue;
        }

        const Vertex3D to = (reflection->getPosition() - from);

        const float distance = to.getSquaredLength();
        if (currentClosestObject == nullptr) {
            currentMinDistance = distance;
            currentClosestObject = reflection;
        } else {
            if (distance < currentMinDistance) {
                currentMinDistance = distance;
                currentClosestObject = reflection;
            }
        }
    }

    return currentClosestObject;
}

Object3D *ComponentGame::getClosesObject3DFromPosition(Vertex3D to, bool skipPlayer, bool skipCurrentSelected)
{
    Object3D *currentClosestObject = nullptr;
    float currentMinDistance = 0;

    for (auto object : Brakeza3D::get()->getSceneObjects()) {
        if (!object->isEnabled() || player == object) {
            continue;
        }

        if (skipPlayer && player == object) {
            continue;
        }

        if (skipCurrentSelected && ComponentsManager::get()->getComponentRender()->getSelectedObject() == object) {
            continue;
        }

        auto mesh = dynamic_cast<EnemyGhost *>(object);
        if (mesh == nullptr) {
            continue;
        }

        if (mesh->isRemoved()) continue;

        mesh->updateBoundingBox();
        for (auto & vertice : mesh->getAabb().vertices) {
            Vector3D v(to, vertice);

            const float distance = v.getComponent().getSquaredLength();
            if (currentClosestObject == nullptr) {
                currentMinDistance = distance;
                currentClosestObject = object;
            } else {
                if (distance < currentMinDistance) {
                    currentMinDistance = distance;
                    currentClosestObject = object;
                }
            }
        }
    }

    return currentClosestObject;
}

void ComponentGame::selectClosestObject3DFromPlayer()
{
    auto currentClosestObject = getClosesObject3DFromPosition(player->getPosition(), true, true);

    if (currentClosestObject != nullptr) {
        ComponentsManager::get()->getComponentSound()->sound("tic", EngineSetup::SoundChannels::SND_GLOBAL, 0);
        ComponentsManager::get()->getComponentRender()->setSelectedObject(currentClosestObject);
    }
}

void ComponentGame::LoadLevels()
{
    auto basePath = EngineSetup::get()->CONFIG_FOLDER + "Levels/";

    for (int i = 1; i <= 30; i++) {
        levelLoader->addLevel(basePath + "level" + (i < 10 ? "0" : "") + std::to_string(i) + ".json");
    }
}

FaderToGameStates *ComponentGame::getFadeToGameState() const
{
    return fadeToGameState;
}

LevelLoader *ComponentGame::getLevelLoader() const
{
    return levelLoader;
}

void ComponentGame::removeProjectiles()
{
    auto objects = Brakeza3D::get()->getSceneObjects();
    for (auto object : objects) {
        auto projectile = dynamic_cast<AmmoProjectileBody *> (object);
        if (projectile != nullptr) {
            projectile->setRemoved(true);
        }

        auto projectileEmitter = dynamic_cast<AmmoProjectileBodyEmitter *> (object);
        if (projectileEmitter != nullptr) {
            projectileEmitter->setRemoved(true);
        }

        auto bomb = dynamic_cast<ItemBombGhost *> (object);
        if (bomb != nullptr) {
            bomb->remove();
        }
    }
}

void ComponentGame::makeFadeToGameState(EngineSetup::GameState value, bool blockInput) const
{
    ComponentsManager::get()->getComponentGameInput()->setEnabled(!blockInput);
    getFadeToGameState()->resetTo(value);
}

void ComponentGame::removeInGameObjects()
{
    for (auto object : Brakeza3D::get()->getSceneObjects()) {
        auto *enemy = dynamic_cast<EnemyGhost *> (object);
        auto *health = dynamic_cast<ItemHealthGhost *> (object);
        auto *energy = dynamic_cast<ItemEnergyGhost *> (object);
        auto *weapon = dynamic_cast<ItemWeaponGhost *> (object);
        auto *human = dynamic_cast<ItemHumanGhost *> (object);
        auto *salvage = dynamic_cast<SalvageSpaceship *> (object);
        auto *projectile = dynamic_cast<Projectile3DBody *> (object);
        auto *projectileEmitter = dynamic_cast<AmmoProjectileBodyEmitter *> (object);
        auto *particleEmitter = dynamic_cast<ParticleEmitter *> (object);
        auto bomb = dynamic_cast<ItemBombGhost *> (object);
        auto projectileRay = dynamic_cast<ProjectileRay *> (object);
        auto rayLight = dynamic_cast<RayLight *> (object);

        if (enemy != nullptr) {
            enemy->remove();
            continue;
        }

        if (
            energy != nullptr ||
            health != nullptr ||
            human != nullptr ||
            weapon != nullptr ||
            projectile != nullptr ||
            projectileRay != nullptr ||
            projectileEmitter != nullptr ||
            particleEmitter != nullptr ||
            bomb != nullptr ||
            salvage != nullptr ||
            rayLight != nullptr
        ) {
            object->setRemoved(true);
            continue;
        }
    }

    LevelLoader::removeBackgroundObjects();
}

void ComponentGame::silenceInGameObjects()
{
    for (auto object : Brakeza3D::get()->getSceneObjects()) {
        auto *enemy = dynamic_cast<EnemyGhost *> (object);

        if (enemy != nullptr && enemy->getWeapon() != nullptr) {
            enemy->getWeapon()->stopSoundChannel();
        }
    }
}

void ComponentGame::setEnemyWeaponsEnabled(bool value)
{
    for (auto &object : Brakeza3D::get()->getSceneObjects()) {
        auto enemy = dynamic_cast<EnemyGhost *> (object);
        auto boss = dynamic_cast<BossEnemy *> (object);

        auto projectileRay = dynamic_cast<ProjectileRay *> (object);

        if (projectileRay != nullptr) {
            projectileRay->setEnabled(true);
        }

        if (boss != nullptr) continue;

        if (enemy != nullptr) {
            if (enemy->getWeapon() != nullptr) {
                enemy->getWeapon()->getCounterCadence()->setEnabled(value);
            }
            enemy->setEmittersEnabled(value);
        }
    }
}

void ComponentGame::setVisibleInGameObjects(bool value)
{
    for (auto object: Brakeza3D::get()->getSceneObjects()) {
        auto *enemy = dynamic_cast<EnemyGhost *> (object);
        auto *health = dynamic_cast<ItemHealthGhost *> (object);
        auto *salvage = dynamic_cast<SalvageSpaceship *> (object);
        auto *weapon = dynamic_cast<ItemWeaponGhost *> (object);
        auto *human = dynamic_cast<ItemHumanGhost *> (object);
        auto *projectile = dynamic_cast<Projectile3DBody *> (object);
        auto *energy = dynamic_cast<ItemEnergyGhost *> (object);
        auto *enemiesEmitter = dynamic_cast<EnemyGhostEmitter *> (object);
        auto *bomb = dynamic_cast<ItemBombGhost *> (object);
        auto *particleEmitter = dynamic_cast<ParticleEmitter *> (object);

        if (enemy != nullptr ||
            health != nullptr ||
            weapon != nullptr ||
            projectile != nullptr ||
            energy != nullptr ||
            human != nullptr ||
            bomb != nullptr ||
            enemiesEmitter != nullptr ||
            salvage != nullptr
        ) {
            object->setEnabled(value);
        }

        if (particleEmitter != nullptr) {
            particleEmitter->setEnabled(value);
        }
    }

    for (auto spaceship: spaceships) {
        spaceship->setEnabled(false);
    }

    shaderEdgeObject->setEnabled(false);
}

void ComponentGame::selectSpaceshipAndStartGame()
{
    makeFadeToGameState(EngineSetup::GameState::PRESS_KEY_NEW_LEVEL, true);
    EngineSetup::get()->SOUND_VOLUME_MUSIC += 0;
    EngineSetup::get()->SOUND_CHANNEL_GLOBAL -= 0;
    Mix_Volume(EngineSetup::SoundChannels::SND_GLOBAL, (int) EngineSetup::get()->SOUND_CHANNEL_GLOBAL);
    Mix_VolumeMusic((int) EngineSetup::get()->SOUND_VOLUME_MUSIC);

    ComponentsManager::get()->getComponentSound()->sound("startIntro", EngineSetup::SoundChannels::SND_GLOBAL, 0);
    loadSelectedSpaceshipModel();
    spaceships[spaceshipSelectedIndex]->setEnabled(false);
    shaderEdgeObject->setEnabled(false);

    player->respawn();
}

void ComponentGame::launchSpaceshipSelector() const
{
    getFadeToGameState()->setSpeed(0.004);
    ComponentSound::stopChannel(EngineSetup::SoundChannels::SND_GLOBAL);
    ComponentsManager::get()->getComponentSound()->stopMusic();

    ComponentsManager::get()->getComponentSound()->sound("startIntro", EngineSetup::SoundChannels::SND_GLOBAL, 0);
    ComponentsManager::get()->getComponentMenu()->setMenuEnabled(false);
    makeFadeToGameState(EngineSetup::GameState::DIFFICULT_SELECTOR, true);
}

void ComponentGame::pressedKeyForWin() const
{
    makeFadeToGameState(EngineSetup::PRESS_KEY_NEW_LEVEL, true);
    ComponentsManager::get()->getComponentSound()->sound("tic", EngineSetup::SoundChannels::SND_GLOBAL, 0);
}

void ComponentGame::pressedKeyForBeginLevel()
{
    getFadeToGameState()->setSpeed(FADE_SPEED_FADEOUT_TIME);

    ComponentsManager::get()->getComponentSound()->sound("startGame", EngineSetup::SoundChannels::SND_GLOBAL, 0);

    if (getLevelLoader()->isHaveMusic()) {
        ComponentSound::fadeInMusic(
            ComponentsManager::get()->getComponentSound()->getSoundPackage().getMusicByLabel(getLevelLoader()->getMusic()), -1, 3000
        );
    }

    getLevelLoader()->startCountDown();
    setGameState(EngineSetup::GameState::COUNTDOWN);
}

void ComponentGame::pressedKeyByDead()
{
    player->respawn();

    ComponentsManager::get()->getComponentSound()->sound("soundMenuClick", EngineSetup::SoundChannels::SND_GLOBAL, 0);

    makeFadeToGameState(EngineSetup::GameState::PRESS_KEY_PREVIOUS_LEVEL, true);
    getPlayer()->startPlayerBlink();
}

void ComponentGame::updateShaders()
{
    shaderColor->update();
    shaderProjectiles->update();

    Vertex3D vel = ComponentsManager::get()->getComponentGame()->getPlayer()->getVelocity().getScaled(0.25);
    vel.z = 0;

    LevelLoader::moveBackgroundObjects(vel.getInverse());
}

void ComponentGame::shaderBackgroundUpdate()
{
    if (
        gameState == EngineSetup::GAMING ||
        gameState == EngineSetup::PRESS_KEY_GAMEOVER ||
        gameState == EngineSetup::COUNTDOWN ||
        gameState == EngineSetup::PRESS_KEY_BY_DEAD ||
        gameState == EngineSetup::PRESS_KEY_NEW_LEVEL ||
        gameState == EngineSetup::PRESS_KEY_BY_WIN ||
        gameState == EngineSetup::PRESS_KEY_PREVIOUS_LEVEL ||
        gameState == EngineSetup::GAMING_TUTORIAL ||
        gameState == EngineSetup::STORE
    ) {
        Vertex3D vel = ComponentsManager::get()->getComponentGame()->getPlayer()->getVelocity().getScaled(0.0075f);

        shaderBackgroundImage->update(vel.y, vel.x);
        shaderForegroundImage->update(vel.y * 0.5f, vel.x * 0.5f);
        ComponentsManager::get()->getComponentWindow()->getShaderOGLRender()->getDirectionalLight()->direction += glm::vec3(vel.x * 15, vel.y * 15, 0);
    }
}

void ComponentGame::zoomCameraCountDown()
{
    Vertex3D origin = ComponentsManager::get()->getComponentCamera()->getCamera()->getPosition();
    Vertex3D to = cameraInGamePosition;

    Vector3D direction(origin, to);
    auto counter = getLevelLoader()->getCountDown();

    const float t = counter->getAcumulatedTime() / counter->getStep();

    ComponentsManager::get()->getComponentCamera()->getCamera()->setPosition(origin + direction.getComponent().getScaled(t * 0.025f));

    const float alpha = std::fmax(0.0f, std::fmin(1.0f - (t * 2.5f), 1.0f));

    handleOnUpdateTutorialImages(alpha);
}

void ComponentGame::handleMenuGameState()
{
    ComponentsManager::get()->getComponentHUD()->setEnabled(false);
    ComponentsManager::get()->getComponentRender()->setEnabled(true);
    ComponentsManager::get()->getComponentMenu()->setEnabled(true);
    ComponentsManager::get()->getComponentCollisions()->setEnabled(false);

    getPlayer()->setEnabled(false);
    getPlayer()->stopBlinkForPlayer();
    getPlayer()->setEnergyShieldEnabled(false);
    getPlayer()->getWeapon()->setStatus(WeaponStatus::RELEASED);

    setVisibleInGameObjects(false);
}

void ComponentGame::handleGamingGameState()
{
    setVisibleInGameObjects(true);
    setEnemyWeaponsEnabled(true);
    setEnemiesBehaviors(true);

    getPlayer()->setEnabled(true);

    ComponentsManager::get()->getComponentHUD()->setEnabled(true);
    ComponentsManager::get()->getComponentMenu()->setEnabled(false);
    ComponentsManager::get()->getComponentRender()->setEnabled(true);
    ComponentsManager::get()->getComponentCollisions()->setEnabled(true);

    getFadeToGameState()->setSpeed(FADE_SPEED_FADEOUT_TIME);

    shaderColor->setEnabled(false);
    shaderProjectiles->setEnabled(true);
}

void ComponentGame::handleCountDownGameState()
{
    shaderProjectiles->setEnabled(true);
    ComponentsManager::get()->getComponentCollisions()->setEnabled(true);
    ComponentsManager::get()->getComponentHUD()->setEnabled(true);
    ComponentsManager::get()->getComponentMenu()->setEnabled(false);
    ComponentsManager::get()->getComponentRender()->setEnabled(true);
    ComponentsManager::get()->getComponentCamera()->getCamera()->setPosition(cameraCountDownPosition);
}

void ComponentGame::handlePressNewLevelKeyGameState()
{
    removeInGameObjects();

    getPlayer()->respawn();
    getPlayer()->setEnabled(true);
    ComponentsManager::get()->getComponentCamera()->getCamera()->setPosition(cameraCountDownPosition);
    ComponentsManager::get()->getComponentRender()->playLUAScripts();

    shaderBackgroundImage->resetOffsets();
    shaderForegroundImage->resetOffsets();

    ComponentsManager::get()->getComponentCollisions()->setEnabled(false);
    getLevelLoader()->loadNext();

    getPlayer()->getWeapon()->setStatus(WeaponStatus::RELEASED);
    getPlayer()->setEnergyShieldEnabled(false);
    getPlayer()->setPosition(playerStartPosition);
    setVisibleInGameObjects(true);
    ComponentsManager::get()->getComponentCollisions()->setEnabled(true);
    ComponentsManager::get()->getComponentHUD()->setEnabled(false);
    ComponentsManager::get()->getComponentMenu()->setEnabled(false);
    ComponentsManager::get()->getComponentRender()->setEnabled(true);
    getFadeToGameState()->setSpeed(FADE_SPEED_FADEOUT_TIME);

    if (getLevelLoader()->getTutorials().size() > 1) {
        ComponentSound::fadeInMusic(
                ComponentsManager::get()->getComponentSound()->getSoundPackage().getMusicByLabel("tutorial"), -1, 3000
        );
    }

    ComponentsManager::get()->getComponentSound()->sound("crt", EngineSetup::SoundChannels::SND_GLOBAL, 0);
}

void ComponentGame::reloadLevel(int level) const
{
    getLevelLoader()->setCurrentLevelIndex(level - 1);
    makeFadeToGameState(EngineSetup::PRESS_KEY_NEW_LEVEL, true);
    ComponentsManager::get()->getComponentSound()->sound("tic", EngineSetup::SoundChannels::SND_GLOBAL, 0);
}

void ComponentGame::handlePressKeyByDead()
{
    currentIndexDeadImage = Tools::random(0, (int) imagesDead.size()-1);

    ComponentsManager::get()->getComponentRender()->setSelectedObject(nullptr);

    shaderBackgroundImage->resetOffsets();
    shaderForegroundImage->resetOffsets();

    getPlayer()->getWeapon()->setStatus(WeaponStatus::RELEASED);

    ComponentsManager::get()->getComponentHUD()->setEnabled(true);
    ComponentsManager::get()->getComponentMenu()->setEnabled(false);
    ComponentsManager::get()->getComponentRender()->setEnabled(true);

    silenceInGameObjects();
}

void ComponentGame::handlePressKeyPreviousLevel()
{
    getPlayer()->setEnabled(true);
    removeInGameObjects();
    getLevelLoader()->reload();
    ComponentsManager::get()->getComponentCamera()->getCamera()->setPosition(cameraCountDownPosition);
    getPlayer()->getWeapon()->setStatus(WeaponStatus::RELEASED);
    getPlayer()->setEnergyShieldEnabled(false);
    getPlayer()->setPosition(playerStartPosition);
    getPlayer()->respawn();
    setVisibleInGameObjects(true);
    ComponentsManager::get()->getComponentHUD()->setEnabled(false);
    ComponentsManager::get()->getComponentMenu()->setEnabled(false);
    ComponentsManager::get()->getComponentRender()->setEnabled(true);
    shaderColor->setEnabled(false);
    getFadeToGameState()->setSpeed(FADE_SPEED_FADEOUT_TIME);
    ComponentsManager::get()->getComponentCamera()->getCamera()->setPosition(cameraCountDownPosition);
}

void ComponentGame::handlePressKeyGameOver()
{
    ComponentsManager::get()->getComponentHUD()->setEnabled(false);
    getPlayer()->setEnabled(true);
    ComponentSound::playMusic(ComponentsManager::get()->getComponentSound()->getSoundPackage().getMusicByLabel("gameOverMusic"), -1);
    setVisibleInGameObjects(false);
    removeInGameObjects();
    getPlayer()->stopBlinkForPlayer();
    getPlayer()->setEnergyShieldEnabled(false);
    getPlayer()->getWeapon()->setStatus(WeaponStatus::RELEASED);
}

void ComponentGame::handlePressKeyCredits()
{
    ComponentsManager::get()->getComponentMenu()->setMenuEnabled(false);
}

void ComponentGame::handlePressKeyHelp()
{
    ComponentsManager::get()->getComponentMenu()->setMenuEnabled(false);
}

FXColorTint *ComponentGame::getShaderColor() const {
    return shaderColor;
}

FXOffsetImage *ComponentGame::getShaderBackgroundImage() const {
    return shaderBackgroundImage;
}

FXOffsetImage *ComponentGame::getShaderForegroundImage() const {
    return shaderForegroundImage;
}

void ComponentGame::handleSplash()
{
    splashCounter.setEnabled(true);
}

void ComponentGame::addRayLightsToShaderLaserLine()
{
    if (gameState != EngineSetup::GAMING && gameState != EngineSetup::COUNTDOWN && gameState != EngineSetup::PRESS_KEY_BY_WIN) return;

    for (auto object : Brakeza3D::get()->getSceneObjects()) {
        const auto ray = dynamic_cast<ProjectileRay *> (object);

        if (ray != nullptr && ray->isEnabled()) {
            shaderProjectiles->addLaserFromRay(ray);
        }
    }
}

FXLaser *ComponentGame::getShaderLasers() const {
    return shaderProjectiles;
}

TextWriter *ComponentGame::getTextWriter() {
    return textWriter;
}

void ComponentGame::handlePressKeyByWin() const
{
    getLevelLoader()->getWaitingToWin()->setEnabled(false);
    ComponentsManager::get()->getComponentRender()->stopLUAScripts();
    removeInGameObjects();
}

void ComponentGame::setEnemiesBehaviors(bool value)
{
    for (auto &object : Brakeza3D::get()->getSceneObjects()) {
        auto enemy = dynamic_cast<EnemyGhost *> (object);

        if (enemy != nullptr) {
            if (enemy->getBehavior() != nullptr) {
                enemy->getBehavior()->setEnabled(value);
            }
        }
    }
}

void ComponentGame::setHelp(Image *help)
{
    ComponentGame::help = *help;

    ComponentsManager::get()->getComponentSound()->sound("crt", EngineSetup::SoundChannels::SND_GLOBAL, 0);
}

StoreManager *ComponentGame::getStoreManager() const {
    return storeManager;
}

void ComponentGame::loadSpaceship(const std::string& fileNameModel, const std::string& fileNameInformation, SpaceshipAttributes attr)
{
    auto model = new Mesh3D();
    model->setMultiScene(true);
    model->setRotation(0, 0, 0);
    model->setRotationFrameEnabled(true);
    model->setRotationFrame(Vertex3D(0, 1, 0));
    model->setEnabled(false);
    model->setAlpha(0);
    model->setEnableLights(true);
    model->setScale(2);
    model->setStencilBufferEnabled(true);
    model->AssimpLoadGeometryFromFile(std::string(EngineSetup::get()->MODELS_FOLDER + fileNameModel));
    model->setPosition(Vertex3D(-4, -1, 20));

    Brakeza3D::get()->addObject3D(model, "spaceshipOption_" + std::to_string(spaceships.size()));
    spaceships.push_back(model);

    spaceshipsInformation.push_back(new Image(EngineSetup::get()->IMAGES_FOLDER + fileNameInformation));
    spaceshipsAttributes.push_back(attr);
}

void ComponentGame::increaseSpaceshipSelected()
{
    spaceships[spaceshipSelectedIndex]->setEnabled(false);
    if (spaceshipSelectedIndex + 1 < (unsigned int) spaceships.size()) {
        spaceshipSelectedIndex++;
    }
    spaceships[spaceshipSelectedIndex]->setEnabled(true);
    shaderEdgeObject->setObject(spaceships[spaceshipSelectedIndex]);
}

void ComponentGame::decreaseSpaceshipSelected()
{
    spaceships[spaceshipSelectedIndex]->setEnabled(false);
    if (spaceshipSelectedIndex > 0) {
        spaceshipSelectedIndex--;
    }
    spaceships[spaceshipSelectedIndex]->setEnabled(true);
    shaderEdgeObject->setObject(spaceships[spaceshipSelectedIndex]);
}

void ComponentGame::setCurrentEnemyDialog(EnemyDialog *currentEnemyDialog)
{
    if (currentEnemyDialog != nullptr) {
        currentEnemyDialog->start();
    }
    ComponentGame::currentEnemyDialog = currentEnemyDialog;
}

void ComponentGame::handleSpaceShipSelector()
{
    getFadeToGameState()->setSpeed(0.004);
    SceneLoader::clearScene();
    ComponentsManager::get()->getComponentMenu()->setEnabled(false);
    spaceships[spaceshipSelectedIndex]->setEnabled(true);
    shaderEdgeObject->setEnabled(true);
    shaderEdgeObject->setObject(spaceships[spaceshipSelectedIndex]);
}

Sprite2D *ComponentGame::getRandomExplosionSprite() const
{
    return explosionSprites[Tools::random(0, (int) explosionSprites.size()-1)];
}

Sprite2D *ComponentGame::getFadeInSpriteRed()  {
    return fadeInSpriteRed;
}

Sprite2D *ComponentGame::getFadeInSpriteGreen() {
    return fadeInSpriteGreen;
}

Sprite2D *ComponentGame::getFadeInSpriteBlue() {
    return fadeInSpriteBlue;
}


Sprite2D *ComponentGame::getSpriteSparklesRed() const {
    return spriteSparklesRed;
}

Sprite2D *ComponentGame::getSpriteSparklesGreen() const {
    return spriteSparklesGreen;
}

Sprite2D *ComponentGame::getSpriteSparklesBlue() const {
    return spriteSparklesBlue;
}

void ComponentGame::handleOnUpdateMessageRadio()
{
    if (currentEnemyDialog == nullptr) return;

    currentEnemyDialog->update();
}

Sprite2D *ComponentGame::getSpriteStuck() const {
    return spriteStuck;
}

void ComponentGame::increaseHelpImage()
{
    if (currentHelpIndex + 1 < (int) helps.size()) {
        currentHelpIndex++;
    }
}

void ComponentGame::decreaseHelpImage()
{
    if (currentHelpIndex > 0) {
        currentHelpIndex--;
    }
}

void ComponentGame::writeDialogTextToContinue(const char *string)
{
    textWriter->setFont(ComponentsManager::get()->getComponentWindow()->getFontDefault());
    textWriter->writeTTFCenterHorizontal(575, string, PaletteColors::getMenuOptions(), 0.75f);
}

void ComponentGame::handleOnUpdateCountDown()
{
    //handleOnUpdateMessageRadio();
    zoomCameraCountDown();

    float oldAlpha = textWriter->getAlpha();
    float accumulatedTime = getLevelLoader()->getCountDown()->getAcumulatedTime();

    static float lastUpdateTime = 0;
    static int lastRestTime = -1;

    int restTime = (int) (getLevelLoader()->getCountDown()->getStep() - accumulatedTime + 1);

    if (restTime != lastRestTime) {
        lastUpdateTime = accumulatedTime;
        lastRestTime = restTime;
    }

    float timeSinceLastUpdate = accumulatedTime - lastUpdateTime;
    float timeRatio = 1 - timeSinceLastUpdate;

    textWriter->setAlpha(timeRatio);

    textWriter->writeTTFCenterHorizontal(250, std::to_string(restTime).c_str(), PaletteColors::getMenuOptions(), 6);

    getLevelLoader()->getCountDown()->update();

    if (getLevelLoader()->getCountDown()->isFinished()) {
        if (!getLevelLoader()->getMainMessage()->isEmpty()) {
            setCurrentEnemyDialog(getLevelLoader()->getMainMessage());
        }
        setGameState(EngineSetup::GameState::GAMING);
    }

    textWriter->setAlpha(oldAlpha);
}

void ComponentGame::handleOnUpdateHelp(const float alpha)
{
    const auto fb = ComponentsManager::get()->getComponentWindow()->getForegroundFramebuffer();
    boxTutorial.drawFlatAlpha(0, 0, alpha, fb);
    border.drawFlatAlpha(0, 0, alpha, fb);
    helps[currentHelpIndex]->drawFlatAlpha(0, 0, alpha, fb);
    if (helps.size() > 1) {
        std::string message = std::to_string(currentHelpIndex + 1) + " of " + std::to_string((int)helps.size());
        textWriter->writeTTFCenterHorizontal(495, message.c_str(), PaletteColors::getCrt(), 0.0f);
    }
    writeDialogTextToContinue("Press ESC to continue...");
}

void ComponentGame::handleOnUpdateStore(const float alpha)
{
    boxTutorial.drawFlatAlpha(0, 0, alpha, ComponentsManager::get()->getComponentWindow()->getForegroundFramebuffer());
    boxStore.drawFlatAlpha(0, 0, alpha, ComponentsManager::get()->getComponentWindow()->getForegroundFramebuffer());
    storeManager->update(alpha);
    writeDialogTextToContinue("Press ESC to continue...");
}

void ComponentGame::handleOnUpdateSpaceshipSelector(const float alpha)
{
    ComponentsManager::get()->getComponentCamera()->getCamera()->setPosition(Vertex3D(0, 0, 0));

    for (auto s: spaceships) {
        s->setAlpha(alpha);
    }
    const auto bb = ComponentsManager::get()->getComponentWindow()->getBackgroundFramebuffer();
    const auto fb = ComponentsManager::get()->getComponentWindow()->getForegroundFramebuffer();
    backgroundSpaceshipSelection.drawFlatAlpha(0, 0, alpha,  bb);
    spaceshipsInformation[spaceshipSelectedIndex]->drawFlatAlpha(0, 0, alpha, fb);
    border.drawFlatAlpha(0, 0, alpha,  fb);
    writeDialogTextToContinue("Press ENTER to select...");
}

void ComponentGame::handleOnUpdatePressKeyGameOver(const float alpha)
{
    auto fb = ComponentsManager::get()->getComponentWindow()->getForegroundFramebuffer();
    boxTutorial.drawFlatAlpha(0, 0, alpha, fb);
    border.drawFlatAlpha(0, 0, alpha, fb);
    imageEndGame.drawFlatAlpha(0, 0, alpha, fb);
    writeDialogTextToContinue("Press ENTER to continue...");
}

void ComponentGame::handleOnUpdateCredits(const float alpha)
{
    auto fb = ComponentsManager::get()->getComponentWindow()->getForegroundFramebuffer();
    boxTutorial.drawFlatAlpha(0, 0, alpha, fb);
    border.drawFlatAlpha(0, 0, alpha, fb);
    imageCredits.drawFlatAlpha(0, 0, alpha, fb);
    writeDialogTextToContinue("Press ESC to continue...");
}

void ComponentGame::handleOnUpdatePressKeyByDead(const float alpha)
{
    auto fb = ComponentsManager::get()->getComponentWindow()->getForegroundFramebuffer();

    boxTutorial.drawFlatAlpha(0, 0, alpha, fb);
    //Logging::Message("handleOnUpdatePressKeyByDead: %d", currentIndexDeadImage);
    auto deadImage = imagesDead[currentIndexDeadImage];
    deadImage->drawFlatAlpha(0, 0, alpha, fb);
    shaderColor->setProgress((1 - getFadeToGameState()->getProgress()) * 0.50f);
}

void ComponentGame::handleOnUpdateSplash(const float alpha)
{
    splashCounter.update();

    imageSplash.drawFlatAlpha(0, 0, alpha, ComponentsManager::get()->getComponentWindow()->getForegroundFramebuffer());

    if (splashCounter.isFinished() && splashCounter.isEnabled()) {
        splashCounter.setEnabled(false);
        getFadeToGameState()->setSpeed(FADE_SPEED_MENU_FIRST_TIME);
        makeFadeToGameState(EngineSetup::GameState::MENU, false);
        ComponentsManager::get()->getComponentMenu()->LoadScene();
        //videoPlayer->play();
    }
}

ShaderOpenGLLineLaser *ComponentGame::getShaderOGLLineLaser() {
    return shaderOGLLineLaser;
}

ShaderOpenGLImageOffset *ComponentGame::getShaderOGLImageOffset() const {
    return shaderOGLImageOffset;
}

TexturePackage *ComponentGame::getImages() {
    return &images;
}

void ComponentGame::handleOnUpdateIntro(float alpha)
{
    introCounter.update();

    if (!imagesIntro.empty()) {

        if (introCounter.isFinished()) {
            increaseIntroImage();
            introCounter.setEnabled(true);
        }
        auto window = ComponentsManager::get()->getComponentWindow();
        auto fb = window->getForegroundFramebuffer();
        auto bb = window->getBackgroundFramebuffer();

        float oldAlpha = textWriter->getAlpha();
        textWriter->setAlpha(alpha);

        //glassEffect.drawFlatAlpha(0, 0, alpha, fb);
        imagesIntro[currentIndexIntro]->drawFlatAlpha(0, 0, alpha, fb);

        if ((int) imagesIntro.size() > 1) {
            std::string message = std::to_string(currentIndexIntro + 1) + " of " + std::to_string((int)imagesIntro.size());
            textWriter->writeTTFCenterHorizontal(530, message.c_str(), PaletteColors::getMenuOptions(), 0.75f);
        }
        writeDialogTextToContinue("Press ENTER to START...");
        boxTutorial.drawFlatAlpha(0, 0, alpha, fb);
        textWriter->setAlpha(oldAlpha);
    }
}

void ComponentGame::decreaseIntroImage() {
    if (currentIndexIntro > 0) {
        ComponentsManager::get()->getComponentSound()->sound("slideMachine", EngineSetup::SoundChannels::SND_GLOBAL, 0);
        currentIndexIntro--;
    }
}

void ComponentGame::increaseIntroImage() {
    if (currentIndexIntro + 1 < (unsigned int) imagesIntro.size()) {
        ComponentsManager::get()->getComponentSound()->sound("slideMachine", EngineSetup::SoundChannels::SND_GLOBAL, 0);
        currentIndexIntro++;
    }
}

void ComponentGame::handleDifficultySelector() {

}

void ComponentGame::handleOnUpdateDifficultySelector(const float alpha)
{
    const char* options[4] = {"Easy", "Normal", "Hard", "BFM" };
    int indexRatio = (int) getLevelLoader()->difficultyRatio-1;

    auto fb = ComponentsManager::get()->getComponentWindow()->getForegroundFramebuffer();
    int startYPosition = 200;

    difficultyInformation[indexRatio]->drawFlatAlpha(0, 0, alpha, fb);
    border.drawFlatAlpha(0, 0, alpha, fb);
    boxTutorial.drawFlatAlpha(0, 0, alpha, fb);

    for (int i = 0; i < 4; i++) {
        if (indexRatio == i) {
            textWriter->writeTextTTFAutoSize(750, startYPosition + i * 50, options[i], PaletteColors::getStamina(), 1.0);
        } else {
            textWriter->writeTextTTFAutoSize(750, startYPosition + i * 50, options[i], PaletteColors::getMenuOptions(), 1.0);
        }
    }

    writeDialogTextToContinue("Press ENTER to continue...");
}

void ComponentGame::resetGame() {
    ComponentSound::stopChannel(EngineSetup::SoundChannels::SND_GLOBAL);
    ComponentsManager::get()->getComponentSound()->stopMusic();
    currentIndexIntro = 0;
    ComponentGame::removeInGameObjects();
    getLevelLoader()->setLevelStartedToPlay(false);
    getLevelLoader()->setCurrentLevelIndex(-1);
    getLevelLoader()->updateConfig(-1);
    SceneLoader::clearScene();
    ComponentsManager::get()->getComponentMenu()->LoadScene();

    ComponentSound::fadeInMusic(
        ComponentsManager::get()->getComponentSound()->getSoundPackage().getMusicByLabel("musicMainMenu"),
        -1,
        SPLASH_TIME * 1000
    );

    getFadeToGameState()->setSpeed(FADE_SPEED_FADEOUT_TIME);
    makeFadeToGameState(EngineSetup::GameState::MENU, true);
}

EnemyGhost* ComponentGame::getEnemyByName(const std::string& name)
{
    for (auto &object : Brakeza3D::get()->getSceneObjects()) {
        auto enemy = dynamic_cast<EnemyGhost *> (object);

        if (enemy != nullptr && !object->isRemoved()) {
            if (enemy->getName() == name) {
                return enemy;
            }
        }
    }
    return nullptr;
}