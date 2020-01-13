#include <vector>
#include "BulletCollision/CollisionDispatch/btGhostObject.h"
#include <thread>
#include "../../headers/Game/Game.h"
#include "../../headers/Render/Maths.h"
#include "../../headers/Physics/Sprite3DBody.h"
#include "../../headers/Brakeza3D.h"
#include "../../headers/Render/Drawable.h"
#include "../../headers/Collisions/CollisionResolverBetweenProjectileAndBSPMap.h"
#include "../../headers/Collisions/CollisionResolverBetweenProjectileAndNPCEnemy.h"
#include "../../headers/Collisions/CollisionResolverBetweenCamera3DAndFuncDoor.h"
#include "../../headers/Collisions/CollisionResolverBetweenCamera3DAndFuncButton.h"
#include "../../headers/Collisions/CollisionResolverBetweenEnemyPartAndBSPMap.h"
#include "../../headers/Collisions/CollisionResolverBetweenProjectileAndPlayer.h"
#include "../../headers/Collisions/CollisionResolverBetweenCamera3DAndItemWeapon.h"
#include "../../headers/Collisions/CollisionResolverBetweenCamera3DAndItemHealth.h"
#include "../../headers/Collisions/CollisionResolverBetweenCamera3DAndItemAmmo.h"

Game* Game::instance = 0;

Game::Game()
{
    player      = new Player();
    controller  = new GameInputController( player );

    //Brakeza3D::get()->setController( controller );

    counterWalkOnWater.setStep(2);
}

Game* Game::get()
{
    if (instance == 0) {
        instance = new Game();
    }

    return instance;
}

void Game::start()
{
    //onStart();
    //mainLoop();
    onEnd();
}

void Game::onStart()
{
    //Engine::onStart();

    // Load JSON Config
    //Brakeza3D::get()->getComponentMenu()->getOptionsJSON();

    Mix_PlayMusic( EngineBuffers::getInstance()->soundPackage->getMusicByLabel("musicMainMenu"), -1 );
}


void Game::onUpdate()
{
    /*Brakeza3D *brakeza = Brakeza3D::get();
    EngineSetup *setup = EngineSetup::getInstance();

    // Core onUpdate
    Engine::onUpdate();

    if (player->state != PlayerState::GAMEOVER) {
        if (setup->DRAW_WEAPON && brakeza->getComponentWeapons()->getCurrentWeaponType()->isAvailable() ) {
            brakeza->getComponentWeapons()->onUpdate(Brakeza3D::get()->getCamera(), Brakeza3D::get()->screenSurface);
        }

        if (EngineSetup::getInstance()->DRAW_HUD) {
            drawHUD();
        }

        if (brakeza->getBSP()->isLoaded() && brakeza->getBSP()->isCurrentLeafLiquid() && !setup->MENU_ACTIVE) {
            Drawable::waterShader( Brakeza3D::get()->getBSP()->currentLeaf->type );
        }

        if (brakeza->getBSP()->isCurrentLeafLiquid()) {
            player->setOxygen(player->getOxygen() - 0.5);
        } else {
            player->setOxygen(100);
        }

        if (brakeza->getBSP()->isCurrentLeafLiquid() && brakeza->getBSP()->currentLeaf->type == -5) {
            player->takeDamage( 5 );
        }

        if ( setup->ENABLE_IA ) {
            onUpdateIA();
        }

        if ( setup->DRAW_CROSSHAIR ) {
            Drawable::drawCrossHair();
        }

        if ( player->isDead() ) {
            redScreen();
        }

        if ( player->tookDamage ) {
            redScreen();
        }

    }

    if (EngineSetup::getInstance()->MENU_ACTIVE) {
        drawMenuScreen();
    }

    if (EngineSetup::getInstance()->LOADING) {
        SDL_BlitSurface(this->HUDTextures->getTextureByLabel("loading")->getSurface(1), NULL, Brakeza3D::get()->screenSurface, NULL);
        Tools::writeTextCenterHorizontal(Brakeza3D::get()->renderer, Brakeza3D::get()->fontDefault, Color::red(), "Loading...", 100);
        Drawable::drawFireShader();
    }

    if (EngineSetup::getInstance()->SPLASHING) {
        SDL_BlitSurface(this->HUDTextures->getTextureByLabel("splash")->getSurface(1), NULL, Brakeza3D::get()->screenSurface, NULL);
        Drawable::drawFireShader();

        if (Brakeza3D::get()->splashCounter->isFinished()) {
            Brakeza3D::get()->splashCounter->setEnabled(true);

            EngineSetup::getInstance()->SPLASHING = false;
            EngineSetup::getInstance()->MENU_ACTIVE = true;
            EngineSetup::getInstance()->FADEIN = true;
            EngineSetup::getInstance()->FADEOUT = false;
        }
        if (Brakeza3D::get()->splashCounter->getAcumulatedTime() > Brakeza3D::get()->splashCounter->getStep() * 0.75) {
            EngineSetup::getInstance()->FADEOUT = true;
            EngineSetup::getInstance()->FADEIN = false;
        }
    }

    if ( EngineSetup::getInstance()->FADEOUT ) {
        Drawable::drawFadeOut();
    }

    if ( EngineSetup::getInstance()->FADEIN ) {
        Drawable::drawFadeIn();
    }*/
}

void Game::preUpdate()
{
    Brakeza3D* brakeza3D = Brakeza3D::get();

    player->evalStatusMachine();

    //Engine::preUpdate();

    changeLeafType = false;
    bool liquidToEarth = false;

    if (brakeza3D->getComponentsManager()->getComponentBSP()->getBsp()->isLoaded()) {
        if (brakeza3D->getComponentsManager()->getComponentBSP()->getBsp()->currentLeaf->type != gameLeafType) {
            if ( BSPMap::isLeafLiquid( gameLeafType ) && !BSPMap::isLeafLiquid( brakeza3D->getComponentsManager()->getComponentBSP()->getBsp()->currentLeaf->type )) {
                liquidToEarth = true;
            }

            gameLeafType = brakeza3D->getComponentsManager()->getComponentBSP()->getBsp()->currentLeaf->type;
            changeLeafType = true;
        }

        if (changeLeafType) {
            if ( brakeza3D->getComponentsManager()->getComponentBSP()->getBsp()->isCurrentLeafLiquid() ) {
                //brakeza3D->getComponentCamera()->kinematicController->setFallSpeed(5);
            } else {
                //brakeza3D->getComponentCamera()->kinematicController->setFallSpeed(256);
            }

            //if (liquidToEarth && !Brakeza3D::get()->getComponentCamera()->kinematicController->onGround()) {
            //    controller->jump( false, EngineSetup::getInstance()->JUMP_FORCE.y, false);
            //}
        }
    }
}

void Game::onEnd()
{
}

void Game::onUpdateInputController()
{
    while (SDL_PollEvent(&e)) {
        ImGui_ImplSDL2_ProcessEvent(&e);

        getController()->updateKeyboardMapping();
        getController()->updateMouseStates(&this->e);
        getController()->handleMouse(&this->e);
        getController()->handleWindowEvents(&this->e, Brakeza3D::get()->finish);
        getController()->handleInGameInput(&this->e, Brakeza3D::get()->finish);
    }

    // Check array Uint8 *keyboard
    getController()->handleMovingCamera(&this->e, this->finish);
}

void Game::onUpdateIA()
{
    if (player->isDead()) return;

    std::vector<Object3D *>::iterator itObject3D;
    for ( itObject3D = Brakeza3D::get()->getSceneObjects().begin(); itObject3D != Brakeza3D::get()->getSceneObjects().end(); itObject3D++) {
        Object3D *object = *(itObject3D);

        auto *enemy = dynamic_cast<NPCEnemyBody*> (object);

        if (enemy != nullptr) {

            enemy->updateCounters();

            if (!Brakeza3D::get()->getComponentsManager()->getComponentCamera()->getCamera()->frustum->isPointInFrustum(*object->getPosition())) {
                continue;
            }

            if (enemy->isDead()) {
                continue;
            }

            Vertex3D A = *object->getPosition();
            Vertex3D B = *Brakeza3D::get()->getComponentsManager()->getComponentCamera()->getCamera()->getPosition();

            Vector3D ray = Vector3D(A, B);

            bool rayCastResult = Brakeza3D::get()->getComponentsManager()->getComponentBSP()->getBsp()->recastWrapper->rayCasting(A, B);

            if (enemy->counterIA->isFinished()) {
                Brakeza3D::get()->getComponentsManager()->getComponentBSP()->getBsp()->recastWrapper->getPathBetween(A, B, enemy->points);
            }

            enemy->evalStatusMachine(
                    rayCastResult,
                    ray.getComponent().getModule(),
                    Brakeza3D::get()->getComponentsManager()->getComponentCamera()->getCamera(),
                    Brakeza3D::get()->getComponentsManager()->getComponentCollisions()->getDynamicsWorld(),
                    Brakeza3D::get()->getSceneObjects()
            );
        }
    }
}

void Game::resolveCollisions()
{
    ComponentCollisions *cm = Brakeza3D::get()->getComponentsManager()->getComponentCollisions();

    std::vector<CollisionResolver *>::iterator itCollision;
    for ( itCollision = cm->getCollisions().begin(); itCollision != cm->getCollisions().end(); itCollision++) {
        CollisionResolver *collision = *(itCollision);
        int collisionType = collision->getTypeCollision();

        if (!collisionType) continue;

        if ( collisionType == EngineSetup::getInstance()->CollisionResolverTypes::COLLISION_RESOLVER_PROJECTILE_AND_BSPMAP ) {
            auto *resolver = new CollisionResolverBetweenProjectileAndBSPMap(
                    collision->contactManifold,
                    collision->objA,
                    collision->objB,
                    cm->getBspMap(),
                    cm->getSceneObjects(),
                    cm->getDynamicsWorld(),
                    Brakeza3D::get()->getComponentsManager()->getComponentWeapons(),
                    cm->getVisibleTriangles()
            );
            resolver->dispatch();
            continue;
        }

        if ( collisionType == EngineSetup::getInstance()->CollisionResolverTypes::COLLISION_RESOLVER_PROJECTILE_AND_NPCENEMY ) {
            auto *resolver = new CollisionResolverBetweenProjectileAndNPCEnemy(
                    collision->contactManifold,
                    collision->objA,
                    collision->objB,
                    cm->getBspMap(),
                    cm->getSceneObjects(),
                    cm->getDynamicsWorld(),
                    Brakeza3D::get()->getComponentsManager()->getComponentWeapons(),
                    cm->getVisibleTriangles()
            );
            resolver->dispatch();
            continue;
        }

        if ( collisionType == EngineSetup::getInstance()->CollisionResolverTypes::COLLISION_RESOLVER_CAMERA_AND_FUNCDOOR ) {
            auto *resolver = new CollisionResolverBetweenCamera3DAndFuncDoor(
                    collision->contactManifold,
                    collision->objA,
                    collision->objB,
                    cm->getBspMap(),
                    cm->getSceneObjects(),
                    cm->getVisibleTriangles()
            );
            resolver->dispatch();
            continue;
        }

        if ( collisionType == EngineSetup::getInstance()->CollisionResolverTypes::COLLISION_RESOLVER_CAMERA_AND_FUNCBUTTON ) {
            auto *resolver = new CollisionResolverBetweenCamera3DAndFuncButton(
                    collision->contactManifold,
                    collision->objA,
                    collision->objB,
                    cm->getBspMap(),
                    cm->getSceneObjects(),
                    cm->getVisibleTriangles()
            );
            resolver->dispatch();
            continue;
        }

        if ( collisionType == EngineSetup::getInstance()->CollisionResolverTypes::COLLISION_RESOLVER_NPCENEMYPART_AND_BSPMAP ) {
            auto *resolver = new CollisionResolverBetweenEnemyPartAndBSPMap(
                    collision->contactManifold,
                    collision->objA,
                    collision->objB,
                    cm->getBspMap(),
                    cm->getSceneObjects(),
                    cm->getDynamicsWorld(),
                    Brakeza3D::get()->getComponentsManager()->getComponentWeapons(),
                    cm->getVisibleTriangles()
            );
            resolver->dispatch();
            continue;
        }

        if ( collisionType == EngineSetup::getInstance()->CollisionResolverTypes::COLLISION_RESOLVER_PROJECTILE_AND_CAMERA ) {
            auto *resolver = new CollisionResolverBetweenProjectileAndPlayer(
                    collision->contactManifold,
                    collision->objA,
                    collision->objB,
                    cm->getBspMap(),
                    cm->getSceneObjects(),
                    cm->getDynamicsWorld(),
                    Brakeza3D::get()->getComponentsManager()->getComponentWeapons(),
                    cm->getVisibleTriangles(),
                    player
            );
            resolver->dispatch();
            continue;
        }

        if ( collisionType == EngineSetup::getInstance()->CollisionResolverTypes::COLLISION_RESOLVER_ITEMWEAPON_AND_CAMERA ) {
            auto *resolver = new CollisionResolverBetweenCamera3DAndItemWeapon(
                    collision->contactManifold,
                    collision->objA,
                    collision->objB,
                    cm->getBspMap(),
                    cm->getSceneObjects(),
                    cm->getDynamicsWorld(),
                    Brakeza3D::get()->getComponentsManager()->getComponentWeapons(),
                    cm->getVisibleTriangles()
            );
            resolver->dispatch();
            continue;
        }

        if ( collisionType == EngineSetup::getInstance()->CollisionResolverTypes::COLLISION_RESOLVER_ITEMHEALTH_AND_CAMERA ) {
            auto *resolver = new CollisionResolverBetweenCamera3DAndItemHealth(
                    collision->contactManifold,
                    collision->objA,
                    collision->objB,
                    cm->getBspMap(),
                    cm->getSceneObjects(),
                    cm->getDynamicsWorld(),
                    Brakeza3D::get()->getComponentsManager()->getComponentWeapons(),
                    cm->getVisibleTriangles(),
                    player
            );
            resolver->dispatch();
            continue;
        }

        if ( collisionType == EngineSetup::getInstance()->CollisionResolverTypes::COLLISION_RESOLVER_ITEMAMMO_AND_CAMERA ) {
            auto *resolver = new CollisionResolverBetweenCamera3DAndItemAmmo(
                    collision->contactManifold,
                    collision->objA,
                    collision->objB,
                    cm->getBspMap(),
                    cm->getSceneObjects(),
                    cm->getDynamicsWorld(),
                    Brakeza3D::get()->getComponentsManager()->getComponentWeapons(),
                    cm->getVisibleTriangles(),
                    player
            );
            resolver->dispatch();
            continue;
        }
    }

    cm->getCollisions().clear();
}

void Game::redScreen()
{
    float intensity_r = 1;
    float intensity_g = 0.5;
    float intensity_b = 0.5;

    for (int y = 0; y < EngineSetup::getInstance()->screenHeight; y++) {
        for (int x = 0; x < EngineSetup::getInstance()->screenWidth; x++) {
            Uint32 currentPixelColor = EngineBuffers::getInstance()->getVideoBuffer(x, y);

            int r_light = (int) (Tools::getRedValueFromColor(currentPixelColor) * intensity_r);
            int g_light = (int) (Tools::getGreenValueFromColor(currentPixelColor) * intensity_g);
            int b_light = (int) (Tools::getBlueValueFromColor(currentPixelColor) * intensity_b);

            currentPixelColor = Tools::createRGB(r_light, g_light, b_light);
            EngineBuffers::getInstance()->setVideoBuffer( x, y, currentPixelColor );
        }
    }
}

void Game::drawMenuScreen()
{
    /*Tools::writeText(Brakeza3D::get()->renderer, Brakeza3D::get()->fontMedium, 20, 10, Color::white(), "Code Project: DarkHeaZ");
    Tools::writeText(Brakeza3D::get()->renderer, Brakeza3D::get()->fontMedium, 20, 30, Color::white(), "Alpha Version");

    Brakeza3D::get()->getComponentMenu()->drawOptions(Brakeza3D::get()->screenSurface);
    Drawable::drawFireShader();

    Tools::writeText(Brakeza3D::get()->renderer, Brakeza3D::get()->fontSmall, 5, 225, Color::white(), "Powered By Brakeza3D - https://brakeza.com");
     */
}


void Game::initBSP()
{
    //cJSON *firstMap = cJSON_GetArrayItem(mapsJSONList, 0);
    //cJSON *nameMap  = cJSON_GetObjectItemCaseSensitive(firstMap, "name");

    EngineSetup::getInstance()->LOADING     = true;
    EngineSetup::getInstance()->MENU_ACTIVE = false;

    //Brakeza3D::get()->initBSP(nameMap->valuestring, &this->frameTriangles);
}

GameInputController *Game::getController() const {
    return controller;
}
