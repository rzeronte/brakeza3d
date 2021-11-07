#include "../../include/Components/ComponentGame.h"
#include "../../include/Components/ComponentCollisions.h"
#include "../../include/Brakeza3D.h"

ComponentGame::ComponentGame() {
    player = new Player();
}

void ComponentGame::onStart() {
    Logging::Log("ComponentGame onStart", "ComponentGame");

    ComponentsManager::get()->getComponentCollisions()->initBulletSystem();

    SETUP->MENU_ACTIVE = true;
    Mix_PlayMusic(BUFFERS->soundPackage->getMusicByLabel("musicMainMenu"), -1);

    startThirdPerson();
}

void ComponentGame::startThirdPerson() {
    Camera3D *camera = ComponentsManager::get()->getComponentCamera()->getCamera();
    Vertex3D originalCameraPosition = Vertex3D(-65, 54, 273);

    // start cam position
    camera->setPosition(originalCameraPosition);
    camera->yaw = 180;
    camera->pitch = 0;
    camera->roll = 0;

    camera->UpdateRotation();

    auto *dungeon = new Mesh3D();
    dungeon->setLabel("scene");
    dungeon->setPosition(Vertex3D(100, 100, 100));
    dungeon->setScale(0.1);
    dungeon->AssimpLoadGeometryFromFile(std::string(EngineSetup::getInstance()->MODELS_FOLDER + "dungeon.fbx"));
    Brakeza3D::get()->addObject3D(dungeon, "dungeon");

    ComponentsManager::get()->getComponentCamera()->setIsFlyMode(true);
//
//    auto *mainCharacter = new Mesh3D();
//    mainCharacter->setLabel("mainCharacter");
//    mainCharacter->setPosition(Vertex3D(-65, 97, 141));
//    mainCharacter->setRotation(M3::getMatrixRotationForEulerAngles(90, 0, 270));
//    mainCharacter->setScale(0.3);
//    mainCharacter->AssimpLoadGeometryFromFile(std::string(EngineSetup::getInstance()->MODELS_FOLDER + "conan.fbx"));
//    Brakeza3D::get()->addObject3D(mainCharacter, "mainCharacter");

}

void ComponentGame::preUpdate() {
}

void ComponentGame::onUpdate() {
    Camera3D *camera = ComponentsManager::get()->getComponentCamera()->getCamera();
    EngineSetup *setup = EngineSetup::getInstance();
    ComponentWeapons *componentWeapons = ComponentsManager::get()->getComponentWeapons();
    ComponentWindow *componentWindow = ComponentsManager::get()->getComponentWindow();
    ComponentHUD *componentHUD = ComponentsManager::get()->getComponentHUD();

    // set car rotation
    Vertex3D impulse = camera->velocity.getComponent();

    if (player->state != PlayerState::GAMEOVER) {
        if (setup->ENABLE_IA) {
            onUpdateIA();
        }
    }

    if (SETUP->LOADING) {
        SDL_BlitSurface(componentHUD->HUDTextures->getTextureByLabel("loading")->getSurface(1), nullptr,
                        componentWindow->screenSurface, nullptr);
        componentHUD->writeTextCenter("Loading", false);
        Drawable::drawFireShader();
    }

    if (SETUP->MENU_ACTIVE) {
        //Drawable::drawFireShader();
    }

    if (SETUP->FADEOUT) {
        Drawable::drawFadeOut();
    }

    if (SETUP->FADEIN) {
        Drawable::drawFadeIn();
    }

    if (player->state != PlayerState::GAMEOVER) {
        this->resolveCollisions();
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

int ComponentGame::getKills() const {
    return kills;
}

void ComponentGame::setKills(int kills) {
    ComponentGame::kills = kills;
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

            if (!Brakeza3D::get()->getComponentsManager()->getComponentCamera()->getCamera()->frustum->isPointInFrustum(
                    object->getPosition())) {
                continue;
            }

            if (enemy->isDead()) {
                continue;
            }

            Vertex3D A = object->getPosition();
            Vertex3D B = Brakeza3D::get()->getComponentsManager()->getComponentCamera()->getCamera()->getPosition();

            Vector3D ray = Vector3D(A, B);

            /*enemy->evalStatusMachine(
                    false,
                    ray.getComponent().getModule(),
                    Brakeza3D::get()->getComponentsManager()->getComponentCamera()->getCamera(),
                    Brakeza3D::get()->getComponentsManager()->getComponentCollisions()->getDynamicsWorld(),
                    Brakeza3D::get()->getSceneObjects()
            );*/
        }
    }
}

void ComponentGame::resolveCollisions() {
    ComponentCollisions *cm = Brakeza3D::get()->getComponentsManager()->getComponentCollisions();

    std::vector<CollisionResolver *>::iterator itCollision;
    for (itCollision = cm->getCollisions().begin(); itCollision != cm->getCollisions().end(); itCollision++) {
        CollisionResolver *collision = *(itCollision);
        int collisionType = collision->getTypeCollision();

        if (!collisionType) continue;
    }

    cm->getCollisions().clear();
}

void ComponentGame::redScreen() {
    float intensity_r = 1;
    float intensity_g = 0.5;
    float intensity_b = 0.5;

    for (int y = 0; y < SETUP->screenHeight; y++) {
        for (int x = 0; x < SETUP->screenWidth; x++) {
            auto currentPixelColor = (Uint32) BUFFERS->getVideoBuffer(x, y);

            int r_light = (int) ((float) Tools::getRedValueFromColor(currentPixelColor) * intensity_r);
            int g_light = (int) ((float) Tools::getGreenValueFromColor(currentPixelColor) * intensity_g);
            int b_light = (int) ((float) Tools::getBlueValueFromColor(currentPixelColor) * intensity_b);

            currentPixelColor = Tools::createRGB(r_light, g_light, b_light);
            BUFFERS->setVideoBuffer(x, y, currentPixelColor);
        }
    }
}


