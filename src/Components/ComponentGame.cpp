#include "../../include/Components/ComponentGame.h"
#include "../../include/Components/ComponentCollisions.h"
#include "../../include/Brakeza3D.h"
#include "../../include/Render/Transforms.h"

ComponentGame::ComponentGame() {
    player = new Player();
    axisPlanes = new Mesh3DBody();
}

void ComponentGame::onStart() {
    Logging::Log("ComponentGame onStart", "ComponentGame");
    setGameState(GameState::MENU);

    Mix_PlayMusic(BUFFERS->soundPackage->getMusicByLabel("musicMainMenu"), -1);

    ComponentsManager::get()->getComponentCollisions()->initBulletSystem();

    ComponentsManager::get()->getComponentCamera()->getCamera()->setPosition(Vertex3D(0, -1000,0));

    ComponentsManager::get()->getComponentCamera()->setAutoScroll(true);
    ComponentsManager::get()->getComponentCamera()->setAutoScrollSpeed(Vertex3D(0, -2.0, 0));

    ComponentsManager::get()->getComponentCamera()->setFreeLook(false);
    ComponentsManager::get()->getComponentInput()->setEnabled(false);

    setupWeapons();
    loadPlayer();
}

void ComponentGame::preUpdate() {
}

void ComponentGame::onUpdate() {

    ComponentHUD *componentHUD = ComponentsManager::get()->getComponentHUD();

    if (player->state != PlayerState::GAMEOVER) {
        if (SETUP->ENABLE_IA) {
            onUpdateIA();
        }
    }

    GameState state = getGameState();

    if (state == GameState::LOADING) {
        componentHUD->writeTextMiddleScreen("Loading", false);
    }

    if (state == GameState::MENU) {
    }

    if (state == GameState::GAMING) {
        blockPlayerPositionInCamera();
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
    if (player->isDead()) return;

    std::vector<Object3D *>::iterator itObject3D;
    for (itObject3D = Brakeza3D::get()->getSceneObjects().begin();
        itObject3D != Brakeza3D::get()->getSceneObjects().end(); itObject3D++) {
        Object3D *object = *(itObject3D);

        auto *enemy = dynamic_cast<NPCEnemyBody *> (object);

        if (enemy != nullptr) {
            enemy->updateCounters();
        }
    }
}

void ComponentGame::blockPlayerPositionInCamera() {
    auto *camera = ComponentsManager::get()->getComponentCamera()->getCamera();

    Vertex3D homogeneousPosition;
    Vertex3D destinyPoint = player->getPosition() + player->getVelocity();
    Transforms::cameraSpace(homogeneousPosition, destinyPoint, camera);
    homogeneousPosition = Transforms::PerspectiveNDCSpace(homogeneousPosition, camera->frustum);

    // Down (if player collision bottom near camera plane, update his velocity with autoScroll speed
    if (homogeneousPosition.y > 1) {
        player->setPosition(player->getPosition()  );
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
    player->setPosition(Vertex3D(1115, -700, 4500));
    player->setScale(1);
    player->setStencilBufferEnabled(true);
    player->AssimpLoadGeometryFromFile(std::string(EngineSetup::get()->MODELS_FOLDER + "spaceship03.fbx"));
    player->makeGhostBody(ComponentsManager::get()->getComponentCollisions()->getDynamicsWorld(), player);
    Brakeza3D::get()->addObject3D(player, "player");

    auto * enemyOne = new Mesh3DGhost();
    enemyOne->setLabel("enemyOne");
    enemyOne->setEnableLights(false);
    enemyOne->setPosition(Vertex3D(1515, -3200, 5000));
    enemyOne->setRotation(90, -90, 0);
    enemyOne->setStencilBufferEnabled(true);
    enemyOne->setScale(1);
    enemyOne->AssimpLoadGeometryFromFile(std::string(EngineSetup::get()->MODELS_FOLDER + "spaceship_enemy_01.fbx"));
    enemyOne->makeGhostBody(ComponentsManager::get()->getComponentCollisions()->getDynamicsWorld(), enemyOne);
    Brakeza3D::get()->addObject3D(enemyOne, "enemyOne");

    /*auto * enemyTwo = new Mesh3DGhost();
    enemyTwo->setStencilBufferEnabled(true);
    enemyTwo->setLabel("enemyTwo");
    enemyTwo->setEnableLights(false);
    enemyTwo->setPosition(Vertex3D(-1115, -3200, 5000));
    enemyTwo->setRotation(90, -90, 0);
    enemyTwo->setScale(1);
    enemyTwo->AssimpLoadGeometryFromFile(std::string(EngineSetup::get()->MODELS_FOLDER + "spaceship_enemy_01.fbx"));
    enemyTwo->makeGhostBody(ComponentsManager::get()->getComponentCollisions()->getDynamicsWorld(), enemyOne);
    Brakeza3D::get()->addObject3D(enemyTwo, "enemyTwo");

    auto * enemyThree = new Mesh3DGhost();
    enemyThree->setStencilBufferEnabled(true);
    enemyThree->setLabel("enemyTwo");
    enemyThree->setEnableLights(false);
    enemyThree->setPosition(Vertex3D(-4115, -3200, 5000));
    enemyThree->setRotation(90, -90, 0);
    enemyThree->setScale(1);
    enemyThree->AssimpLoadGeometryFromFile(std::string(EngineSetup::get()->MODELS_FOLDER + "spaceship_enemy_01.fbx"));
    enemyThree->makeGhostBody(ComponentsManager::get()->getComponentCollisions()->getDynamicsWorld(), enemyOne);
    Brakeza3D::get()->addObject3D(enemyThree, "enemyThree");*/

}

void ComponentGame::setupWeapons() {
    auto *cw = ComponentsManager::get()->getComponentWeapons();
    cw->addWeaponType("defaultWeapon");
    WeaponType *weaponType = cw->getWeaponTypeByLabel("defaultWeapon");
    weaponType->setSpeed(500);
    weaponType->setDispersion(10);
    weaponType->setAvailable(true);
    weaponType->setAccuracy(100);

    auto *ammoType = new AmmoType();
    ammoType->setName("defaultWeapon_ammoType");
    ammoType->getModelProjectile()->AssimpLoadGeometryFromFile(std::string(EngineSetup::get()->MODELS_FOLDER + "basic/cube.fbx"));
    ammoType->getModelProjectile()->setLabel("projectile_template");
    ammoType->getModelProjectile()->setScale(1);
    ammoType->setAmount(1000);
    weaponType->setAmmoType(ammoType);

    cw->setCurrentWeaponIndex(WeaponsTypes::DEFAULT);
}


void ComponentGame::selectClosestObject3DFromPlayer() {
    Object3D *currentClosestObject = nullptr;
    Object3D *currentSelectedObject = ComponentsManager::get()->getComponentRender()->getSelectedObject();

    float currentMinDistance = 0;

    for (auto object : Brakeza3D::get()->getSceneObjects()) {
        if (!object->isEnabled())  {
            continue;
        }

        if (player == object) {
            continue;
        }

        if (currentSelectedObject == object) {
            continue;
        }

        Mesh3D *mesh = dynamic_cast<Mesh3D *> (object);
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
        auto *shader = dynamic_cast<ShaderObjectSilhouette *>(ComponentsManager::get()->getComponentRender()->getShaderByType(EngineSetup::SILHOUETTE));
        shader->setObject(currentClosestObject);
        ComponentsManager::get()->getComponentRender()->setSelectedObject(currentClosestObject);
    }
}