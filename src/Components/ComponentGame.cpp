#include "../../include/Components/ComponentGame.h"
#include "../../include/Components/ComponentCollisions.h"
#include "../../include/Brakeza3D.h"
#include "../../darkheaz/src/items/ItemBombGhost.h"

ComponentGame::ComponentGame()
:
    cameraCountDownPosition(Vertex3D(0, 3000, 5000)),
    cameraInGamePosition(Vertex3D(0, -1000, -1000)),
    textWriter(nullptr),
    fadeToGameState(nullptr),
    player(nullptr),
    shaderLasers(nullptr),
    explosionSpriteTemplate(nullptr),
    imageCredits(nullptr),
    imageHelp(nullptr),
    imageSplash(nullptr),
    imageCrossFire(nullptr),
    levelLoader(nullptr),
    shaderBackgroundImage(nullptr),
    shaderColor(nullptr),
    shaderShockWave(nullptr),
    gameState(EngineSetup::GameState::NONE),
    primaryColor(Color(118, 185, 32)),
    secondaryColor(Color(118, 185, 32)),
    thirdColor(Color(0, 0, 255))
{
}

void ComponentGame::onStart()
{
    player = new Player();

    Logging::Log("ComponentGame onStart");

    setGameState(EngineSetup::GameState::NONE);

    auto componentWindow = ComponentsManager::get()->getComponentWindow();
    textWriter = new TextWriter(componentWindow->getRenderer(), componentWindow->getFontDefault());

    fadeToGameState = new FaderToGameStates(
        Color(3, 3, 111),
        0.01f,
        EngineSetup::GameState::SPLASH,
        false
    );

    imageCrossFire = new Image(SETUP->IMAGES_FOLDER + "crossfire.png");
    imageCredits = new Image(SETUP->IMAGES_FOLDER + "credits.png");
    imageHelp = new Image(SETUP->IMAGES_FOLDER + SETUP->DEFAULT_HELP_IMAGE);
    imageSplash = new Image(SETUP->IMAGES_FOLDER + SETUP->LOGO_BRAKEZA);
    imageCablesVertical = new Image(SETUP->IMAGES_FOLDER + "cables_hud_vertical.png");
    imageCablesHorizontal = new Image(SETUP->IMAGES_FOLDER + "cables_hud_horizontal.png");
    imageCablesStore = new Image(SETUP->IMAGES_FOLDER + "cables_hud_store.png");
    imageStatistics = new Image(SETUP->IMAGES_FOLDER + "statistics_screen.png");
    splashCounter.setStep(SPLASH_TIME);

    ComponentsManager::get()->getComponentCollisions()->initBulletSystem();

    playerStartPosition = Vertex3D(-40, 2990, Z_COORDINATE_GAMEPLAY);

    ComponentsManager::get()->getComponentCamera()->getCamera()->setPosition(cameraCountDownPosition);

    ComponentsManager::get()->getComponentCamera()->setAutoScroll(false);
    ComponentsManager::get()->getComponentCamera()->setAutoScrollSpeed(Vertex3D(0, -0.0, 0));

    ComponentsManager::get()->getComponentCamera()->setFreeLook(FREE_LOOK_ENABLED);
    ComponentsManager::get()->getComponentInput()->setEnabled(FREE_LOOK_ENABLED);
    ComponentsManager::get()->getComponentMenu()->setEnabled(false);

    videoPlayer = new VideoPlayer(SETUP->VIDEOS_FOLDER + "0000-1326.mp4");

    loadPlayer();
    loadWeapons();
    loadLevels();
    loadShaders();
    loadGameFonts();

    swarm = new Swarm(Vertex3D(0, -1000, 500), Vertex3D(1000, 1000, 500));

    for (int i = 0; i < 250; i++) swarm->createBoid(EngineSetup::get()->MODELS_FOLDER + "red_pill.fbx");
    for (int i = 0; i < 1; i++) swarm->createPredator(EngineSetup::get()->MODELS_FOLDER + "pill.fbx");

    //Brakeza3D::get()->addObject3D(swarm, "swarm");

    ComponentSound::fadeInMusic(
        ComponentsManager::get()->getComponentSound()->getSoundPackage().getMusicByLabel("musicMainMenu"),
        -1,
        SPLASH_TIME * 1000
    );

    shaderExplosion = new ShaderExplosion(
        true,
        Color::white(),
        Color::green(),
        Point2D(320, 240),
        -1,
        OCParticlesContext(
            0,
            0.025f,
            10.00f,
            0.0f,
            0.0f,
            0.0000025f,
            100.0f,
            200.0f,
            100.0f,
            1.0f,
            0.999f
        )
    );
    shaderExplosion->setIntensity(1);

    dialogBackground = new ShaderImageMask(true, SETUP->IMAGES_FOLDER + "gridTutorial.png", SETUP->IMAGES_FOLDER + "/tutorial_mask.png");
    boxTutorial = new Image(SETUP->IMAGES_FOLDER + "tutorial_box.png");

    shaderCRT = new ShaderCRT(true, SETUP->IMAGES_FOLDER + "cloud.png", SETUP->IMAGES_FOLDER + "tutorial_mask.png");
    shaderCRT->setMaxAlpha(255);

    boxStore = new Image(SETUP->IMAGES_FOLDER + "store_box.png");

    storeManager = new StoreManager(player, textWriter);
}

void ComponentGame::loadShaders()
{
    shaderBackgroundImage = new ShaderImage(EngineSetup::get()->IMAGES_FOLDER + "cloud.png");
    shaderColor = new ShaderColor(false, Color::red(), 0.75f);
    shaderLasers = new ShaderProjectiles();
    shaderShockWave = new ShaderShockWave(true);
    shaderBackgroundImage->setUseOffset(true);
}

ComponentGame::~ComponentGame()
{
    delete player;

    delete fadeToGameState;
    delete levelLoader;

    delete shaderLasers;
    delete shaderBackgroundImage;
    delete shaderColor;

    delete explosionSpriteTemplate;
    delete imageCrossFire;
    delete imageCredits;
    delete imageHelp;
    delete imageSplash;

    delete storeManager;

    for (auto weapon: weapons) {
        delete weapon;
    }
}

void ComponentGame::preUpdate()
{
    shaderBackgroundUpdate();

    if (gameState == EngineSetup::GameState::SPLASH) {
        splashCounter.update();

        imageSplash->drawFlatAlpha(0, 0, 255 - getFadeToGameState()->getProgress() * 255);

        if (splashCounter.isFinished() && splashCounter.isEnabled()) {
            splashCounter.setEnabled(false);
            //setGameState(EngineSetup::GameState::MENU);
            getFadeToGameState()->setSpeed(FADE_SPEED_MENU_FIRST_TIME);
            makeFadeToGameState(EngineSetup::GameState::MENU, false);

            //videoPlayer->play();
        }
    }

    getPlayer()->updateWeaponInteractionStatus();

    textWriter->setAlpha(255 - getFadeToGameState()->getProgress() * 255);

    switch(gameState) {
        case EngineSetup::PRESS_KEY_BY_DEAD: {
            textWriter->writeTextTTFMiddleScreen("GAME OVER", Color::black(), 0.5f);
            shaderColor->setProgress((1 - getFadeToGameState()->getProgress()) * 0.50f);
            break;
        }
        case EngineSetup::GAMING: {
            blockPlayerPositionInCamera();
            checkForEndLevel();
            break;
        }
        case EngineSetup::PRESS_KEY_GAMEOVER: {
            textWriter->writeTextTTFMiddleScreen("congratulations! END GAME...", primaryColor, 0.5);
            break;
        }

        case EngineSetup::INTRO: {
            if (videoPlayer->isFinished() ) {
                videoPlayer->finished = false;
                makeFadeToGameState(EngineSetup::GameState::MENU, true);
            }
            break;
        }
        case EngineSetup::NONE:
        case EngineSetup::SPLASH:
        case EngineSetup::MENU:
            break;

    }
}

void ComponentGame::onUpdate()
{
    updateFadeToGameState();
    updateEnemyTargetedCrossFire();
    addProjectilesToShaderLasers();
    updateShaders();

    shaderExplosion->update();

    switch(gameState) {
        case EngineSetup::PRESS_KEY_NEW_LEVEL:
        case EngineSetup::PRESS_KEY_PREVIOUS_LEVEL: {
            handleTutorialImages(255);
            break;
        }
        case EngineSetup::COUNTDOWN: {
            zoomCameraCountDown();
            float oldAlpha = textWriter->getAlpha();
            // Obtén el tiempo total del contador y el tiempo acumulado
            float accumulatedTime = getLevelLoader()->getCountDown()->getAcumulatedTime();

            static float lastUpdateTime = 0;
            static int lastRestTime = -1;

            int restTime = (int) (getLevelLoader()->getCountDown()->getStep() - accumulatedTime + 1);

            if (restTime != lastRestTime) {
                lastUpdateTime = accumulatedTime;
                lastRestTime = restTime;
            }

            // Calcula la proporción de tiempo restante en el segundo actual
            float timeSinceLastUpdate = accumulatedTime - lastUpdateTime;
            float timeRatio = 1 - timeSinceLastUpdate;

            // Establece el alfa en función de la proporción de tiempo restante
            textWriter->setAlpha(timeRatio * 255);

            textWriter->writeTTFCenterHorizontal(120, std::to_string(restTime).c_str(), secondaryColor, 3);

            getLevelLoader()->getCountDown()->update();

            if (getLevelLoader()->getCountDown()->isFinished()) {
                setGameState(EngineSetup::GameState::GAMING);
            }

            textWriter->setAlpha(oldAlpha);
            break;
        }
        case EngineSetup::PRESS_KEY_BY_WIN: {
            const float alpha = 255 - getFadeToGameState()->getProgress() * 255;
            showLevelStatistics(alpha);
            break;
        }
        case EngineSetup::GAMING_TUTORIAL: {
            const float alpha = 255 - getFadeToGameState()->getProgress() * 255;
            imageCablesHorizontal->drawFlatAlpha(0, 0, alpha);

            dialogBackground->setMaxAlpha((int) alpha);
            dialogBackground->update();
            boxTutorial->drawFlatAlpha(0, 0, alpha);
            shaderCRT->setMaxAlpha((int) alpha);
            shaderCRT->update();
            textWriter->writeTTFCenterHorizontal(362, "Press ENTER to continue...", Color::black(), 0.2);

            help->drawFlatAlpha(0, 0, alpha);
            break;
        }
        case EngineSetup::HELP: {
            const float alpha = 255 - getFadeToGameState()->getProgress() * 255;

            dialogBackground->setMaxAlpha((int) alpha);
            dialogBackground->update();
            boxTutorial->drawFlatAlpha(0, 0, alpha);

            shaderCRT->setMaxAlpha((int) alpha);
            shaderCRT->update();
            textWriter->writeTTFCenterHorizontal(362, "Press ESC to continue...", Color::black(), 0.2);

            imageHelp->drawFlatAlpha(0, 0, alpha);

            ComponentsManager::get()->getComponentMenu()->getBorder()->drawFlatAlpha(0, 0, alpha);
            break;
        }
        case EngineSetup::STORE: {
            const float alpha = 255 - getFadeToGameState()->getProgress() * 255;

            imageCablesStore->drawFlatAlpha(0, 0, alpha);

            dialogBackground->setMaxAlpha((int) alpha);
            dialogBackground->update();

            shaderCRT->setMaxAlpha((int) alpha);
            shaderCRT->update();

            boxTutorial->drawFlatAlpha(0, 0, alpha);
            boxStore->drawFlatAlpha(0, 0, alpha);

            storeManager->update(alpha);

            textWriter->writeTTFCenterHorizontal(362, "Press ESC to continue...", Color::black(), 0.2);
            break;
        }
        case EngineSetup::CREDITS: {
            const float alpha = 255 - getFadeToGameState()->getProgress() * 255;

            imageCredits->drawFlatAlpha(0, 0, alpha);

            dialogBackground->setMaxAlpha((int) alpha);
            dialogBackground->update();
            boxTutorial->drawFlatAlpha(0, 0, alpha);
            shaderCRT->setMaxAlpha((int) alpha);
            shaderCRT->update();
            textWriter->writeTTFCenterHorizontal(362, "Press ESC to continue...", Color::black(), 0.2);

            ComponentsManager::get()->getComponentMenu()->getBorder()->drawFlatAlpha(0, 0, alpha);
            break;
        }
    }
}

void ComponentGame::handleTutorialImages(float alpha)
{
    if (!getLevelLoader()->getTutorials().empty()) {
        imageCablesVertical->drawFlatAlpha(0, 0, alpha);

        float oldAlpha = textWriter->getAlpha();
        textWriter->setAlpha(alpha);
        std::string message = std::to_string(getLevelLoader()->getCurrentTutorialIndex() + 1) + " / " + std::to_string((int)getLevelLoader()->getTutorials().size());
        if (getLevelLoader()->getTutorials().size() > 1) {
            textWriter->writeTTFCenterHorizontal(323, message.c_str(), primaryColor, 0.3f);
        }
        getLevelLoader()->drawCurrentTutorialImage(alpha);
        dialogBackground->setMaxAlpha((int) alpha);
        dialogBackground->update();
        boxTutorial->drawFlatAlpha(0, 0, alpha);

        textWriter->writeTTFCenterHorizontal(362, "Press ENTER to start...", Color::black(), 0.2);

        textWriter->setAlpha(oldAlpha);
        shaderCRT->setMaxAlpha((int) alpha);
        shaderCRT->update();
    }
}

void ComponentGame::drawMedalAlpha(int type, int x, int y, float alpha)
{
    auto hudTexturePackage = ComponentsManager::get()->getComponentHUD()->getHudTextures();
    switch(type) {
        case 0: { hudTexturePackage->getTextureByLabel("medalBronze")->getImage()->drawFlatAlpha(x, y, alpha); break; }
        case 1: { hudTexturePackage->getTextureByLabel("medalSilver")->getImage()->drawFlatAlpha(x, y, alpha); break; }
        case 2: { hudTexturePackage->getTextureByLabel("medalGold")->getImage()->drawFlatAlpha(x, y, alpha); break; }
        default: break;
    }
}

void ComponentGame::showLevelStatistics(float alpha)
{
    imageCablesHorizontal->drawFlatAlpha(0, 0, alpha);

    textWriter->setFont(fontGameAlternative);

    dialogBackground->setMaxAlpha(alpha);
    dialogBackground->update();
    boxTutorial->drawFlatAlpha(0, 0, alpha);

    shaderCRT->setMaxAlpha(alpha);
    shaderCRT->update();

    int offsetX = 160;
    const int space = 100;
    player->getWeaponTypeByLabel("projectile")->getIcon()->drawFlatAlpha(offsetX, 160, alpha);
    textWriter->writeTextTTFAutoSize(offsetX, 195, getLevelLoader()->getStats()->stats(WEAPON_PROJECTILE).c_str(), secondaryColor, 0.3);
    textWriter->writeTextTTFAutoSize(offsetX, 220, getLevelLoader()->getStats()->accuracyPercentageFormatted(WEAPON_PROJECTILE).c_str(), secondaryColor, 0.3);
    drawMedalAlpha(getLevelLoader()->getStats()->medalType(WEAPON_PROJECTILE), offsetX, 250, alpha);

    offsetX += space;
    player->getWeaponTypeByLabel("laser")->getIcon()->drawFlatAlpha(offsetX, 160, alpha);
    textWriter->writeTextTTFAutoSize(offsetX, 195, getLevelLoader()->getStats()->stats(WEAPON_LASER_PROJECTILE).c_str(), secondaryColor, 0.3);
    textWriter->writeTextTTFAutoSize(offsetX, 220, getLevelLoader()->getStats()->accuracyPercentageFormatted(WEAPON_LASER_PROJECTILE).c_str(), secondaryColor, 0.3);
    drawMedalAlpha(getLevelLoader()->getStats()->medalType(WEAPON_LASER_PROJECTILE), offsetX, 250, alpha);

    offsetX += space;
    player->getWeaponTypeByLabel("ray")->getIcon()->drawFlatAlpha(offsetX, 160, alpha);
    textWriter->writeTextTTFAutoSize(offsetX, 195, getLevelLoader()->getStats()->stats(WEAPON_LASER_RAY).c_str(), secondaryColor, 0.3);
    textWriter->writeTextTTFAutoSize(offsetX, 220, getLevelLoader()->getStats()->accuracyPercentageFormatted(WEAPON_LASER_RAY).c_str(), secondaryColor, 0.3);
    drawMedalAlpha(getLevelLoader()->getStats()->medalType(WEAPON_LASER_RAY), offsetX, 250, alpha);

    offsetX += space;
    player->getWeaponTypeByLabel("bomb")->getIcon()->drawFlatAlpha(offsetX, 160, alpha);
    textWriter->writeTextTTFAutoSize(offsetX, 195, getLevelLoader()->getStats()->stats(WEAPON_BOMB).c_str(), secondaryColor, 0.3);
    textWriter->writeTextTTFAutoSize(offsetX, 220, getLevelLoader()->getStats()->accuracyPercentageFormatted(WEAPON_BOMB).c_str(), secondaryColor, 0.3);
    drawMedalAlpha(getLevelLoader()->getStats()->medalType(WEAPON_BOMB), offsetX, 250, alpha);

    textWriter->setFont(fontGame);

    //textWriter->writeTTFCenterHorizontal(350, "Press ENTER to START!", primaryColor, 0.5f);

    imageStatistics->drawFlatAlpha(0, 0, alpha);

    textWriter->writeTTFCenterHorizontal(362, "Press ENTER to continue...", Color::black(), 0.2);
    auto iconCon = ComponentsManager::get()->getComponentHUD()->getHudTextures()->getTextureByLabel("coinIcon");
    iconCon->getImage()->drawFlatAlpha(313  , 300, alpha);
    textWriter->writeTTFCenterHorizontal(320, std::to_string(getLevelLoader()->getStats()->coinsGained).c_str(), secondaryColor, 0.3);
}

void ComponentGame::updateFadeToGameState()
{
    getFadeToGameState()->onUpdate();
    if (getFadeToGameState()->isEndFadeOut()) {
        getFadeToGameState()->setEndFadeOut(false);
        setGameState(getFadeToGameState()->getGameStateWhenEnds());
    }

    if (getFadeToGameState()->isFinished()) {
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

    if (selected != nullptr) {
        auto direction = selected->getPosition() - spaceCrossFirePosition;

        const float speed = 0.4f;
        spaceCrossFirePosition = spaceCrossFirePosition + direction.getScaled(speed);

        imageCrossFireScreenPosition = Transforms::WorldToPoint(spaceCrossFirePosition, ComponentsManager::get()->getComponentCamera()->getCamera());

        imageCrossFireScreenPosition.x -= imageCrossFire->width() / 2;
        imageCrossFireScreenPosition.y -= imageCrossFire->height() / 2;

        imageCrossFire->drawFlat(imageCrossFireScreenPosition.x, imageCrossFireScreenPosition.y);
    };
}

int ComponentGame::getLiveEnemiesCounter()
{
    int cont = 0 ;
    for (auto object : Brakeza3D::get()->getSceneObjects()) {
        auto *enemy = dynamic_cast<EnemyGhost *> (object);
        auto *enemiesEmitter = dynamic_cast<EnemyGhostEmitter *> (object);

        if (enemy != nullptr || enemiesEmitter != nullptr) {
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
            handleGoIntoGamingTutorial();
            break;
        case EngineSetup::STORE: {
            break;
        }
    }

    this->gameState = state;

    Logging::Log("GameState changed to: %d", state);
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

void ComponentGame::loadPlayer()
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
    player->setAutoRotationToFacingSelectedObjectSpeed(5);
    player->AssimpLoadGeometryFromFile(std::string(EngineSetup::get()->MODELS_FOLDER + "spaceships/player.fbx"));
    player->updateBoundingBox();
    player->makeSimpleGhostBody(
        Vertex3D(500, 500, 500),
        ComponentsManager::get()->getComponentCollisions()->getDynamicsWorld(),
        EngineSetup::collisionGroups::Player,
        EngineSetup::collisionGroups::AllFilter
    );
    Brakeza3D::get()->addObject3D(player, "player");

    player->onStartSetup();

    explosionSpriteTemplate = new Sprite3D(EngineSetup::get()->BILLBOARD_WIDTH_DEFAULT, EngineSetup::get()->BILLBOARD_HEIGHT_DEFAULT);
    explosionSpriteTemplate->addAnimation(std::string(EngineSetup::get()->SPRITES_FOLDER + "explosion/explosion"), 12, 30);
    explosionSpriteTemplate->setAnimation(0);

    radioWave = new TextureAnimated(std::string(EngineSetup::get()->SPRITES_FOLDER + "radio/radio"), 7, 10);
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
    levelLoader = new LevelLoader(EngineSetup::get()->CONFIG_FOLDER + "level01.json");
    levelLoader->addLevel(EngineSetup::get()->CONFIG_FOLDER + "level02.json");
    levelLoader->addLevel(EngineSetup::get()->CONFIG_FOLDER + "level03.json");
    levelLoader->addLevel(EngineSetup::get()->CONFIG_FOLDER + "level04.json");
    levelLoader->addLevel(EngineSetup::get()->CONFIG_FOLDER + "level05.json");
    levelLoader->addLevel(EngineSetup::get()->CONFIG_FOLDER + "level06.json");
    levelLoader->addLevel(EngineSetup::get()->CONFIG_FOLDER + "level07.json");
    levelLoader->addLevel(EngineSetup::get()->CONFIG_FOLDER + "level08.json");
    levelLoader->addLevel(EngineSetup::get()->CONFIG_FOLDER + "level09.json");
    levelLoader->addLevel(EngineSetup::get()->CONFIG_FOLDER + "level10.json");
    levelLoader->addLevel(EngineSetup::get()->CONFIG_FOLDER + "level11.json");
    levelLoader->addLevel(EngineSetup::get()->CONFIG_FOLDER + "level12.json");
    levelLoader->addLevel(EngineSetup::get()->CONFIG_FOLDER + "level13.json");
    levelLoader->addLevel(EngineSetup::get()->CONFIG_FOLDER + "level14.json");
    levelLoader->addLevel(EngineSetup::get()->CONFIG_FOLDER + "level15.json");
    levelLoader->addLevel(EngineSetup::get()->CONFIG_FOLDER + "level16.json");
    levelLoader->addLevel(EngineSetup::get()->CONFIG_FOLDER + "level17.json");
    levelLoader->addLevel(EngineSetup::get()->CONFIG_FOLDER + "level18.json");
    levelLoader->addLevel(EngineSetup::get()->CONFIG_FOLDER + "level19.json");
    levelLoader->addLevel(EngineSetup::get()->CONFIG_FOLDER + "level20.json");
    levelLoader->addLevel(EngineSetup::get()->CONFIG_FOLDER + "level30.json");
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
        auto *enemyDialog = dynamic_cast<EnemyDialog *> (object);
        auto *health = dynamic_cast<ItemHealthGhost *> (object);
        auto *weapon = dynamic_cast<ItemWeaponGhost *> (object);
        auto *projectile = dynamic_cast<Projectile3DBody *> (object);
        auto *projectileEmitter = dynamic_cast<AmmoProjectileBodyEmitter *> (object);
        auto *particleEmitter = dynamic_cast<ParticleEmitter *> (object);
        auto bomb = dynamic_cast<ItemBombGhost *> (object);
        auto projectileRay = dynamic_cast<ProjectileRay *> (object);

        if (enemy != nullptr) {
            enemy->remove();
            continue;
        }

        if (enemyDialog != nullptr) {
            enemyDialog->setRemoved(true);
            continue;
        }
        if (health != nullptr) {
            health->remove();
            continue;
        }
        if (weapon != nullptr) {
            weapon->remove();
            continue;
        }
        if (projectile != nullptr) {
            projectile->setRemoved(true);
            continue;
        }
        if (projectileRay != nullptr) {
            projectileRay->setRemoved(true);
            continue;
        }
        if (projectileEmitter != nullptr) {
            projectileEmitter->setRemoved(true);
            continue;
        }
        if (particleEmitter) {
            particleEmitter->setRemoved(true);
            continue;
        }

        if (bomb != nullptr) {
            bomb->remove();
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

            if (enemy->getBehavior() != nullptr) {
                enemy->getBehavior()->setEnabled(true);
            }
        }
    }
}

void ComponentGame::setVisibleInGameObjects(bool value) {
    for (auto object: Brakeza3D::get()->getSceneObjects()) {
        auto *enemy = dynamic_cast<EnemyGhost *> (object);
        auto *health = dynamic_cast<ItemHealthGhost *> (object);
        auto *weapon = dynamic_cast<ItemWeaponGhost *> (object);
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
            bomb != nullptr ||
            enemiesEmitter != nullptr
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
}

void ComponentGame::loadWeapons()
{
    Logging::Log("Loading Weapons for game...");

    std::string sndPath = EngineSetup::get()->SOUNDS_FOLDER;
    std::string filePath = EngineSetup::get()->CONFIG_FOLDER + "playerWeapons.json";

    size_t file_size;
    auto contentFile = Tools::readFile(filePath, file_size);

    cJSON *myDataJSON = cJSON_Parse(contentFile);

    if (myDataJSON == nullptr) {
        Logging::Log("Can't be loaded: %s", filePath.c_str());
        return;
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

void ComponentGame::pressedKeyForNewGame()
{
    if (!getLevelLoader()->isLevelStartedToPlay()) {
        getFadeToGameState()->setSpeed(FADE_SPEED_FADEOUT_TIME);
        makeFadeToGameState(EngineSetup::GameState::PRESS_KEY_NEW_LEVEL, true);
        ComponentsManager::get()->getComponentSound()->sound("levelCompleted", EngineSetup::SoundChannels::SND_GLOBAL, 0);

        player->respawn();
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

    ComponentsManager::get()->getComponentSound()->sound("startGame", EngineSetup::SoundChannels::SND_GLOBAL, 0);

    makeFadeToGameState(EngineSetup::GameState::PRESS_KEY_PREVIOUS_LEVEL, true);
    getPlayer()->startPlayerBlink();
}

void ComponentGame::updateShaders()
{
    shaderColor->update();
    shaderLasers->update();
    shaderShockWave->update();

    Vertex3D vel = ComponentsManager::get()->getComponentGame()->getPlayer()->getVelocity().getScaled(0.000015);
    Vertex3D offset(vel.x, vel.y, 0);

    levelLoader->moveBackgroundObjects(offset.getScaled(20000).getInverse());
}

void ComponentGame::shaderBackgroundUpdate() {
    if (
        gameState == EngineSetup::GAMING ||
        gameState == EngineSetup::PRESS_KEY_GAMEOVER ||
        gameState == EngineSetup::COUNTDOWN ||
        gameState == EngineSetup::PRESS_KEY_BY_DEAD ||
        gameState == EngineSetup::PRESS_KEY_NEW_LEVEL ||
        gameState == EngineSetup::PRESS_KEY_BY_WIN ||
        gameState == EngineSetup::PRESS_KEY_PREVIOUS_LEVEL ||
        gameState == EngineSetup::GAMING_TUTORIAL ||
        gameState == EngineSetup::GAMING_TUTORIAL
    ) {
        shaderBackgroundImage->update();
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

    handleTutorialImages(alpha);
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

    getPlayer()->setEnabled(true);
    getPlayer()->startPlayerBlink();

    ComponentsManager::get()->getComponentHUD()->setEnabled(true);
    ComponentsManager::get()->getComponentMenu()->setEnabled(false);
    ComponentsManager::get()->getComponentRender()->setEnabled(true);
    ComponentsManager::get()->getComponentCollisions()->setEnabled(true);

    getFadeToGameState()->setSpeed(FADE_SPEED_FADEOUT_TIME);

    shaderColor->setEnabled(false);
    shaderLasers->setEnabled(true);

}

void ComponentGame::handleCountDownGameState()
{
    shaderLasers->setEnabled(true);
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
    getPlayer()->getShaderLaser().setEnabled(false);
    ComponentsManager::get()->getComponentCamera()->getCamera()->setPosition(cameraCountDownPosition);

    shaderBackgroundImage->resetOffsets();

    getLevelLoader()->loadNext();
    getPlayer()->getWeapon()->setStatus(WeaponStatus::RELEASED);
    getPlayer()->setEnergyShieldEnabled(false);
    getPlayer()->setGravityShieldsNumber(0);
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

    getLevelLoader()->load(level);

    getPlayer()->getWeapon()->setStatus(WeaponStatus::RELEASED);
    getPlayer()->setEnergyShieldEnabled(false);
    getPlayer()->setGravityShieldsNumber(0);
    getPlayer()->setPosition(playerStartPosition);
    setVisibleInGameObjects(true);

    ComponentsManager::get()->getComponentHUD()->setEnabled(true);
    ComponentsManager::get()->getComponentMenu()->setEnabled(false);
    ComponentsManager::get()->getComponentRender()->setEnabled(true);

    getLevelLoader()->startCountDown();
    setGameState(EngineSetup::GameState::PRESS_KEY_PREVIOUS_LEVEL);
    getPlayer()->startPlayerBlink();
}

void ComponentGame::handlePressKeyByDead()
{
    ComponentsManager::get()->getComponentRender()->setSelectedObject(nullptr);

    shaderBackgroundImage->resetOffsets();

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
    getPlayer()->setGravityShieldsNumber(0);
    getPlayer()->setPosition(playerStartPosition);
    getPlayer()->respawn();
    setVisibleInGameObjects(true);
    ComponentsManager::get()->getComponentHUD()->setEnabled(false);
    ComponentsManager::get()->getComponentMenu()->setEnabled(false);
    ComponentsManager::get()->getComponentRender()->setEnabled(true);
    shaderColor->setEnabled(false);
    getFadeToGameState()->setSpeed(FADE_SPEED_FADEOUT_TIME);
    ComponentSound::fadeInMusic(ComponentsManager::get()->getComponentSound()->getSoundPackage().getMusicByLabel(
            getLevelLoader()->getMusic()), -1, 3000);
    ComponentsManager::get()->getComponentCamera()->getCamera()->setPosition(cameraCountDownPosition);
}

void ComponentGame::handlePressKeyGameOver()
{
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

const Color &ComponentGame::getPrimaryColor() const {
    return primaryColor;
}

const Color &ComponentGame::getSecondaryColor() const {
    return secondaryColor;explosionSpriteTemplate;
}

ShaderColor *ComponentGame::getShaderColor() const {
    return shaderColor;
}

ShaderImage *ComponentGame::getShaderBackgroundImage() const {
    return shaderBackgroundImage;
}

void ComponentGame::handleSplash()
{
    splashCounter.setEnabled(true);
}

void ComponentGame::addProjectilesToShaderLasers()
{
    if (gameState != EngineSetup::GAMING && gameState != EngineSetup::COUNTDOWN && gameState != EngineSetup::PRESS_KEY_BY_WIN) return;

    for (auto object : Brakeza3D::get()->getSceneObjects()) {
        if (object->isRemoved()) continue;

        auto projectile = dynamic_cast<AmmoProjectileBody *> (object);
        auto ray = dynamic_cast<ProjectileRay *> (object);
        auto wave = dynamic_cast<ShockWave *> (object);

        if (projectile != nullptr) {
            shaderLasers->addProjectile(
                object->getPosition(),
                projectile->getColor(),
                (float) projectile->getWeaponType()->getSpeed()
            );
        }

        if (wave != nullptr) {
            shaderShockWave->addShockWave(wave);
        }

        if (ray != nullptr) {
            shaderLasers->addLaserFromRay(ray);
        }
    }
}

ShaderProjectiles *ComponentGame::getShaderLasers() const {
    return shaderLasers;
}

TextWriter *ComponentGame::getTextWriter() {
    return textWriter;
}

void ComponentGame::handlePressKeyByWin()
{
    getLevelLoader()->getWaitingToWin()->setEnabled(false);
    removeInGameObjects();
}

VideoPlayer *ComponentGame::getVideoPlayer()
{
    return videoPlayer;
}

void ComponentGame::loadGameFonts()
{
    fontGame = TTF_OpenFont(std::string(EngineSetup::get()->FONTS_FOLDER + "TroubleFont.ttf").c_str(), 50);

    if (!fontGame)  {
        Logging::Log(TTF_GetError());
        exit(-1);
    }

    fontGameAlternative = TTF_OpenFont(std::string(EngineSetup::get()->FONTS_FOLDER + "Open24DisplaySt.ttf").c_str(), 50);
    if (!fontGameAlternative)  {
        Logging::Log(TTF_GetError());
        exit(-1);
    }

}

TTF_Font *ComponentGame::getFontGame() const {
    return fontGame;
}

TextureAnimated *ComponentGame::getRadioWave() const {
    return radioWave;
}

Sprite3D *ComponentGame::getExplosionSpriteTemplate() const {
    return explosionSpriteTemplate;
}

void ComponentGame::handleGoIntoGamingTutorial()
{
    for (auto &object : Brakeza3D::get()->getSceneObjects()) {
        auto enemy = dynamic_cast<EnemyGhost *> (object);

        if (enemy != nullptr) {
            if (enemy->getBehavior() != nullptr) {
                enemy->getBehavior()->setEnabled(false);
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

