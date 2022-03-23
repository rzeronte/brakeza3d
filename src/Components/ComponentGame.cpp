#include "../../include/Components/ComponentGame.h"
#include "../../include/Components/ComponentCollisions.h"
#include "../../include/Brakeza3D.h"
#include "../../include/Render/Transforms.h"

#define FREELOOK false
#define SPLASH_TIME 3.0f
#define FADE_TIME 0.4f

ComponentGame::ComponentGame()
{
    player = new Player();
    axisPlanes = new Mesh3DBody();
}

void ComponentGame::onStart()
{
    Logging::Log("ComponentGame onStart", "ComponentGame");
    setGameState(EngineSetup::GameState::NONE);
    fadeToGameState = new ShaderFadeBetweenGameStates(
            Color(0, 255, 0),
        0.5f,
        EngineSetup::GameState::SPLASH,
        false
    );

    shaderAutoScrollSpeed = Vertex3D(0, 0.4, 0);
    imageHelp = new Image(SETUP->IMAGES_FOLDER + SETUP->DEFAULT_HELP_IMAGE);
    imageSplash = new Image(SETUP->IMAGES_FOLDER + SETUP->LOGO_BRAKEZA);
    splashCounter.setStep(SPLASH_TIME);

    ComponentsManager::get()->getComponentCollisions()->initBulletSystem();

    ComponentsManager::get()->getComponentCamera()->getCamera()->setPosition(Vertex3D(0, -1000, -1000));

    ComponentsManager::get()->getComponentCamera()->setAutoScroll(true);
    ComponentsManager::get()->getComponentCamera()->setAutoScrollSpeed(Vertex3D(0, -5.0, 0));

    ComponentsManager::get()->getComponentCamera()->setFreeLook(FREELOOK);
    ComponentsManager::get()->getComponentInput()->setEnabled(FREELOOK);

    loadBackgroundImageShader();
    loadPlayerWeapons();
    loadPlayer();
    loadLevels();
}

void ComponentGame::preUpdate() {
    if (getGameState() == EngineSetup::GameState::SPLASH) {
        splashCounter.update();
        if (splashCounter.isFinished() && splashCounter.isEnabled()) {
            splashCounter.setEnabled(false);
            fadeToGameState->resetTo(EngineSetup::GameState::MENU);
        }

        imageSplash->drawFlat(0, 0);
    }
}

void ComponentGame::onUpdate()
{
    EngineSetup::GameState state = getGameState();

    if (state == EngineSetup::GameState::GAMING) {
        onUpdateIA();
        blockPlayerPositionInCamera();
        checkForEndLevel();
    }

    if (state == EngineSetup::GameState::ENDGAME) {
        ComponentsManager::get()->getComponentHUD()->writeTextMiddleScreen("HAS TERMINAO GAYU", false);
    }

    if (state == EngineSetup::GameState::PRESSKEY) {
        ComponentsManager::get()->getComponentHUD()->writeTextMiddleScreen("PRESIONA UNA TECLA GAYU", false);
    }

    if (state == EngineSetup::GameState::HELP) {
        imageHelp->drawFlat(0, 0);
    }

    getFadeToGameState()->onUpdate();
    if (getFadeToGameState()->isEndFadeOut()) {
        getFadeToGameState()->setEndFadeOut(false);
        setGameState(getFadeToGameState()->getGameStateWhenEnds());
    }
}

void ComponentGame::checkForEndLevel() const
{
    if (getPlayer()->getKillsCounter() >= getLevelInfo()->getNumberLevelEnemies()) {
        getPlayer()->increaseLevelsCompleted();
        getPlayer()->setKillsCounter(0);
        getLevelInfo()->setLevelStartedToPlay(false);
        if (getPlayer()->getLevelCompletedCounter() >= getLevelInfo()->getNumberLevel()) {
            getFadeToGameState()->resetTo(EngineSetup::ENDGAME);
        } else {
            getFadeToGameState()->resetTo(EngineSetup::PRESSKEY);
        }
    }
}

void ComponentGame::setGameState(EngineSetup::GameState state) {
    Logging::getInstance()->Log("GameState changed to: " + std::to_string(state));

    if (getGameState() == EngineSetup::GameState::NONE && state == EngineSetup::GameState::SPLASH) {
        Logging::getInstance()->Log("GameState changed to SPLASH");

        splashCounter.setEnabled(true);
        Mix_PlayMusic(BUFFERS->soundPackage->getMusicByLabel("musicMainMenu"), -1);
    }

    if (getGameState() == EngineSetup::GameState::SPLASH && state == EngineSetup::GameState::MENU) {
        getFadeToGameState()->setSpeed(FADE_TIME);
        Logging::getInstance()->Log("GameState changed to MENU");
    }

    if (state == EngineSetup::GameState::MENU) {
        ComponentsManager::get()->getComponentMenu()->setEnabled(true);
    }

    if (state == EngineSetup::GameState::GAMING) {
        ComponentsManager::get()->getComponentCamera()->setAutoScroll(true);
        startBackgroundShader();
        ComponentsManager::get()->getComponentHUD()->setEnabled(true);
        ComponentsManager::get()->getComponentMenu()->setEnabled(false);
        ComponentsManager::get()->getComponentRender()->setEnabled(true);
        ComponentsManager::get()->getComponentCollisions()->setEnabled(true);
    } else {
        ComponentsManager::get()->getComponentCamera()->setAutoScroll(false);
        stopBackgroundShader();
        ComponentsManager::get()->getComponentHUD()->setEnabled(false);
        ComponentsManager::get()->getComponentRender()->setEnabled(false);
        ComponentsManager::get()->getComponentCollisions()->setEnabled(false);
    }

    if (state == EngineSetup::GameState::PRESSKEY) {
        ComponentsManager::get()->getComponentHUD()->setEnabled(true);
        ComponentsManager::get()->getComponentMenu()->setEnabled(false);
        ComponentsManager::get()->getComponentRender()->setEnabled(true);
        startBackgroundShader();
    }

    this->gameState = state;
}

void ComponentGame::postUpdate() {
    player->evalStatusMachine();
}

void ComponentGame::onEnd() {
}

void ComponentGame::onSDLPollEvent(SDL_Event *event, bool &finish) {
}

Player *ComponentGame::getPlayer() const {
    return player;
}

void ComponentGame::onUpdateIA() const {
    for (auto object : Brakeza3D::get()->getSceneObjects()) {
        auto *enemy = dynamic_cast<EnemyGhost *> (object);

        if (enemy != nullptr) {
            evalStatusMachine(enemy);
        }
    }
}

void ComponentGame::blockPlayerPositionInCamera() {
    auto *camera = ComponentsManager::get()->getComponentCamera()->getCamera();

    Vertex3D homogeneousPosition;
    Vertex3D destinyPoint = player->getPosition() + player->getVelocity();
    Transforms::cameraSpace(homogeneousPosition, destinyPoint, camera);
    homogeneousPosition = Transforms::PerspectiveNDCSpace(homogeneousPosition, camera->frustum);

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
    player->setEnableLights(false);
    player->setPosition(Vertex3D(1115, -700, Z_COORDINATE_GAMEPLAY));
    player->setScale(1);
    player->setStamina(100);
    player->setStencilBufferEnabled(true);
    player->setAutoRotationSelectedObjectSpeed(5);
    player->AssimpLoadGeometryFromFile(std::string(EngineSetup::get()->MODELS_FOLDER + "spaceship03.fbx"));
    player->makeGhostBody(ComponentsManager::get()->getComponentCollisions()->getDynamicsWorld(), player);
    Brakeza3D::get()->addObject3D(player, "player");

    auto *sprite = new Sprite3D();
    sprite->setEnabled(true);
    sprite->setPosition(Vertex3D(0, -1000, 200));
    sprite->setScale(500);
    sprite->addAnimation(EngineSetup::get()->SPRITES_FOLDER + "blood1/blood", 3, 1);
    sprite->setAnimation(0);
    Brakeza3D::get()->addObject3D(sprite, "sprite");
}

void ComponentGame::loadPlayerWeapons() {

    player->createWeaponType("defaultWeapon");
    WeaponType *weaponType = player->getWeaponTypeByLabel("defaultWeapon");
    weaponType->getModelProjectile()->AssimpLoadGeometryFromFile(std::string(EngineSetup::get()->MODELS_FOLDER + "basic/icosphere.fbx"));
    weaponType->getModelProjectile()->setLabel("projectile_template");
    weaponType->setAmmoAmount(999);
    weaponType->setSpeed(700);
    weaponType->setDamage(10);
    weaponType->setDispersion(10);
    weaponType->setAvailable(true);
    weaponType->setAccuracy(100);
    weaponType->setCadenceTime(0.05);
    weaponType->setIconImage(SETUP->HUD_FOLDER + "flare.png");

    player->createWeaponType("secondaryWeapon");
    WeaponType *weaponSecondaryType = player->getWeaponTypeByLabel("secondaryWeapon");
    weaponSecondaryType->getModelProjectile()->AssimpLoadGeometryFromFile(std::string(EngineSetup::get()->MODELS_FOLDER + "basic/cone.fbx"));
    weaponSecondaryType->getModelProjectile()->setLabel("projectile_template");
    weaponSecondaryType->setSpeed(500);
    weaponSecondaryType->setDamage(10);
    weaponSecondaryType->setAmmoAmount(15);
    weaponSecondaryType->setDispersion(10);
    weaponSecondaryType->setAvailable(true);
    weaponSecondaryType->setAccuracy(100);
    weaponSecondaryType->setCadenceTime(0.15);
    weaponSecondaryType->setIconImage(SETUP->HUD_FOLDER + "plague.png");

    player->setWeaponType(weaponType);
}


void ComponentGame::selectClosestObject3DFromPlayer() {
    Object3D *currentClosestObject = nullptr;
    Object3D *currentSelectedObject = ComponentsManager::get()->getComponentRender()->getSelectedObject();

    float currentMinDistance = 0;

    for (auto object : Brakeza3D::get()->getSceneObjects()) {
        if (
            !object->isEnabled() ||
            player == object ||
            currentSelectedObject == object
        ) {
            continue;
        }

        auto mesh = dynamic_cast<EnemyGhost *>(object);
        if (mesh == nullptr) {
            continue;
        }

        mesh->updateBoundingBox();

        for (auto & vertice : mesh->aabb.vertices) {
            Vector3D v(player->getPosition(), vertice);

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

    if (currentClosestObject != nullptr) {
        ComponentsManager::get()->getComponentRender()->setSelectedObject(currentClosestObject);
        ComponentsManager::get()->getComponentRender()->updateSelectedObject3DInShaders(currentClosestObject);
    }
}

void ComponentGame::evalStatusMachine(EnemyGhost *enemy) const
{
    if (enemy->getState() == EnemyState::ENEMY_STATE_DIE) {
        return;
    }

    enemy->shoot(ComponentsManager::get()->getComponentGame()->getPlayer());
}

void ComponentGame::loadLevels()
{
    levelInfo = new LevelsLoader(EngineSetup::get()->IMAGES_FOLDER + "level01.png");
    levelInfo->addLevel(EngineSetup::get()->IMAGES_FOLDER + "level02.png");

    /*
    auto *weaponItem = new ItemWeaponGhost(enemyWeaponType);
    weaponItem->setLabel("weaponItem");
    weaponItem->setEnableLights(false);
    weaponItem->setPosition(Vertex3D(-3515, -3200, 5000));
    weaponItem->setRotation(0, 0, 180);
    weaponItem->setStencilBufferEnabled(true);
    weaponItem->setScale(1);
    weaponItem->AssimpLoadGeometryFromFile(std::string(EngineSetup::get()->MODELS_FOLDER + "basic/cube.fbx"));
    weaponItem->makeGhostBody(ComponentsManager::get()->getComponentCollisions()->getDynamicsWorld(), weaponItem);
    Brakeza3D::get()->addObject3D(weaponItem, "weaponItem");

    auto *weaponItemTwo = new ItemWeaponGhost(enemyWeaponType);
    weaponItemTwo->setLabel("weaponItemTwo");
    weaponItemTwo->setEnableLights(false);
    weaponItemTwo->setPosition(Vertex3D(-2515, -3200, 5000));
    weaponItemTwo->setRotation(0, 0, 180);
    weaponItemTwo->setStencilBufferEnabled(true);
    weaponItemTwo->setScale(1);
    weaponItemTwo->AssimpLoadGeometryFromFile(std::string(EngineSetup::get()->MODELS_FOLDER + "basic/cube.fbx"));
    weaponItemTwo->makeGhostBody(ComponentsManager::get()->getComponentCollisions()->getDynamicsWorld(), weaponItemTwo);
    Brakeza3D::get()->addObject3D(weaponItemTwo, "weaponItemTwo");
    */
}

void ComponentGame::loadBackgroundImageShader()
{
    auto shaderBackground = dynamic_cast<ShaderImageBackground*> (ComponentsManager::get()->getComponentRender()->getShaderByType(EngineSetup::ShaderTypes::BACKGROUND));
    shaderBackground->setEnabled(true);
    shaderBackground->setType(ShaderImageBackgroundTypes::PORTION);
    stopBackgroundShader();
}

void ComponentGame::stopBackgroundShader()
{
    auto shaderBackground = dynamic_cast<ShaderImageBackground*> (ComponentsManager::get()->getComponentRender()->getShaderByType(EngineSetup::ShaderTypes::BACKGROUND));
    shaderBackground->setAutoScrollSpeed(Vertex3D(0, 0, 0));
    shaderBackground->setAutoScrollEnabled(false);
    shaderBackground->setEnabled(false);
}

void ComponentGame::startBackgroundShader()
{
    auto shaderBackground = dynamic_cast<ShaderImageBackground*> (ComponentsManager::get()->getComponentRender()->getShaderByType(EngineSetup::ShaderTypes::BACKGROUND));
    shaderBackground->setAutoScrollSpeed(this->shaderAutoScrollSpeed);
    shaderBackground->setAutoScrollEnabled(true);
    shaderBackground->setEnabled(true);
}

ShaderFadeBetweenGameStates *ComponentGame::getFadeToGameState() const {
    return fadeToGameState;
}

void ComponentGame::startWaterShader() {
    auto shaderFire = dynamic_cast<ShaderWater*> (ComponentsManager::get()->getComponentRender()->getShaderByType(EngineSetup::ShaderTypes::WATER));
    shaderFire->setPhaseRender(EngineSetup::ShadersPhaseRender::POSTUPDATE);
    shaderFire->setEnabled(true);
}

void ComponentGame::stopWaterShader() {
    auto shaderFire = dynamic_cast<ShaderWater*> (ComponentsManager::get()->getComponentRender()->getShaderByType(EngineSetup::ShaderTypes::WATER));
    shaderFire->setEnabled(false);
}

LevelsLoader *ComponentGame::getLevelInfo() const {
    return levelInfo;
}
