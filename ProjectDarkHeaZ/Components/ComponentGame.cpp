#include "ComponentGame.h"
#include "../../include/Components/ComponentCollisions.h"
#include "../../include/Brakeza3D.h"
#include "../Items/ItemBombGhost.h"

ComponentGame::ComponentGame()
:
    cameraCountDownPosition(Vertex3D(0, 3000, 5000)),
    cameraInGamePosition(Vertex3D(0, -1000, -1000)),
    textWriter(nullptr),
    fadeToGameState(nullptr),
    player(nullptr),
    shaderProjectiles(nullptr),
    imageCredits(nullptr),
    imageSplash(nullptr),
    imageCrossFire(nullptr),
    currentHelpIndex(0),
    levelLoader(nullptr),
    shaderBackgroundImage(nullptr),
    shaderForegroundImage(nullptr),
    shaderColor(nullptr),
    shaderShockWave(nullptr),
    spaceshipSelectedIndex(0),
    currentEnemyDialog(nullptr),
    gameState(EngineSetup::GameState::NONE)
{
}

void ComponentGame::onStart()
{
    Logging::head("ComponentGame onStart");

    Brakeza3D::get()->addObject3D(new LightPoint3D(11, 1, 0, 0, 0, Color(100, 16, 22), Color(15, 33, 92)), "epe");

    player = new Player();

    setGameState(EngineSetup::GameState::NONE);

    auto componentWindow = ComponentsManager::get()->getComponentWindow();
    textWriter = new TextWriter(componentWindow->getRenderer(), componentWindow->getFontDefault());

    fadeToGameState = new FaderToGameStates(Color(3, 3, 111), 0.01f, EngineSetup::GameState::SPLASH, false);

    imageCrossFire = new Image(SETUP->IMAGES_FOLDER + "crossfire.png");
    imageCredits = new Image(SETUP->IMAGES_FOLDER + "credits.png");
    helps.push_back(new Image(SETUP->IMAGES_FOLDER + SETUP->DEFAULT_HELP_IMAGE));
    helps.push_back(new Image(SETUP->IMAGES_FOLDER + "keyboard.png"));
    imageSplash = new Image(SETUP->IMAGES_FOLDER + SETUP->LOGO_BRAKEZA);
    imageCablesVertical = new Image(SETUP->IMAGES_FOLDER + "cables_hud_vertical.png");
    imageCablesHorizontal = new Image(SETUP->IMAGES_FOLDER + "cables_hud_horizontal.png");
    imageCablesStore = new Image(SETUP->IMAGES_FOLDER + "cables_hud_store.png");
    imageStatistics = new Image(SETUP->IMAGES_FOLDER + "statistics_screen.png");
    imageDead = new Image(SETUP->IMAGES_FOLDER + "game_over.png");
    imageEndGame = new Image(SETUP->IMAGES_FOLDER + "end_game.png");
    boxTutorial = new Image(SETUP->IMAGES_FOLDER + "tutorial_box.png");
    boxStore = new Image(SETUP->IMAGES_FOLDER + "store_box.png");

    splashCounter.setStep(SPLASH_TIME);

    ComponentsManager::get()->getComponentCollisions()->initBulletSystem();

    playerStartPosition = Vertex3D(-40, 2990, Z_COORDINATE_GAMEPLAY);

    auto componentCamera = ComponentsManager::get()->getComponentCamera();
    componentCamera->getCamera()->setPosition(cameraCountDownPosition);
    componentCamera->setAutoScroll(false);
    componentCamera->setAutoScrollSpeed(Vertex3D(0, -0.0, 0));
    componentCamera->setFreeLook(FREE_LOOK_ENABLED);

    ComponentsManager::get()->getComponentInput()->setEnabled(FREE_LOOK_ENABLED);
    ComponentsManager::get()->getComponentMenu()->setEnabled(false);

    initPlayer();
    loadWeapons();
    loadLevels();
    loadShaders();
    levelLoader->loadConfig();

    ComponentSound::fadeInMusic(
        ComponentsManager::get()->getComponentSound()->getSoundPackage().getMusicByLabel("musicMainMenu"),
        -1,
        SPLASH_TIME * 1000
    );

    swarm = new Swarm(Vertex3D(0, -1000, 10000), Vertex3D(1000, 1000, 500));
    swarm->setScale(10);
    swarm->addPredator(new SwarmObject(player));
    Brakeza3D::get()->addObject3D(swarm, "swarm");

    shaderExplosion = new ShaderExplosion(true, Color::white(), Color::green(), Point2D(320, 240), -1, OCParticlesContext::forSplash());
    shaderExplosion->setIntensity(1);

    dialogBackground = new ShaderImageMask(true, SETUP->IMAGES_FOLDER + "gridTutorial.png", SETUP->IMAGES_FOLDER + "/tutorial_mask.png");

    shaderCRT = new ShaderCRT(true, SETUP->IMAGES_FOLDER + "cloud.png", SETUP->IMAGES_FOLDER + "tutorial_mask.png");
    shaderCRT->setMaxAlpha(255);

    storeManager = new StoreManager(player, textWriter);

    itemBoxFrame = new Mesh3D();
    itemBoxFrame->setRotation(0, 0, 0);
    itemBoxFrame->setEnabled(false);
    itemBoxFrame->setAlpha(255);
    itemBoxFrame->setEnableLights(true);
    itemBoxFrame->setScale(1);
    itemBoxFrame->setStencilBufferEnabled(true);
    itemBoxFrame->AssimpLoadGeometryFromFile(std::string(EngineSetup::get()->MODELS_FOLDER + "frame_box.fbx"));

    loadSpaceship("spaceships/player.fbx", "spaceships/spaceship_01.png");
    loadSpaceship("spaceships/player02.fbx", "spaceships/spaceship_02.png");
    loadSpaceship("spaceships/player03.fbx", "spaceships/spaceship_03.png");
    loadSpaceship("spaceships/player04.fbx", "spaceships/spaceship_04.png");

    fadeInSpriteRed = new Sprite2D(0, 0, false, new TextureAnimated(std::string(EngineSetup::get()->SPRITES_FOLDER + "flash01.png"), 96, 96, 37, 45));
    fadeInSpriteGreen = new Sprite2D(0, 0, false, new TextureAnimated(std::string(EngineSetup::get()->SPRITES_FOLDER + "flash02.png"), 96, 96, 37, 45));
    fadeInSpriteBlue = new Sprite2D(0, 0, false, new TextureAnimated(std::string(EngineSetup::get()->SPRITES_FOLDER + "flash03.png"), 96, 96, 37, 45));

    spriteSparklesRed = new Sprite2D(0, 0, false, new TextureAnimated(std::string(EngineSetup::get()->SPRITES_FOLDER + "impact_a.png"), 48, 48, 9, 60));
    spriteSparklesGreen = new Sprite2D(0, 0, false, new TextureAnimated(std::string(EngineSetup::get()->SPRITES_FOLDER + "impact02.png"), 64, 64, 40, 160));
    spriteSparklesBlue = new Sprite2D(0, 0, false, new TextureAnimated(std::string(EngineSetup::get()->SPRITES_FOLDER + "impact03.png"), 64, 64, 40, 160));

    spriteStuck = new Sprite2D(0, 0, false, new TextureAnimated(std::string(EngineSetup::get()->SPRITES_FOLDER + "Smoke45Frames.png"), 128, 128, 45, 24));

    explosionSprites.push_back(new Sprite2D(0, 0, false, new TextureAnimated(std::string(EngineSetup::get()->SPRITES_FOLDER + "explosion_a.png"), 128, 128, 15, 35)));
    explosionSprites.push_back(new Sprite2D(0, 0, false, new TextureAnimated(std::string(EngineSetup::get()->SPRITES_FOLDER + "explosion_b.png"), 128, 128, 15, 35)));
    explosionSprites.push_back(new Sprite2D(0, 0, false, new TextureAnimated(std::string(EngineSetup::get()->SPRITES_FOLDER + "explosion_c.png"), 128, 128, 15, 35)));
    explosionSprites.push_back(new Sprite2D(0, 0, false, new TextureAnimated(std::string(EngineSetup::get()->SPRITES_FOLDER + "explosion_d.png"), 128, 128, 15, 35)));
}

void ComponentGame::loadShaders()
{
    shaderBackgroundImage = new ShaderImage(EngineSetup::get()->IMAGES_FOLDER + "cloud.png");
    shaderForegroundImage = new ShaderImage(EngineSetup::get()->IMAGES_FOLDER + "cloud.png");
    shaderBackgroundImage->setUseOffset(true);
    shaderForegroundImage->setUseOffset(true);
    shaderForegroundImage->setUseColors(false);
    shaderColor = new ShaderColor(false, PaletteColors::getStamina(), 0.75f);
    shaderProjectiles = new ShaderProjectiles();
    shaderShockWave = new ShaderShockWave(true);

    shaderEdgeObject = new ShaderEdgeObject(false, PaletteColors::getStatisticsText());
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

    delete imageCrossFire;
    delete imageCredits;
    delete imageSplash;

    delete storeManager;

    for (auto weapon: weapons) {
        delete weapon;
    }
}

void ComponentGame::preUpdate()
{
    shaderBackgroundUpdate();
}

void ComponentGame::onUpdate()
{
    updateFadeToGameState();
    updateEnemyTargetedCrossFire();
    addProjectilesToShaderLasers();
    updateShaders();

    const float alpha = 255 - getFadeToGameState()->getProgress() * 255;
    textWriter->setAlpha(alpha);

    switch(gameState) {
        case EngineSetup::SPLASH: {
            handleOnUpdateSplash(alpha);
            break;
        }
        case EngineSetup::PRESS_KEY_NEW_LEVEL:
        case EngineSetup::PRESS_KEY_PREVIOUS_LEVEL: {
            handleOnUpdateTutorialImages(255);
            break;
        }
        case EngineSetup::GAMING: {
            blockPlayerPositionInCamera();
            checkForEndLevel();
            handleOnUpdateMessageRadio();
            break;
        }
        case EngineSetup::COUNTDOWN: {
            handleOnUpdateCountDown();
            break;
        }
        case EngineSetup::PRESS_KEY_BY_WIN: {
            showLevelStatistics(alpha);
            break;
        }
        case EngineSetup::GAMING_TUTORIAL: {
            handleOnUpdateGamingTutorial(alpha);
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
        /*case EngineSetup::VAT: {
            Vertex3D origin = ComponentsManager::get()->getComponentCamera()->getCamera()->getPosition();
            Vertex3D to = player->getPosition() - player->AxisLeft().getScaled(-2000) - player->AxisUp().getScaled(-2000);

            Vector3D direction(origin, to);

            float t = Brakeza3D::get()->getDeltaTime();
            auto camera = ComponentsManager::get()->getComponentCamera()->getCamera();
            auto selected = ComponentsManager::get()->getComponentRender()->getSelectedObject();
            camera->setPosition(to);
            if (selected != nullptr) {
                camera->lookAt(selected);
            }
            break;
        }*/
        case EngineSetup::SPACESHIP_SELECTOR: {
            handleOnUpdateSpaceshipSelector(alpha);
            break;
        }
        case EngineSetup::PRESS_KEY_GAMEOVER: {
            handleOnUpdatePressKeyGameOver(alpha);
            break;
        }
    }
}

void ComponentGame::handleOnUpdateTutorialImages(float alpha)
{
    if (!getLevelLoader()->getTutorials().empty()) {
        imageCablesVertical->drawFlatAlpha(0, 0, alpha);

        float oldAlpha = textWriter->getAlpha();
        textWriter->setAlpha(alpha);
        textWriter->setFont(ComponentsManager::get()->getComponentWindow()->getFontAlternative());
        std::string message = std::to_string(getLevelLoader()->getCurrentTutorialIndex() + 1) + " / " + std::to_string((int)getLevelLoader()->getTutorials().size());
        if (getLevelLoader()->getTutorials().size() > 1) {
            textWriter->writeTTFCenterHorizontal(323, message.c_str(), PaletteColors::getCrt(), 0.3f);
        }
        getLevelLoader()->drawCurrentTutorialImage(alpha);
        dialogBackground->setMaxAlpha((int) alpha);
        dialogBackground->update();
        boxTutorial->drawFlatAlpha(0, 0, alpha);

        writeDialogTextToContinue("Press ENTER to start...");

        textWriter->setAlpha(oldAlpha);
        shaderCRT->setMaxAlpha((int) alpha);
        shaderCRT->update();
    }
}

void ComponentGame::drawMedalAlpha(int type, int x, int y, float alpha)
{
    auto hudTexturePackage = ComponentsManager::get()->getComponentHUD()->getHudTextures();
    switch(type) {
        case 0: { hudTexturePackage->getTextureByLabel("medalBronze")->drawFlatAlpha(x, y, alpha); break; }
        case 1: { hudTexturePackage->getTextureByLabel("medalSilver")->drawFlatAlpha(x, y, alpha); break; }
        case 2: { hudTexturePackage->getTextureByLabel("medalGold")->drawFlatAlpha(x, y, alpha); break; }
        default: break;
    }
}

void ComponentGame::showLevelStatistics(float alpha)
{
    imageCablesHorizontal->drawFlatAlpha(0, 0, alpha);

    textWriter->setFont(ComponentsManager::get()->getComponentWindow()->getFontAlternative());

    dialogBackground->setMaxAlpha((int)alpha);
    dialogBackground->update();
    boxTutorial->drawFlatAlpha(0, 0, alpha);

    shaderCRT->setMaxAlpha((int)alpha);
    shaderCRT->update();

    int offsetX = 160;
    const int space = 100;
    player->getWeaponTypeByLabel("projectile")->getIcon()->drawFlatAlpha(offsetX, 160, alpha);
    textWriter->writeTextTTFAutoSize(offsetX, 195, getLevelLoader()->getStats()->stats(WEAPON_PROJECTILE).c_str(),
                                     PaletteColors::getStatisticsText(), 0.3);
    textWriter->writeTextTTFAutoSize(offsetX, 220, getLevelLoader()->getStats()->accuracyPercentageFormatted(WEAPON_PROJECTILE).c_str(),
                                     PaletteColors::getStatisticsText(), 0.3);
    drawMedalAlpha(getLevelLoader()->getStats()->medalType(WEAPON_PROJECTILE), offsetX, 250, alpha);

    offsetX += space;
    player->getWeaponTypeByLabel("laser")->getIcon()->drawFlatAlpha(offsetX, 160, alpha);
    textWriter->writeTextTTFAutoSize(offsetX, 195, getLevelLoader()->getStats()->stats(WEAPON_LASER).c_str(),
                                     PaletteColors::getStatisticsText(), 0.3);
    textWriter->writeTextTTFAutoSize(offsetX, 220, getLevelLoader()->getStats()->accuracyPercentageFormatted(WEAPON_LASER).c_str(),
                                     PaletteColors::getStatisticsText(), 0.3);
    drawMedalAlpha(getLevelLoader()->getStats()->medalType(WEAPON_LASER), offsetX, 250, alpha);

    offsetX += space;
    player->getWeaponTypeByLabel("ray")->getIcon()->drawFlatAlpha(offsetX, 160, alpha);
    textWriter->writeTextTTFAutoSize(offsetX, 195, getLevelLoader()->getStats()->stats(WEAPON_RAYLIGHT).c_str(),
                                     PaletteColors::getStatisticsText(), 0.3);
    textWriter->writeTextTTFAutoSize(offsetX, 220, getLevelLoader()->getStats()->accuracyPercentageFormatted(WEAPON_RAYLIGHT).c_str(),
                                     PaletteColors::getStatisticsText(), 0.3);
    drawMedalAlpha(getLevelLoader()->getStats()->medalType(WEAPON_RAYLIGHT), offsetX, 250, alpha);

    offsetX += space;
    player->getWeaponTypeByLabel("bomb")->getIcon()->drawFlatAlpha(offsetX, 160, alpha);
    textWriter->writeTextTTFAutoSize(offsetX, 195, getLevelLoader()->getStats()->stats(WEAPON_BOMB).c_str(),
                                     PaletteColors::getStatisticsText(), 0.3);
    textWriter->writeTextTTFAutoSize(offsetX, 220, getLevelLoader()->getStats()->accuracyPercentageFormatted(WEAPON_BOMB).c_str(),
                                     PaletteColors::getStatisticsText(), 0.3);
    drawMedalAlpha(getLevelLoader()->getStats()->medalType(WEAPON_BOMB), offsetX, 250, alpha);

    textWriter->setFont(ComponentsManager::get()->getComponentWindow()->getFontAlternative());

    //textWriter->writeTTFCenterHorizontal(350, "Press ENTER to START!", primaryColor, 0.5f);

    imageStatistics->drawFlatAlpha(0, 0, alpha);

    writeDialogTextToContinue("Press ENTER to continue...");

    ComponentsManager::get()->getComponentHUD()->getHudTextures()->getTextureByLabel("coinIcon")->drawFlatAlpha(313  , 300, alpha);
    textWriter->writeTTFCenterHorizontal(320, std::to_string(getLevelLoader()->getStats()->coinsGained).c_str(),
                                         PaletteColors::getStatisticsText(), 0.3);
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

void ComponentGame::postUpdate()
{
    player->updateWeaponAutomaticStatus();
}

void ComponentGame::updateEnemyTargetedCrossFire()
{
    if (gameState != EngineSetup::GAMING) return;

    auto selected = ComponentsManager::get()->getComponentRender()->getSelectedObject();

    if (selected == nullptr) return;

    const auto direction = selected->getPosition() - spaceCrossFirePosition;

    const float speed = 0.4f;
    spaceCrossFirePosition = spaceCrossFirePosition + direction.getScaled(speed);

    imageCrossFireScreenPosition = Transforms::WorldToPoint(spaceCrossFirePosition, ComponentsManager::get()->getComponentCamera()->getCamera());

    imageCrossFireScreenPosition.x -= imageCrossFire->width() / 2;
    imageCrossFireScreenPosition.y -= imageCrossFire->height() / 2;

    imageCrossFire->drawFlat(imageCrossFireScreenPosition.x, imageCrossFireScreenPosition.y);

}

int ComponentGame::getLiveEnemiesCounter()
{
    int cont = 0 ;
    for (auto object : Brakeza3D::get()->getSceneObjects()) {
        auto *enemy = dynamic_cast<EnemyGhost *> (object);
        auto *enemiesEmitter = dynamic_cast<EnemyGhostEmitter *> (object);

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
            getPlayer()->setKillsCounter(0);
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
    auto *camera = ComponentsManager::get()->getComponentCamera()->getCamera();

    Vertex3D homogeneousPosition;
    Vertex3D destinyPoint = player->getPosition() + player->getVelocity();
    Transforms::cameraSpace(homogeneousPosition, destinyPoint, camera);
    homogeneousPosition = Transforms::PerspectiveNDCSpace(homogeneousPosition, camera->getFrustum());

    if (homogeneousPosition.y > 1) {
        player->setPosition(player->getPosition() + ComponentsManager::get()->getComponentCamera()->getAutoScrollSpeed());
        if (player->getVelocity().y > 0) {
            Vertex3D newVelocity = player->getVelocity();
            newVelocity.y = -1;
            player->setVelocity(newVelocity);
        }
    }

    // top
    if (homogeneousPosition.y < -1) {
        if (player->getVelocity().y < 0) {
            Vertex3D newVelocity = player->getVelocity();
            newVelocity.y = 0;
            player->setVelocity(newVelocity);
        }
    }

    //right
    if (homogeneousPosition.x > 1) {
        if (player->getVelocity().x > 0) {
            Vertex3D newVelocity = player->getVelocity();
            newVelocity.x = -2;
            player->setVelocity(newVelocity);
        }
    }

    //left;
    if (homogeneousPosition.x < -1) {
        if (player->getVelocity().x < 0) {
            Vertex3D newVelocity = player->getVelocity();
            newVelocity.x = 2;
            player->setVelocity(newVelocity);
        }
    }
}

EngineSetup::GameState ComponentGame::getGameState() {
    return gameState;
}

void ComponentGame::loadSelectedSpaceshipModel()
{
    player->clone(spaceships[spaceshipSelectedIndex]);
    player->updateBoundingBox();
    player->makeSimpleGhostBody(
        Vertex3D(600, 600, 600),
        ComponentsManager::get()->getComponentCollisions()->getDynamicsWorld(),
        EngineSetup::collisionGroups::Player,
        EngineSetup::collisionGroups::AllFilter
    );
}

void ComponentGame::initPlayer()
{
    player->setLabel("player");
    player->setRotation(0, 0, 0);
    player->setEnabled(false);
    player->setAlpha(255);
    player->setEnableLights(true);
    player->setPosition(playerStartPosition);
    player->setScale(1);
    player->setStamina(100);
    player->setStencilBufferEnabled(true);
    Brakeza3D::get()->addObject3D(player, "player");

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

PlayerReflection *ComponentGame::getClosestReflection(Vertex3D from) const
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

void ComponentGame::loadLevels()
{
    auto basePath = EngineSetup::get()->CONFIG_FOLDER + "Levels/";
    
    levelLoader = new LevelLoader(basePath + "level01.json");
    levelLoader->addLevel(basePath + "level02.json");
    levelLoader->addLevel(basePath + "level03.json");
    levelLoader->addLevel(basePath + "level04.json");
    levelLoader->addLevel(basePath + "level05.json");
    levelLoader->addLevel(basePath + "level06.json");
    levelLoader->addLevel(basePath + "level07.json");
    levelLoader->addLevel(basePath + "level08.json");
    levelLoader->addLevel(basePath + "level09.json");
    levelLoader->addLevel(basePath + "level10.json");
    levelLoader->addLevel(basePath + "level11.json");
    levelLoader->addLevel(basePath + "level12.json");
    levelLoader->addLevel(basePath + "level13.json");
    levelLoader->addLevel(basePath + "level14.json");
    levelLoader->addLevel(basePath + "level15.json");
    levelLoader->addLevel(basePath + "level16.json");
    levelLoader->addLevel(basePath + "level17.json");
    levelLoader->addLevel(basePath + "level18.json");
    levelLoader->addLevel(basePath + "level19.json");
    levelLoader->addLevel(basePath + "level20.json");
    levelLoader->addLevel(basePath + "level21.json");
    levelLoader->addLevel(basePath + "level22.json");
    levelLoader->addLevel(basePath + "level23.json");
    levelLoader->addLevel(basePath + "level24.json");
    levelLoader->addLevel(basePath + "level25.json");
    levelLoader->addLevel(basePath + "level26.json");
    levelLoader->addLevel(basePath + "level27.json");
    levelLoader->addLevel(basePath + "level28.json");
    levelLoader->addLevel(basePath + "level29.json");
    levelLoader->addLevel(basePath + "level30.json");
}


FaderToGameStates *ComponentGame::getFadeToGameState() const
{
    return fadeToGameState;
}

LevelLoader *ComponentGame::getLevelLoader() const
{
    return levelLoader;
}

void ComponentGame::removeProjectiles() const
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

    getLevelLoader()->removeBackgroundObjects();
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
        auto projectileRay = dynamic_cast<ProjectileRay *> (object);

        if (projectileRay != nullptr) {
            projectileRay->setEnabled(true);
        }

        if (enemy != nullptr) {
            if (enemy->getProjectileEmitter() != nullptr) {
                enemy->getProjectileEmitter()->setActive(value);
            }
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

    for (auto object: getLevelLoader()->objectsBackground) {
        object->setEnabled(value);
    }

    for (auto spaceship: spaceships) {
        spaceship->setEnabled(false);
    }

    shaderEdgeObject->setEnabled(false);
}

void ComponentGame::loadWeapons()
{
    std::string sndPath = EngineSetup::get()->SOUNDS_FOLDER;
    std::string filePath = EngineSetup::get()->CONFIG_FOLDER + EngineSetup::get()->CFG_WEAPONS;

    Logging::Message("Loading weapons from file %s", filePath.c_str());

    size_t file_size;
    auto contentFile = Tools::readFile(filePath, file_size);

    cJSON *myDataJSON = cJSON_Parse(contentFile);

    if (myDataJSON == nullptr) {
        Logging::Message("[Load Weapons] Can't be loaded: %s", filePath.c_str());
        exit(-1);
    }

    cJSON *currentWeapon;
    cJSON_ArrayForEach(currentWeapon, cJSON_GetObjectItemCaseSensitive(myDataJSON, "weapons")) {
        auto weapon = getLevelLoader()->parseWeaponJSON(currentWeapon);
        weapon->setSoundChannel(1);
        weapons.push_back(weapon);
    }

    for (auto weapon : weapons) {
        player->addWeapon(weapon);
    }

    player->setWeaponTypeByIndex(0);

    free (contentFile);
    cJSON_Delete(myDataJSON);
}

const std::vector<Weapon *> &ComponentGame::getWeapons() const
{
    return weapons;
}

void ComponentGame::selectSpaceshipAndStartGame()
{
    makeFadeToGameState(EngineSetup::GameState::PRESS_KEY_NEW_LEVEL, true);
    ComponentsManager::get()->getComponentSound()->sound("tic", EngineSetup::SoundChannels::SND_GLOBAL, 0);
    loadSelectedSpaceshipModel();
    spaceships[spaceshipSelectedIndex]->setEnabled(false);
    shaderEdgeObject->setEnabled(false);

    player->respawn();
}

void ComponentGame::pressedKeyForNewGameOrResumeGame()
{
    if (!getLevelLoader()->isLevelStartedToPlay()) {
        getFadeToGameState()->setSpeed(FADE_SPEED_FADEOUT_TIME);

        ComponentsManager::get()->getComponentSound()->sound("levelCompleted", EngineSetup::SoundChannels::SND_GLOBAL, 0);
        ComponentsManager::get()->getComponentMenu()->setMenuEnabled(false);
        makeFadeToGameState(EngineSetup::GameState::SPACESHIP_SELECTOR, true);

    } else {
        makeFadeToGameState(EngineSetup::GameState::GAMING, true);
    }
}

void ComponentGame::pressedKeyForWin()
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
    getPlayer()->startPlayerBlink();
}

void ComponentGame::pressedKeyForFinishGameAndRestart()
{
    ComponentSound::fadeInMusic(ComponentsManager::get()->getComponentSound()->getSoundPackage().getMusicByLabel("musicMainMenu"), -1, 3000);
    getLevelLoader()->setCurrentLevelIndex(-1);
    makeFadeToGameState(EngineSetup::GameState::MENU, true);
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
    shaderShockWave->update();
    shaderExplosion->update();

    Vertex3D vel = ComponentsManager::get()->getComponentGame()->getPlayer()->getVelocity().getScaled(0.000015);
    vel.z = 0;

    levelLoader->moveBackgroundObjects(vel.getScaled(20000).getInverse());
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
        gameState == EngineSetup::GAMING_TUTORIAL
    ) {
        Vertex3D vel = ComponentsManager::get()->getComponentGame()->getPlayer()->getVelocity().getScaled(0.000015);

        shaderBackgroundImage->update(vel.x, vel.y);
        shaderForegroundImage->update(vel.x * 0.5, vel.y * 0.5);
    }
}

void ComponentGame::zoomCameraCountDown()
{
    Vertex3D origin = ComponentsManager::get()->getComponentCamera()->getCamera()->getPosition();
    Vertex3D to = cameraInGamePosition;

    Vector3D direction(origin, to);
    auto counter = getLevelLoader()->getCountDown();

    float t = counter->getAcumulatedTime() / counter->getStep();

    ComponentsManager::get()->getComponentCamera()->getCamera()->setPosition(origin + direction.getComponent().getScaled(t * 0.025f));

    int alpha = 255 - (t * 255 * 2.5);
    alpha = std::max(0, std::min(alpha, 255));

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

    ComponentsManager::get()->getComponentCamera()->getCamera()->setPosition(cameraInGamePosition);

    setVisibleInGameObjects(false);
}

void ComponentGame::handleGamingGameState()
{
    setVisibleInGameObjects(true);
    setEnemyWeaponsEnabled(true);
    setEnemiesBehaviors(true);

    getPlayer()->setEnabled(true);
    getPlayer()->startPlayerBlink();

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
    getPlayer()->getRayLight().setEnabled(false);
    ComponentsManager::get()->getComponentCamera()->getCamera()->setPosition(cameraCountDownPosition);

    shaderBackgroundImage->resetOffsets();
    shaderForegroundImage->resetOffsets();

    ComponentsManager::get()->getComponentCollisions()->setEnabled(false);
    getLevelLoader()->loadNext();

    getPlayer()->getWeapon()->setStatus(WeaponStatus::RELEASED);
    getPlayer()->setEnergyShieldEnabled(false);
    getPlayer()->setPosition(playerStartPosition);
    setVisibleInGameObjects(true);
    ComponentsManager::get()->getComponentHUD()->setEnabled(false);
    ComponentsManager::get()->getComponentMenu()->setEnabled(false);
    ComponentsManager::get()->getComponentRender()->setEnabled(true);
    getFadeToGameState()->setSpeed(FADE_SPEED_FADEOUT_TIME);

    ComponentSound::fadeInMusic(
        ComponentsManager::get()->getComponentSound()->getSoundPackage().getMusicByLabel("tutorial"), -1, 3000
    );

    ComponentsManager::get()->getComponentSound()->sound("crt", EngineSetup::SoundChannels::SND_GLOBAL, 0);
}

void ComponentGame::reloadLevel(int level)
{
    removeInGameObjects();
    getPlayer()->respawn();
    getPlayer()->setEnabled(true);

    ComponentsManager::get()->getComponentCamera()->getCamera()->setPosition(cameraInGamePosition);

    shaderBackgroundImage->resetOffsets();
    shaderForegroundImage->resetOffsets();

    getLevelLoader()->load(level);

    getPlayer()->getWeapon()->setStatus(WeaponStatus::RELEASED);
    getPlayer()->setEnergyShieldEnabled(false);
    getPlayer()->setPosition(playerStartPosition);
    setVisibleInGameObjects(true);

    ComponentsManager::get()->getComponentHUD()->setEnabled(false);
    ComponentsManager::get()->getComponentMenu()->setEnabled(false);
    ComponentsManager::get()->getComponentRender()->setEnabled(true);

    getLevelLoader()->startCountDown();
    gameState = EngineSetup::GameState::PRESS_KEY_PREVIOUS_LEVEL;
    getPlayer()->startPlayerBlink();
}

void ComponentGame::handlePressKeyByDead()
{
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
    getLevelLoader()->loadPrevious();
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
    ComponentSound::fadeInMusic(
        ComponentsManager::get()->getComponentSound()->getSoundPackage().getMusicByLabel(getLevelLoader()->getMusic()),
        -1,
        3000
    );
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

ShaderColor *ComponentGame::getShaderColor() const {
    return shaderColor;
}

ShaderImage *ComponentGame::getShaderBackgroundImage() const {
    return shaderBackgroundImage;
}

ShaderImage *ComponentGame::getShaderForegroundImage() const {
    return shaderForegroundImage;
}

void ComponentGame::handleSplash()
{
    splashCounter.setEnabled(true);
}

void ComponentGame::addProjectilesToShaderLasers()
{
    if (gameState != EngineSetup::GAMING && gameState != EngineSetup::COUNTDOWN && gameState != EngineSetup::PRESS_KEY_BY_WIN) return;

    for (auto object : Brakeza3D::get()->getSceneObjects()) {
        const auto projectile = dynamic_cast<AmmoProjectileBody *> (object);
        const auto ray = dynamic_cast<ProjectileRay *> (object);
        const auto wave = dynamic_cast<ShockWave *> (object);

        if (projectile != nullptr && !projectile->isWasCollision()) {
            shaderProjectiles->addProjectile(
                object->getPosition(),
                projectile->getColor(),
                projectile->getIntensity()
            );
        }

        if (wave != nullptr) {
            shaderShockWave->addShockWave(wave);
        }

        if (ray != nullptr && ray->isEnabled()) {
            shaderProjectiles->addLaserFromRay(ray);
        }
    }
}

ShaderProjectiles *ComponentGame::getShaderLasers() const {
    return shaderProjectiles;
}

TextWriter *ComponentGame::getTextWriter() {
    return textWriter;
}

void ComponentGame::handlePressKeyByWin()
{
    getLevelLoader()->getWaitingToWin()->setEnabled(false);
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
    ComponentGame::help = help;

    ComponentsManager::get()->getComponentSound()->sound("crt", EngineSetup::SoundChannels::SND_GLOBAL, 0);
}

StoreManager *ComponentGame::getStoreManager() const {
    return storeManager;
}

Mesh3D *ComponentGame::getItemBoxFrame() const {
    return itemBoxFrame;
}

void ComponentGame::loadSpaceship(const std::string& fileNameModel, const std::string& fileNameInformation)
{
    auto model = new Mesh3D();
    model->setLayer(Mesh3DRenderLayer::SECONDARY);
    model->setRotation(0, 0, 0);
    model->setRotationFrameEnabled(true);
    model->setRotationFrame(Vertex3D(0, 1, 0));
    model->setEnabled(false);
    model->setAlpha(255);
    model->setEnableLights(true);
    model->setScale(1);
    model->setStencilBufferEnabled(true);
    model->AssimpLoadGeometryFromFile(std::string(EngineSetup::get()->MODELS_FOLDER + "/" + fileNameModel));
    model->setPosition(Vertex3D(-1300, -1300, 4100));

    Brakeza3D::get()->addObject3D(model, "spaceshipForSelection_" + std::to_string(spaceships.size()));
    spaceships.push_back(model);

    spaceshipsInformation.push_back(new Image(EngineSetup::get()->IMAGES_FOLDER + "/" + fileNameInformation));
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
    spaceships[spaceshipSelectedIndex]->setEnabled(true);
    shaderEdgeObject->setEnabled(true);

    shaderEdgeObject->setObject(spaceships[spaceshipSelectedIndex]);

    ComponentsManager::get()->getComponentMenu()->hexagonStation->setEnabled(false);

}

Sprite2D *ComponentGame::getExplosionSprite() const
{
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    const int index = std::rand() % explosionSprites.size();

    return explosionSprites[index];
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

Swarm *ComponentGame::getSwarm() const {
    return swarm;
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
    textWriter->writeTTFCenterHorizontal(365, string, PaletteColors::getPressKeyToContinue(), 0.25);
}

void ComponentGame::handleOnUpdateCountDown()
{
    handleOnUpdateMessageRadio();
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

    textWriter->setAlpha(timeRatio * 255);

    textWriter->writeTTFCenterHorizontal(140, std::to_string(restTime).c_str(), PaletteColors::getEnergy(), 2);

    getLevelLoader()->getCountDown()->update();

    if (getLevelLoader()->getCountDown()->isFinished()) {
        if (getLevelLoader()->getMainMessage() != nullptr) {
            setCurrentEnemyDialog(getLevelLoader()->getMainMessage());
        }
        setGameState(EngineSetup::GameState::GAMING);
    }

    textWriter->setAlpha(oldAlpha);
}

void ComponentGame::handleOnUpdateGamingTutorial(const float alpha)
{
    imageCablesHorizontal->drawFlatAlpha(0, 0, alpha);

    dialogBackground->setMaxAlpha((int) alpha);
    dialogBackground->update();
    boxTutorial->drawFlatAlpha(0, 0, alpha);
    shaderCRT->setMaxAlpha((int) alpha);
    shaderCRT->update();

    writeDialogTextToContinue("Press ENTER to continue...");

    help->drawFlatAlpha(0, 0, alpha);
    handleOnUpdateMessageRadio();

}

void ComponentGame::handleOnUpdateHelp(const float alpha)
{
    dialogBackground->setMaxAlpha((int) alpha);
    dialogBackground->update();
    boxTutorial->drawFlatAlpha(0, 0, alpha);

    shaderCRT->setMaxAlpha((int) alpha);
    shaderCRT->update();

    writeDialogTextToContinue("Press ESC to continue...");

    helps[currentHelpIndex]->drawFlatAlpha(0, 0, alpha);

    std::string message = std::to_string(currentHelpIndex + 1) + " / " + std::to_string((int)helps.size());
    if (helps.size() > 1) {
        textWriter->writeTTFCenterHorizontal(323, message.c_str(), PaletteColors::getCrt(), 0.3f);
    }

    ComponentsManager::get()->getComponentMenu()->getBorder()->drawFlatAlpha(0, 0, alpha);
}

void ComponentGame::handleOnUpdateStore(const float alpha)
{
    imageCablesStore->drawFlatAlpha(0, 0, alpha);

    dialogBackground->setMaxAlpha((int) alpha);
    dialogBackground->update();

    shaderCRT->setMaxAlpha((int) alpha);
    shaderCRT->update();

    boxTutorial->drawFlatAlpha(0, 0, alpha);
    boxStore->drawFlatAlpha(0, 0, alpha);

    storeManager->update(alpha);

    writeDialogTextToContinue("Press ESC to continue...");

}

void ComponentGame::handleOnUpdateSpaceshipSelector(const float alpha)
{
    spaceshipsInformation[spaceshipSelectedIndex]->drawFlatAlpha(0, 0, alpha);

    imageCablesStore->drawFlatAlpha(0, 0, alpha);

    dialogBackground->setMaxAlpha((int) alpha);
    dialogBackground->update();

    boxTutorial->drawFlatAlpha(0, 0, alpha);
    ComponentsManager::get()->getComponentMenu()->getBorder()->drawFlatAlpha(0, 0, alpha);

    writeDialogTextToContinue("Press ENTER to select ...");

    shaderCRT->setMaxAlpha((int) alpha);
    shaderCRT->update();
    shaderEdgeObject->update();
}

void ComponentGame::handleOnUpdatePressKeyGameOver(const float alpha)
{
    imageCablesStore->drawFlatAlpha(0, 0, alpha);

    boxTutorial->drawFlatAlpha(0, 0, alpha);
    ComponentsManager::get()->getComponentMenu()->getBorder()->drawFlatAlpha(0, 0, alpha);

    dialogBackground->setMaxAlpha((int) alpha);
    dialogBackground->update();

    writeDialogTextToContinue("Press ENTER to continue...");

    shaderCRT->setMaxAlpha((int) alpha);
    shaderCRT->update();

    imageEndGame->drawFlatAlpha(0, 0, alpha);
}

void ComponentGame::handleOnUpdateCredits(const float alpha)
{
    imageCredits->drawFlatAlpha(0, 0, alpha);

    dialogBackground->setMaxAlpha((int) alpha);
    dialogBackground->update();
    boxTutorial->drawFlatAlpha(0, 0, alpha);
    shaderCRT->setMaxAlpha((int) alpha);
    shaderCRT->update();

    writeDialogTextToContinue("Press ESC to continue...");

    ComponentsManager::get()->getComponentMenu()->getBorder()->drawFlatAlpha(0, 0, alpha);
}

void ComponentGame::handleOnUpdatePressKeyByDead(const float alpha)
{
    imageDead->drawFlatAlpha(0, 0, alpha);
    shaderColor->setProgress((1 - getFadeToGameState()->getProgress()) * 0.50f);
}

void ComponentGame::handleOnUpdateSplash(const float alpha)
{
    splashCounter.update();

    imageSplash->drawFlatAlpha(0, 0, alpha);

    if (splashCounter.isFinished() && splashCounter.isEnabled()) {
        splashCounter.setEnabled(false);
        //setGameState(EngineSetup::GameState::MENU);
        getFadeToGameState()->setSpeed(FADE_SPEED_MENU_FIRST_TIME);
        makeFadeToGameState(EngineSetup::GameState::MENU, false);

        //videoPlayer->play();
    }

}
