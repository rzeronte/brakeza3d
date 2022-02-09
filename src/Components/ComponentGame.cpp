#include "../../include/Components/ComponentGame.h"
#include "../../include/Components/ComponentCollisions.h"
#include "../../include/Brakeza3D.h"
#include "../../include/Render/M4.h"
#include "../../include/Objects/ParticleEmissor.h"

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
    Vertex3D originalCameraPosition = Vertex3D(73, -10, 1);

    // start frustum position
    camera->setPosition(originalCameraPosition);

    auto *cubes = new Mesh3D();
    cubes->setLabel("cubes");
    cubes->setPosition(Vertex3D(100, 100, 100));
    cubes->setScale(100);
    cubes->AssimpLoadGeometryFromFile(std::string(EngineSetup::get()->MODELS_FOLDER + "cubes.fbx"));
    //Brakeza3D::get()->addObject3D(cubes, "cubes");

    auto *lp1 = new LightPoint3D();
    lp1->setEnabled(true);
    lp1->setLabel("lp2");
    lp1->setPower(40);
    lp1->setColor(229, 229, 30);
    lp1->setColorSpecularity(220, 220, 30);
    lp1->setSpecularComponent(20);
    lp1->setPosition(Vertex3D(1045, 145, 230));
    lp1->setColor(255, 255, 255);
    //Brakeza3D::get()->addObject3D(lp1, "lp1");

    auto *lp2 = new LightPoint3D();
    lp2->setEnabled(true);
    lp2->setLabel("lp3");
    lp2->setPower(40);
    lp2->setColor(229, 229, 30);
    lp2->setColorSpecularity(220, 220, 30);
    lp2->setSpecularComponent(20);
    lp2->setPosition(Vertex3D(100, 500, 120));
    lp2->setColor(0, 0, 255);
    //Brakeza3D::get()->addObject3D(lp2, "lp2");

    auto *particles = new ParticleEmissor(true, 20, 10, 0.1, Color::fuchsia());
    particles->setPosition(Vertex3D(62, 22, 183));
    Brakeza3D::get()->addObject3D(particles, "particles");


    auto *particles2 = new ParticleEmissor(true, 30, 5, 0.1, Color::green());
    particles2->setPosition(Vertex3D(10, 22, 183));
    Brakeza3D::get()->addObject3D(particles2, "particles2");

    ComponentsManager::get()->getComponentCamera()->setIsFlyMode(true);
}

void ComponentGame::preUpdate() {
}

void ComponentGame::onUpdate() {
    Camera3D *camera = ComponentsManager::get()->getComponentCamera()->getCamera();
    EngineSetup *setup = EngineSetup::get();

    ComponentWeapons *componentWeapons = ComponentsManager::get()->getComponentWeapons();
    ComponentWindow *componentWindow = ComponentsManager::get()->getComponentWindow();
    ComponentHUD *componentHUD = ComponentsManager::get()->getComponentHUD();

    Object3D *particles = Brakeza3D::get()->getObjectByLabel("particles");
    particles->setRotation( M3::getMatrixRotationForEulerAngles(0, 3.75, 0) * particles->getRotation());

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

            if (!Brakeza3D::get()->getComponentsManager()->getComponentCamera()->getCamera()->frustum->isVertexInside(
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
            auto currentPixelColor = Color(BUFFERS->getVideoBuffer(x, y));

            int r_light = (int) ((float) Tools::getRedValueFromColor(currentPixelColor.getColor()) * intensity_r);
            int g_light = (int) ((float) Tools::getGreenValueFromColor(currentPixelColor.getColor()) * intensity_g);
            int b_light = (int) ((float) Tools::getBlueValueFromColor(currentPixelColor.getColor()) * intensity_b);

            currentPixelColor = Color(r_light, g_light, b_light);
            BUFFERS->setVideoBuffer(x, y, currentPixelColor.getColor());
        }
    }
}


