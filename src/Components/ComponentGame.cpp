#include "../../include/Components/ComponentGame.h"
#include "../../include/Components/ComponentCollisions.h"
#include "../../include/Brakeza3D.h"
#include "../../include/Render/Transforms.h"

#define FREELOOK false

ComponentGame::ComponentGame() {
    player = new Player();
    axisPlanes = new Mesh3DBody();
}

void ComponentGame::onStart() {
    Logging::Log("ComponentGame onStart", "ComponentGame");
    setGameState(GameState::MENU);
    SETUP->DRAW_HUD = false;

    imageHelp = new Image(SETUP->IMAGES_FOLDER + SETUP->DEFAULT_HELP_IMAGE);

    Mix_PlayMusic(BUFFERS->soundPackage->getMusicByLabel("musicMainMenu"), -1);

    ComponentsManager::get()->getComponentCollisions()->initBulletSystem();

    ComponentsManager::get()->getComponentCamera()->getCamera()->setPosition(Vertex3D(0, -1000,0));

    ComponentsManager::get()->getComponentCamera()->setAutoScroll(false);
    ComponentsManager::get()->getComponentCamera()->setAutoScrollSpeed(Vertex3D(0, -1.0, 0));

    ComponentsManager::get()->getComponentCamera()->setFreeLook(FREELOOK);
    ComponentsManager::get()->getComponentInput()->setEnabled(FREELOOK);
    ComponentsManager::get()->getComponentRender()->getShaderByType(EngineSetup::ShaderTypes::BACKGROUND)->setEnabled(true);

    loadPlayerWeapons();
    loadPlayer();
    loadEnemy();
}

void ComponentGame::preUpdate() {
}

void ComponentGame::onUpdate() {

    ComponentHUD *componentHUD = ComponentsManager::get()->getComponentHUD();

    GameState state = getGameState();

    if (state == GameState::GAMING) {
        onUpdateIA();
    }

    if (state == GameState::LOADING) {
        componentHUD->writeTextMiddleScreen("Loading", false);
    }

    if (state == GameState::MENU) {
    }

    if (state == GameState::GAMING) {
        blockPlayerPositionInCamera();
    }

    if (state == GameState::HELP) {
        imageHelp->drawFlat(0, 0);
    }
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

void ComponentGame::setGameState(GameState state) {
    if (state == GameState::GAMING) {
        ComponentsManager::get()->getComponentCamera()->setAutoScroll(true);
    } else {
        ComponentsManager::get()->getComponentCamera()->setAutoScroll(false);
    }

    this->gameState = state;
}

GameState ComponentGame::getGameState() {
    return gameState;
}

void ComponentGame::loadPlayer()
{
    player->setLabel("player");
    player->setEnableLights(false);
    player->setPosition(Vertex3D(1115, -700, 5000));
    player->setScale(1);
    player->setStamina(100);
    player->setStencilBufferEnabled(true);
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
    auto *ammoType = new AmmoType();
    ammoType->setName("defaultWeapon_ammoType");
    ammoType->getModelProjectile()->AssimpLoadGeometryFromFile(std::string(EngineSetup::get()->MODELS_FOLDER + "basic/icosphere.fbx"));
    ammoType->getModelProjectile()->setLabel("projectile_template");
    ammoType->getModelProjectile()->setScale(1);
    ammoType->setAmount(1000);

    player->addWeaponType("defaultWeapon");
    WeaponType *weaponType = player->getWeaponTypeByLabel("defaultWeapon");
    weaponType->setSpeed(500);
    weaponType->setDamage(10);
    weaponType->setDispersion(10);
    weaponType->setAvailable(true);
    weaponType->setAccuracy(100);
    weaponType->setCadenceTime(0.25);
    weaponType->setAmmoType(ammoType);
    weaponType->setCadenceTime(0.15);
    weaponType->setIconImage(SETUP->HUD_FOLDER + "flare.png");

    player->addWeaponType("secondaryWeapon");
    WeaponType *weaponSecondaryType = player->getWeaponTypeByLabel("secondaryWeapon");
    weaponSecondaryType->setSpeed(700);
    weaponSecondaryType->setDamage(10);
    weaponSecondaryType->setDispersion(10);
    weaponSecondaryType->setAvailable(true);
    weaponSecondaryType->setAccuracy(100);
    weaponSecondaryType->setCadenceTime(0.25);
    weaponSecondaryType->setAmmoType(ammoType);
    weaponSecondaryType->setCadenceTime(0.05);
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

void ComponentGame::evalStatusMachine(EnemyGhost *enemy) const {
    if (enemy->getState() == EnemyState::ENEMY_STATE_DIE) {
        return;
    }

    enemy->shoot(ComponentsManager::get()->getComponentGame()->getPlayer());
}

void ComponentGame::loadEnemy() {

    auto *enemyOne = new EnemyGhost();
    enemyOne->setLabel("enemyOne");
    enemyOne->setEnableLights(false);
    enemyOne->setPosition(Vertex3D(1515, -3200, 5000));
    enemyOne->setRotation(0, 0, 180);
    enemyOne->setStencilBufferEnabled(true);
    enemyOne->setScale(1);
    enemyOne->AssimpLoadGeometryFromFile(std::string(EngineSetup::get()->MODELS_FOLDER + "spaceship03.fbx"));
    enemyOne->makeGhostBody(ComponentsManager::get()->getComponentCollisions()->getDynamicsWorld(), enemyOne);
    Brakeza3D::get()->addObject3D(enemyOne, "enemyOne");

    auto *ammoType = new AmmoType();
    ammoType->setName("enemy_ammoType");
    ammoType->getModelProjectile()->AssimpLoadGeometryFromFile(std::string(EngineSetup::get()->MODELS_FOLDER + "basic/icosphere.fbx"));
    ammoType->getModelProjectile()->setLabel("projectile_enemy_template");
    ammoType->getModelProjectile()->setScale(1);
    ammoType->setAmount(10);

    auto enemyWeaponType = new WeaponType("enemy_weaponDefault");
    enemyWeaponType->setSpeed(500);
    enemyWeaponType->setDamage(10);
    enemyWeaponType->setDispersion(10);
    enemyWeaponType->setAvailable(true);
    enemyWeaponType->setAccuracy(100);
    enemyWeaponType->setAmmoType(ammoType);
    enemyWeaponType->setCadenceTime(3);

    enemyOne->setWeaponType(enemyWeaponType);
}
