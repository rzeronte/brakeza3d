//
// Created by darkhead on 14/1/20.
//

#include "../../headers/Components/ComponentGame.h"
#include "../../headers/EngineBuffers.h"
#include "../../headers/Misc/Tools.h"
#include "../../headers/Components/ComponentCollisions.h"
#include "../../headers/Collisions/CollisionResolverBetweenProjectileAndBSPMap.h"
#include "../../headers/Collisions/CollisionResolverBetweenProjectileAndNPCEnemy.h"
#include "../../headers/Collisions/CollisionResolverBetweenCamera3DAndFuncDoor.h"
#include "../../headers/Collisions/CollisionResolverBetweenCamera3DAndFuncButton.h"
#include "../../headers/Collisions/CollisionResolverBetweenEnemyPartAndBSPMap.h"
#include "../../headers/Collisions/CollisionResolverBetweenProjectileAndPlayer.h"
#include "../../headers/Collisions/CollisionResolverBetweenCamera3DAndItemWeapon.h"
#include "../../headers/Collisions/CollisionResolverBetweenCamera3DAndItemHealth.h"
#include "../../headers/Collisions/CollisionResolverBetweenCamera3DAndItemAmmo.h"

ComponentGame::ComponentGame()
{
    player = new Player();
}

void ComponentGame::onStart()
{
    SETUP->MENU_ACTIVE = true;
    Mix_PlayMusic( BUFFERS->soundPackage->getMusicByLabel("musicMainMenu"), -1 );
}

void ComponentGame::preUpdate()
{
    ComponentsManager::get()->getComponentGame()->getPlayer()->tookDamage = false;
    if (player->state != PlayerState::GAMEOVER) {
        this->resolveCollisions();
    }
}

void ComponentGame::onUpdate() 
{
    EngineSetup      *setup            = EngineSetup::getInstance();
    ComponentWeapons *componentWeapons = ComponentsManager::get()->getComponentWeapons();
    Camera3D         *camera           = ComponentsManager::get()->getComponentCamera()->getCamera();
    BSPMap           *mapBSP           = ComponentsManager::get()->getComponentBSP()->getBSP();
    ComponentWindow  *componentWindow  = ComponentsManager::get()->getComponentWindow();
    ComponentHUD     *componentHUD     = ComponentsManager::get()->getComponentHUD();


    if (player->state != PlayerState::GAMEOVER) {

        if (mapBSP->isLoaded() && mapBSP->isCurrentLeafLiquid() && !setup->MENU_ACTIVE) {
            Drawable::waterShader( mapBSP->currentLeaf->type );
        }

        if (mapBSP->isCurrentLeafLiquid()) {
            player->setOxygen(player->getOxygen() - 0.5);
        } else {
            player->setOxygen(100);
        }

        if (mapBSP->isCurrentLeafLiquid() && mapBSP->currentLeaf->type == -5) {
            player->takeDamage( 5 );
        }

        if ( setup->ENABLE_IA ) {
            onUpdateIA();
        }

        if ( player->isDead() || player->tookDamage ) {
            redScreen();
        }
    }

    if (SETUP->LOADING) {
        SDL_BlitSurface(componentHUD->HUDTextures->getTextureByLabel("loading")->getSurface(1), NULL, componentWindow->screenSurface, NULL);
        std::string text = "Loading";
        Tools::writeTextCenterHorizontal( componentWindow->renderer, componentWindow->fontDefault, Color::red(), text, 100);
        Drawable::drawFireShader();
    }

    if ( SETUP->FADEOUT ) {
        Drawable::drawFadeOut();
    }

    if ( SETUP->FADEIN ) {
        Drawable::drawFadeIn();
    }
}

void ComponentGame::postUpdate()
{
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


void ComponentGame::onUpdateIA()
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

            bool rayCastResult = Brakeza3D::get()->getComponentsManager()->getComponentBSP()->getBSP()->recastWrapper->rayCasting(A, B);

            if (enemy->counterIA->isFinished()) {
                Brakeza3D::get()->getComponentsManager()->getComponentBSP()->getBSP()->recastWrapper->getPathBetween(A, B, enemy->points);
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

void ComponentGame::resolveCollisions()
{
    ComponentCollisions *cm = Brakeza3D::get()->getComponentsManager()->getComponentCollisions();

    std::vector<CollisionResolver *>::iterator itCollision;
    for ( itCollision = cm->getCollisions().begin(); itCollision != cm->getCollisions().end(); itCollision++) {
        CollisionResolver *collision = *(itCollision);
        int collisionType = collision->getTypeCollision();

        if (!collisionType) continue;

        if ( collisionType == SETUP->CollisionResolverTypes::COLLISION_RESOLVER_PROJECTILE_AND_BSPMAP ) {
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

        if ( collisionType == SETUP->CollisionResolverTypes::COLLISION_RESOLVER_PROJECTILE_AND_NPCENEMY ) {
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

        if ( collisionType == SETUP->CollisionResolverTypes::COLLISION_RESOLVER_CAMERA_AND_FUNCDOOR ) {
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

        if ( collisionType == SETUP->CollisionResolverTypes::COLLISION_RESOLVER_CAMERA_AND_FUNCBUTTON ) {
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

        if ( collisionType == SETUP->CollisionResolverTypes::COLLISION_RESOLVER_NPCENEMYPART_AND_BSPMAP ) {
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

        if ( collisionType == SETUP->CollisionResolverTypes::COLLISION_RESOLVER_PROJECTILE_AND_CAMERA ) {
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

        if ( collisionType == SETUP->CollisionResolverTypes::COLLISION_RESOLVER_ITEMWEAPON_AND_CAMERA ) {
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

        if ( collisionType == SETUP->CollisionResolverTypes::COLLISION_RESOLVER_ITEMHEALTH_AND_CAMERA ) {
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

        if ( collisionType == SETUP->CollisionResolverTypes::COLLISION_RESOLVER_ITEMAMMO_AND_CAMERA ) {
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

void ComponentGame::redScreen()
{
    float intensity_r = 1;
    float intensity_g = 0.5;
    float intensity_b = 0.5;

    for (int y = 0; y < SETUP->screenHeight; y++) {
        for (int x = 0; x < SETUP->screenWidth; x++) {
            Uint32 currentPixelColor = BUFFERS->getVideoBuffer(x, y);

            int r_light = (int) (Tools::getRedValueFromColor(currentPixelColor) * intensity_r);
            int g_light = (int) (Tools::getGreenValueFromColor(currentPixelColor) * intensity_g);
            int b_light = (int) (Tools::getBlueValueFromColor(currentPixelColor) * intensity_b);

            currentPixelColor = Tools::createRGB(r_light, g_light, b_light);
            BUFFERS->setVideoBuffer( x, y, currentPixelColor );
        }
    }
}