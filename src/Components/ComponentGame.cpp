#include "../../include/Components/ComponentGame.h"
#include "../../include/Components/ComponentCollisions.h"
#include "../../include/Brakeza3D.h"
#include "../../darkheaz/src/items/ItemWeaponGhost.h"
#include "../../darkheaz/src/items/ItemEnergyGhost.h"
#include "../../include/Misc/VideoPlayer.h"
#include "../../darkheaz/src/items/ItemBombGhost.h"

#define FREELOOK false
#define SPLASH_TIME 3.0f
#define FADE_SPEED_START_GAME 0.04
#define FADE_SPEED_ENDLEVEL 0.04
#define FADE_SPEED_FROM_MENU_TO_GAMING 0.04
#define FADE_SPEED_PRESSKEY_NEWLEVEL 0.04

ComponentGame::ComponentGame() :
    player(nullptr),
    shaderLasers(nullptr),
    shaderClouds(nullptr),
    shaderBackgroundImage(nullptr),
    shaderTrailBuffer(nullptr),
    shaderColor(nullptr),
    gameState(EngineSetup::GameState::NONE),
    primaryColor(Color(255, 0, 0)),
    secondaryColor(Color(1, 179, 52)),
    thirdColor(Color(0, 0, 255))
{
}

void ComponentGame::onStart()
{
    player = new Player();

    Logging::Log("ComponentGame onStart");

    setGameState(EngineSetup::GameState::NONE);

    textWriter = new TextWriter(
        ComponentsManager::get()->getComponentWindow()->getRenderer(),
        ComponentsManager::get()->getComponentWindow()->getFontDefault(),
        std::string(SETUP->SPRITES_FOLDER + "conchars2.png").c_str()
    );

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
    splashCounter.setStep(SPLASH_TIME);

    ComponentsManager::get()->getComponentCollisions()->initBulletSystem();

    playerStartPosition = Vertex3D(-40, 2990, Z_COORDINATE_GAMEPLAY);

    cameraCountDownPosition = Vertex3D(0, 3000, 5000);
    cameraInGamePosition = Vertex3D(0, -1000, -1000);
    ComponentsManager::get()->getComponentCamera()->getCamera()->setPosition(cameraCountDownPosition);

    ComponentsManager::get()->getComponentCamera()->setAutoScroll(false);
    ComponentsManager::get()->getComponentCamera()->setAutoScrollSpeed(Vertex3D(0, -0.0, 0));

    ComponentsManager::get()->getComponentCamera()->setFreeLook(FREELOOK);
    ComponentsManager::get()->getComponentInput()->setEnabled(FREELOOK);
    ComponentsManager::get()->getComponentMenu()->setEnabled(false);

    loadPlayer();
    loadWeapons();
    loadLevels();

    shaderClouds = new ShaderClouds(Color::black());
    shaderClouds->setEnabled(false);

    shaderBackgroundImage = new ShaderImage();
    shaderBackgroundImage->setEnabled(true);
    shaderBackgroundImage->setUseOffset(true);

    shaderColor = new ShaderColor(Color::red(), 0.75);
    shaderColor->setEnabled(false);

    shaderTrailBuffer = new ShaderTrailBuffer();
    shaderTrailBuffer->setEnabled(false);
    shaderTrailBuffer->clearStencilBuffer();

    shaderLasers = new ShaderProjectiles();
    shaderLasers->setEnabled(true);
}

ComponentGame::~ComponentGame()
{
    delete player;

    delete fadeToGameState;
    delete levelInfo;

    delete shaderLasers;
    delete shaderBackgroundImage;
    delete shaderClouds;
    delete shaderColor;
    delete shaderTrailBuffer,

    delete explosionSpriteTemplate;
    delete imageCrossFire;
    delete imageCredits;
    delete imageHelp;
    delete imageSplash;

    for (auto weapon: weapons) {
        delete weapon;
    }
}

void ComponentGame::preUpdate()
{
    EngineSetup::GameState state = getGameState();


    if (state == EngineSetup::GameState::SPLASH) {
        splashCounter.update();
        if (splashCounter.isFinished() && splashCounter.isEnabled()) {
            splashCounter.setEnabled(false);
            makeFadeToGameState(EngineSetup::GameState::MENU);
        }

        imageSplash->drawFlatAlpha(0, 0, 255 - getFadeToGameState()->getProgress() * 255);
    }

    if (
        state == EngineSetup::GameState::GAMING ||
        state == EngineSetup::GameState::PRESS_KEY_GAMEOVER ||
        state == EngineSetup::GameState::COUNTDOWN ||
        state == EngineSetup::GameState::PRESS_KEY_BY_DEAD ||
        state == EngineSetup::GameState::PRESS_KEY_NEWLEVEL ||
        state == EngineSetup::GameState::PRESS_KEY_BY_WIN ||
        state == EngineSetup::GameState::PRESS_KEY_PREVIOUS_LEVEL
    ) {
        shaderBackgroundImage->update();
    }

    getPlayer()->updateWeaponInteractionStatus();
}

void ComponentGame::onUpdate()
{
    EngineSetup::GameState state = getGameState();

    switch(state) {
        case EngineSetup::PRESS_KEY_NEWLEVEL:
        case EngineSetup::PRESS_KEY_PREVIOUS_LEVEL: {
            textWriter->writeTTFCenterHorizontal(50, "press a key to START...", primaryColor, 0.5);
            if (getLevelInfo()->isHasTutorial()) {
                getLevelInfo()->getTutorialImage()->drawFlat(EngineSetup::get()->screenWidth/2-(getLevelInfo()->getTutorialImage()->width()/2), 40);
            }
            break;
        }
        case EngineSetup::PRESS_KEY_BY_DEAD: {
            textWriter->writeTextTTFMiddleScreen("you are die...", Color::black(), 0.5f);
            shaderColor->setProgress((1 - getFadeToGameState()->getProgress()) * 0.50f);
            break;
        }
        case EngineSetup::COUNTDOWN: {
            zoomCameraCountDown();
            int restTime = (int) (getLevelInfo()->getCountDown()->getStep() - getLevelInfo()->getCountDown()->getAcumulatedTime() + 1);
            textWriter->writeTextTTFMiddleScreen(std::to_string(restTime).c_str(), primaryColor, 1);
            getLevelInfo()->getCountDown()->update();
            if (getLevelInfo()->getCountDown()->isFinished()) {
                setGameState(EngineSetup::GameState::GAMING);
            }
            break;
        }
        case EngineSetup::GAMING: {
            blockPlayerPositionInCamera();
            checkForEndLevel();
            break;
        }
        case EngineSetup::HELP:
        case EngineSetup::CREDITS: {
            imageHelp->drawFlatAlpha(0, 0, 255 - getFadeToGameState()->getProgress() * 255);
            break;
        }
        case EngineSetup::PRESS_KEY_GAMEOVER: {
            textWriter->writeTextTTFMiddleScreen("congratulations! END GAME...", primaryColor, 0.5);
            break;
        }
        case EngineSetup::PRESS_KEY_BY_WIN: {
            textWriter->writeTextTTFMiddleScreen("congratulations! Mission done...", primaryColor, 0.5);
            break;
        }
        case EngineSetup::NONE:
        case EngineSetup::SPLASH:
        case EngineSetup::MENU:
        break;
    }

    updateFadeToGameState();
    addObjectsToStencilBuffer();
    addProjectilesToShaderLasers();
    updateShaders();
    updateCrossFire();

    textWriter->setAlpha(255 - getFadeToGameState()->getProgress() * 255);
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

void ComponentGame::updateCrossFire() {
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
    if (getLiveEnemiesCounter() == 0 && !getLevelInfo()->isLevelFinished()) {
        getPlayer()->setKillsCounter(0);
        getLevelInfo()->setLevelFinished(true);
        getLevelInfo()->setLevelStartedToPlay(false);
        removeProjectiles();
        getFadeToGameState()->setSpeed(FADE_SPEED_ENDLEVEL);
        ComponentSound::playSound(
            ComponentsManager::get()->getComponentSound()->getSoundPackage().getByLabel("levelCompleted"),
            EngineSetup::SoundChannels::SND_GLOBAL,
            0
        );

        if (getLevelInfo()->isEndLevel()) {
            makeFadeToGameState(EngineSetup::PRESS_KEY_GAMEOVER);
        } else {
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
        case EngineSetup::PRESS_KEY_NEWLEVEL:
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
            handlePressKeyCredits();
            break;
        case EngineSetup::PRESS_KEY_GAMEOVER:
            handlePressKeyGameOver();
            break;
        case EngineSetup::CREDITS:
            break;
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

    // load in this point because alpha is not working if is load previous (todo)
    player->loadShieldModel();
    player->loadBlinkShader();
    player->loadReflection();

    explosionSpriteTemplate = new Sprite3D(EngineSetup::get()->BILLBOARD_WIDTH_DEFAULT, EngineSetup::get()->BILLBOARD_HEIGHT_DEFAULT);
    explosionSpriteTemplate->addAnimation(std::string(EngineSetup::get()->SPRITES_FOLDER + "explosion/explosion"), 12, 30);
    explosionSpriteTemplate->setAnimation(0);
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
        for (auto & vertice : mesh->aabb.vertices) {
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
        ComponentSound::playSound(
            ComponentsManager::get()->getComponentSound()->getSoundPackage().getByLabel("tic"),
            EngineSetup::SoundChannels::SND_GLOBAL,
            0
        );
        ComponentsManager::get()->getComponentRender()->setSelectedObject(currentClosestObject);
    }
}

void ComponentGame::loadLevels()
{
    levelInfo = new LevelLoader(EngineSetup::get()->CONFIG_FOLDER + "level01.json");
    levelInfo->addLevel(EngineSetup::get()->CONFIG_FOLDER + "level02.json");
    levelInfo->addLevel(EngineSetup::get()->CONFIG_FOLDER + "level03.json");
    levelInfo->addLevel(EngineSetup::get()->CONFIG_FOLDER + "level04.json");
    levelInfo->addLevel(EngineSetup::get()->CONFIG_FOLDER + "level05.json");
    levelInfo->addLevel(EngineSetup::get()->CONFIG_FOLDER + "level06.json");
    levelInfo->addLevel(EngineSetup::get()->CONFIG_FOLDER + "level07.json");
    levelInfo->addLevel(EngineSetup::get()->CONFIG_FOLDER + "level08.json");
    levelInfo->addLevel(EngineSetup::get()->CONFIG_FOLDER + "level09.json");
    levelInfo->addLevel(EngineSetup::get()->CONFIG_FOLDER + "level10.json");
    levelInfo->addLevel(EngineSetup::get()->CONFIG_FOLDER + "level11.json");
    levelInfo->addLevel(EngineSetup::get()->CONFIG_FOLDER + "level12.json");
    levelInfo->addLevel(EngineSetup::get()->CONFIG_FOLDER + "level13.json");
    levelInfo->addLevel(EngineSetup::get()->CONFIG_FOLDER + "level14.json");
    levelInfo->addLevel(EngineSetup::get()->CONFIG_FOLDER + "level15.json");
    levelInfo->addLevel(EngineSetup::get()->CONFIG_FOLDER + "level16.json");
    levelInfo->addLevel(EngineSetup::get()->CONFIG_FOLDER + "level17.json");
    levelInfo->addLevel(EngineSetup::get()->CONFIG_FOLDER + "level18.json");
    levelInfo->addLevel(EngineSetup::get()->CONFIG_FOLDER + "level19.json");
    levelInfo->addLevel(EngineSetup::get()->CONFIG_FOLDER + "level20.json");
    levelInfo->addLevel(EngineSetup::get()->CONFIG_FOLDER + "level30.json");
}


FaderToGameStates *ComponentGame::getFadeToGameState() const
{
    return fadeToGameState;
}

LevelLoader *ComponentGame::getLevelInfo() const
{
    return levelInfo;
}

void ComponentGame::removeProjectiles() const
{
    auto objects = Brakeza3D::get()->getSceneObjects();
    for (auto object : objects) {
        auto projectile = dynamic_cast<AmmoProjectileBody *> (object);
        if (projectile != nullptr) {
            projectile->remove();
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

void ComponentGame::makeFadeToGameState(EngineSetup::GameState value) const
{
    ComponentsManager::get()->getComponentGameInput()->setEnabled(false);

    getFadeToGameState()->resetTo(value);
}

void ComponentGame::removeInGameObjects()
{
    for (auto object : Brakeza3D::get()->getSceneObjects()) {
        auto *enemy = dynamic_cast<EnemyGhost *> (object);
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
        if (health != nullptr) {
            health->remove();
            continue;
        }
        if (weapon != nullptr) {
            weapon->remove();
            continue;
        }
        if (projectile != nullptr) {
            projectile->remove();
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

void ComponentGame::setVisibleInGameObjects(bool value)
{
    for (auto object : Brakeza3D::get()->getSceneObjects()) {
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
            particleEmitter->setRemoved(true);
        }
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
        auto weapon = getLevelInfo()->parseWeaponJSON(currentWeapon);
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
    if (!getLevelInfo()->isLevelStartedToPlay()) {
        getFadeToGameState()->setSpeed(FADE_SPEED_START_GAME);
        makeFadeToGameState(EngineSetup::GameState::PRESS_KEY_NEWLEVEL);
        ComponentSound::playSound(
            ComponentsManager::get()->getComponentSound()->getSoundPackage().getByLabel("startGame"),
            EngineSetup::SoundChannels::SND_GLOBAL,
            0
        );

        player->respawn();
    } else {
        makeFadeToGameState(EngineSetup::GameState::GAMING);
    }
}

void ComponentGame::pressedKeyForWin()
{
    makeFadeToGameState(EngineSetup::PRESS_KEY_NEWLEVEL);
}

void ComponentGame::pressedKeyForBeginLevel()
{
    ComponentSound::playSound(
        ComponentsManager::get()->getComponentSound()->getSoundPackage().getByLabel("newLevel"),
        EngineSetup::SoundChannels::SND_GLOBAL,
        0
    );
    getLevelInfo()->startCountDown();
    setGameState(EngineSetup::GameState::COUNTDOWN);
    getPlayer()->startPlayerBlink();
}

void ComponentGame::pressedKeyForFinishGameAndRestart()
{
    ComponentSound::fadeInMusic(ComponentsManager::get()->getComponentSound()->getSoundPackage().getMusicByLabel("musicMainMenu"), -1, 3000);
    getLevelInfo()->setCurrentLevelIndex(-1);
    makeFadeToGameState(EngineSetup::GameState::MENU);
}

void ComponentGame::pressedKeyByDead()
{
    player->respawn();

    ComponentSound::playSound(
        ComponentsManager::get()->getComponentSound()->getSoundPackage().getByLabel("startGame"),
        EngineSetup::SoundChannels::SND_GLOBAL,
        0
    );
    makeFadeToGameState(EngineSetup::GameState::PRESS_KEY_PREVIOUS_LEVEL);
    getPlayer()->startPlayerBlink();
}

void ComponentGame::addObjectsToStencilBuffer()
{
    if (getGameState() != EngineSetup::GAMING) return;

    for (auto object : Brakeza3D::get()->getSceneObjects()) {
        auto projectile = dynamic_cast<AmmoProjectileBody *> (object);
        auto reflection = dynamic_cast<PlayerReflection *> (object);
        auto energy = dynamic_cast<ItemEnergyGhost *> (object);
        auto health = dynamic_cast<ItemHealthGhost *> (object);
        auto weapon = dynamic_cast<ItemWeaponGhost *> (object);
        auto bomb = dynamic_cast<ItemBombGhost *> (object);

        if (
            projectile != nullptr ||
            reflection != nullptr ||
            bomb != nullptr ||
            health != nullptr ||
            weapon != nullptr ||
            energy != nullptr
        ) {
            this->shaderTrailBuffer->addStencilBufferObject(object);
        }
    }
}

void ComponentGame::updateShaders()
{
    shaderTrailBuffer->update();
    shaderClouds->update();
    shaderColor->update();
    shaderLasers->update();
}

void ComponentGame::zoomCameraCountDown()
{
    Vertex3D origin = ComponentsManager::get()->getComponentCamera()->getCamera()->getPosition();
    Vertex3D to = cameraInGamePosition;

    Vector3D direction(origin, to);
    auto counter = getLevelInfo()->getCountDown();

    float t = counter->getAcumulatedTime() / counter->getStep();

    ComponentsManager::get()->getComponentCamera()->getCamera()->setPosition(origin + direction.getComponent().getScaled(t));
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

    getFadeToGameState()->setSpeed(FADE_SPEED_FROM_MENU_TO_GAMING);

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
    getPlayer()->setEnabled(true);
    getPlayer()->getShaderLaser().setEnabled(false);
    ComponentsManager::get()->getComponentCamera()->getCamera()->setPosition(cameraCountDownPosition);

    shaderTrailBuffer->setEnabled(true);
    shaderBackgroundImage->resetOffsets();

    getLevelInfo()->loadNext();
    getPlayer()->getWeapon()->setStatus(WeaponStatus::RELEASED);
    getPlayer()->setEnergyShieldEnabled(false);
    getPlayer()->setGravityShieldsNumber(0);
    getPlayer()->setPosition(playerStartPosition);
    setVisibleInGameObjects(true);
    ComponentsManager::get()->getComponentHUD()->setEnabled(false);
    ComponentsManager::get()->getComponentMenu()->setEnabled(false);
    ComponentsManager::get()->getComponentRender()->setEnabled(true);
    getFadeToGameState()->setSpeed(FADE_SPEED_PRESSKEY_NEWLEVEL);
    if (getLevelInfo()->isHaveMusic()) {
        ComponentSound::fadeInMusic(ComponentsManager::get()->getComponentSound()->getSoundPackage().getMusicByLabel(getLevelInfo()->getMusic()), -1, 3000);
    }
}

void ComponentGame::reloadLevel(int level)
{
    removeInGameObjects();
    getPlayer()->respawn();
    getPlayer()->setEnabled(true);

    ComponentsManager::get()->getComponentCamera()->getCamera()->setPosition(cameraInGamePosition);

    shaderBackgroundImage->resetOffsets();

    getLevelInfo()->load(level);

    getPlayer()->getWeapon()->setStatus(WeaponStatus::RELEASED);
    getPlayer()->setEnergyShieldEnabled(false);
    getPlayer()->setGravityShieldsNumber(0);
    getPlayer()->setPosition(playerStartPosition);
    setVisibleInGameObjects(true);

    ComponentsManager::get()->getComponentHUD()->setEnabled(true);
    ComponentsManager::get()->getComponentMenu()->setEnabled(false);
    ComponentsManager::get()->getComponentRender()->setEnabled(true);

    getLevelInfo()->startCountDown();
    setGameState(EngineSetup::GameState::COUNTDOWN);
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
    getLevelInfo()->loadPrevious();
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
    getFadeToGameState()->setSpeed(FADE_SPEED_PRESSKEY_NEWLEVEL);
    ComponentSound::fadeInMusic(ComponentsManager::get()->getComponentSound()->getSoundPackage().getMusicByLabel(getLevelInfo()->getMusic()), -1, 3000);
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
    ComponentsManager::get()->getComponentMenu()->setEnabled(false);
}

const Color &ComponentGame::getPrimaryColor() const {
    return primaryColor;
}

Sprite3D *ComponentGame::getExplosionSpriteTemplate() const {
    return explosionSpriteTemplate;
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

ShaderClouds *ComponentGame::getShaderClouds() const {
    return shaderClouds;
}

void ComponentGame::handleSplash()
{
    splashCounter.setEnabled(true);
    shaderClouds->setEnabled(false);

    ComponentSound::fadeInMusic(
        ComponentsManager::get()->getComponentSound()->getSoundPackage().getMusicByLabel("musicMainMenu"),
        -1,
        SPLASH_TIME * 1000
    );
}

void ComponentGame::addProjectilesToShaderLasers()
{
    if (getGameState() != EngineSetup::GAMING && getGameState() != EngineSetup::COUNTDOWN) return;

    for (auto object : Brakeza3D::get()->getSceneObjects()) {
        if (object->isRemoved()) continue;

        auto projectile = dynamic_cast<AmmoProjectileBody *> (object);
        auto ray = dynamic_cast<ProjectileRay *> (object);

        if (projectile != nullptr) {
            shaderLasers->addProjectile(
                object->getPosition(),
                projectile->getColor(),
                (float) projectile->getWeaponType()->getSpeed()
            );
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
