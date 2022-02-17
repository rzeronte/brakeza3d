#include "../../include/Components/ComponentGame.h"
#include "../../include/Components/ComponentCollisions.h"
#include "../../include/Brakeza3D.h"
#include "../../include/Particles/ParticleEmissorGravity.h"
#include "../../include/Render/Transforms.h"

ComponentGame::ComponentGame() {
    player = new Player();
}

void ComponentGame::onStart() {
    Logging::Log("ComponentGame onStart", "ComponentGame");

    SETUP->MENU_ACTIVE = true;

    Mix_PlayMusic(BUFFERS->soundPackage->getMusicByLabel("musicMainMenu"), -1);

    ComponentsManager::get()->getComponentCollisions()->initBulletSystem();
    ComponentsManager::get()->getComponentCamera()->getCamera()->setPosition(Vertex3D(0, -1000,0));
    ComponentsManager::get()->getComponentCamera()->setIsFlyMode(false);

    shaderImageBackground = ShaderImageBackground(std::string(SETUP->IMAGES_FOLDER + "brakeza.png").c_str());
    shaderImageBackground.setType(ShaderImageBackgroundTypes::CENTER);

    shaderTintScreen.setTintColorIntensity(1, 0, 0);

    loadObjects3D();
}

void ComponentGame::preUpdate() {
    shaderYScroll += 0.5;

    shaderImageBackground.setupFlatPortion(0, 0, 0, shaderYScroll, 400, 400);
    shaderImageBackground.onUpdate();
}

void ComponentGame::onUpdate() {
    ComponentHUD *componentHUD = ComponentsManager::get()->getComponentHUD();

    Object3D *moon = Brakeza3D::get()->getObjectByLabel("moon");
    moon->setRotation( M3::getMatrixRotationForEulerAngles(0, 0, 0.15) * moon->getRotation());

    Object3D *moon2 = Brakeza3D::get()->getObjectByLabel("moon2");
    moon2->setRotation( M3::getMatrixRotationForEulerAngles(0.15, 0, 0.15) * moon->getRotation());

    Object3D *moon3 = Brakeza3D::get()->getObjectByLabel("moon3");
    moon3->setRotation( M3::getMatrixRotationForEulerAngles(0, 0.25, 0.15) * moon->getRotation());

    autoScroll();

    if (player->state != PlayerState::GAMEOVER) {
        if (SETUP->ENABLE_IA) {
            onUpdateIA();
        }
    }

    if (SETUP->LOADING) {
        componentHUD->writeTextMiddleScreen("Loading", false);
    }

    if (SETUP->MENU_ACTIVE) {
        shaderTintScreen.onUpdate();
        //shaderWater.onUpdate();
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


void ComponentGame::loadObjects3D()
{
    player->setLabel("spacehip");
    //player->setEnableLights(false);
    player->setPosition(Vertex3D(15, -700, 1200));
    player->setRotation(90, 90, 0);
    player->setScale(0.45);
    player->AssimpLoadGeometryFromFile(std::string(EngineSetup::get()->MODELS_FOLDER + "spaceship.fbx"));
    Brakeza3D::get()->addObject3D(player, "player");

    auto moon = new Mesh3D();
    moon->setLabel("moon");
    moon->setScale(5);
    moon->setFlatTextureColor(true);
    moon->setPosition(Vertex3D(548, -600, 1700));
    moon->AssimpLoadGeometryFromFile(std::string(EngineSetup::get()->MODELS_FOLDER + "moon.fbx"));
    Brakeza3D::get()->addObject3D(moon, "moon");


    auto moon2 = new Mesh3D();
    moon2->setLabel("moon2");
    moon2->setScale(9);
    moon2->setFlatTextureColor(true);
    moon2->setPosition(Vertex3D(-1145, -2000, 2100));
    moon2->AssimpLoadGeometryFromFile(std::string(EngineSetup::get()->MODELS_FOLDER + "moon.fbx"));
    Brakeza3D::get()->addObject3D(moon2, "moon2");

    auto moon3 = new Mesh3D();
    moon3->setLabel("moon3");
    moon3->setScale(2);
    moon3->setFlatTextureColor(true);
    moon3->setPosition(Vertex3D(950, -2300, 1500));
    moon3->AssimpLoadGeometryFromFile(std::string(EngineSetup::get()->MODELS_FOLDER + "moon.fbx"));
    Brakeza3D::get()->addObject3D(moon3, "moon3");

    auto *lp1 = new LightPoint3D();
    lp1->setEnabled(true);
    lp1->setLabel("lp2");
    lp1->setPower(40);
    lp1->setColor(229, 229, 30);
    lp1->setColorSpecularity(220, 220, 30);
    lp1->setSpecularComponent(20);
    lp1->setPosition(Vertex3D(35, 299, 1651));
    lp1->setColor(0, 255, 0);
    lp1->setRotation(270, 0, 0);
    Brakeza3D::get()->addObject3D(lp1, "lp1");

    auto *lp2 = new LightPoint3D();
    lp2->setEnabled(true);
    lp2->setLabel("lp3");
    lp2->setPower(40);
    lp2->setColor(255, 0, 0);
    lp2->setColorSpecularity(0, 220, 30);
    lp2->setSpecularComponent(20);
    lp2->setPosition(Vertex3D(60, -1950, 1600));
    lp2->setRotation(90, 0, 0);
    lp2->setColor(0, 0, 255);
    Brakeza3D::get()->addObject3D(lp2, "lp2");

    auto *particles = new ParticleEmissorGravity(true, 120, 10, 0.01, Color::gray());
    particles->setPosition(Vertex3D(-189, -1200, 670));
    particles->setRotationFrame(17, 5, 0);
    Brakeza3D::get()->addObject3D(particles, "particles");

    auto *particles2 = new ParticleEmissorGravity(true, 20, 5, 0.1, Color::green());
    particles2->setPosition(Vertex3D(10, 22, 183));
    Brakeza3D::get()->addObject3D(particles2, "particles2");

}

void ComponentGame::autoScroll() {
    Camera3D *camera = ComponentsManager::get()->getComponentCamera()->getCamera();

    camera->setPosition(camera->getPosition() + player->autoScrollSpeed );
}


