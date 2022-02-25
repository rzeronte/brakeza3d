#include "../../include/Components/ComponentGame.h"
#include "../../include/Components/ComponentCollisions.h"
#include "../../include/Brakeza3D.h"
#include "../../include/Render/Transforms.h"

ComponentGame::ComponentGame() {
    player = new Player();
}

void ComponentGame::onStart() {
    Logging::Log("ComponentGame onStart", "ComponentGame");
    setAutoScroll(true);
    setAutoScrollSpeed(Vertex3D(0, -2.0, 0));
    setGameState(GameState::MENU);

    Mix_PlayMusic(BUFFERS->soundPackage->getMusicByLabel("musicMainMenu"), -1);

    ComponentsManager::get()->getComponentCollisions()->initBulletSystem();

    ComponentsManager::get()->getComponentCamera()->getCamera()->setPosition(Vertex3D(0, -1000,0));

    ComponentsManager::get()->getComponentCamera()->setFreeLook(true);
    ComponentsManager::get()->getComponentInput()->setEnabled(true);

    setupWeapons();

    shaderImageBackground = ShaderImageBackground(std::string(SETUP->IMAGES_FOLDER + "deep_space.png").c_str());
    shaderImageBackground.setType(ShaderImageBackgroundTypes::PORTION);

    shaderTintScreen.setTintColorIntensity(1, 0, 0);

    loadPlayer();

    auto object = new Mesh3DBody();
    object->setRotation(0, 0, 0);
    object->setEnabled(true);
    object->setLabel("test");
    object->setCollisionsEnabled(true);
    object->setScale(1);
    object->setFlatTextureColor(false);
    object->setPosition(Vertex3D(15, -700, 2600));
    object->setRotationFrameEnabled(false);
    object->AssimpLoadGeometryFromFile(std::string(EngineSetup::get()->MODELS_FOLDER + "planet_cube_02.fbx"));
    object->makeRigidBody(100, ComponentsManager::get()->getComponentCollisions()->getDynamicsWorld());
    Brakeza3D::get()->addObject3D(object, "test");
}

void ComponentGame::preUpdate() {
    if (isAutoscrollEnabled()) {
        shaderYScroll -= autoScrollSpeed.y/ 10;
    }

    shaderImageBackground.setupFlatPortion(0, 0, 0, (int) shaderYScroll, 320, 240);
    shaderImageBackground.onUpdate();
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
        //shaderTintScreen.onUpdate();
        //shaderWater.onUpdate();
    }

    if (state == GameState::FEADEOUT) {
        Drawable::drawFadeOut();
    }

    if (state == GameState::FADEIN) {
        Drawable::drawFadeIn();
    }

    if (state == GameState::GAMING) {
        checkPlayerCameraScrollCollision();

        if (isAutoscrollEnabled()) {
            CameraAutoScroll();
        }
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

void ComponentGame::CameraAutoScroll() {
    Camera3D *camera = ComponentsManager::get()->getComponentCamera()->getCamera();
    camera->addToPosition(autoScrollSpeed);
}

void ComponentGame::checkPlayerCameraScrollCollision() {
    auto *camera = ComponentsManager::get()->getComponentCamera()->getCamera();

    Vertex3D homogeneousPosition;
    Vertex3D destinyPoint = player->getPosition() + player->getVelocity();
    Transforms::cameraSpace(homogeneousPosition, destinyPoint, camera);
    homogeneousPosition = Transforms::PerspectiveNDCSpace(homogeneousPosition, camera->frustum);

    // Down (if player collision bottom near camera plane, update his velocity with autoScroll speed
    if (homogeneousPosition.y > 1) {
        player->setPosition(player->getPosition() + autoScrollSpeed);
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

bool ComponentGame::isAutoscrollEnabled() {
    return autoScrollEnabled;
}

void ComponentGame::setAutoScroll(bool v) {
    autoScrollEnabled = v;
}

void ComponentGame::setAutoScrollSpeed(Vertex3D speed) {
    autoScrollSpeed = speed;
}

void ComponentGame::setGameState(GameState state) {
    if (state == GameState::GAMING) {
        setAutoScroll(true);
    } else {
        setAutoScroll(false);
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
    player->setPosition(Vertex3D(1115, -700, 2600));
    player->setRotation(90, 90, 0);
    player->setScale(1);
    player->AssimpLoadGeometryFromFile(std::string(EngineSetup::get()->MODELS_FOLDER + "spaceship03.fbx"));
    player->makeGhostBody(ComponentsManager::get()->getComponentCollisions()->getDynamicsWorld(), player);
    Brakeza3D::get()->addObject3D(player, "player");
}

void ComponentGame::setupWeapons() {
    auto *cw = ComponentsManager::get()->getComponentWeapons();
    cw->addWeaponType("defaultWeapon");
    WeaponType *weaponType = cw->getWeaponTypeByLabel("defaultWeapon");
    weaponType->setSpeed(10);
    weaponType->setDispersion(10);
    weaponType->setAvailable(true);
    weaponType->setAccuracy(100);

    auto *ammoType = new AmmoType();
    ammoType->setName("defaultWeapon_ammoType");
    ammoType->getModelProjectile()->AssimpLoadGeometryFromFile(std::string(EngineSetup::get()->MODELS_FOLDER + "bullet.fbx"));
    ammoType->getModelProjectile()->setScale(10);
    ammoType->getModelProjectile()->setFlatTextureColor(true);
    ammoType->setAmount(1000);
    weaponType->setAmmoType(ammoType);

    cw->setCurrentWeaponIndex(WeaponsTypes::DEFAULT);
}
