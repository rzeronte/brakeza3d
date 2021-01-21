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

    createObjects3DFromBSPMap();
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
            Drawable::waterShader( mapBSP->currentLeaf->contents );
        }

        if (mapBSP->isCurrentLeafLiquid()) {
            player->setOxygen(player->getOxygen() - 0.5);
        } else {
            player->setOxygen(100);
        }

        if (mapBSP->isCurrentLeafLiquid() && mapBSP->currentLeaf->contents == -5) {
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

            if (!Brakeza3D::get()->getComponentsManager()->getComponentCamera()->getCamera()->frustum->isPointInFrustum( object->getPosition() )) {
                continue;
            }

            if (enemy->isDead()) {
                continue;
            }

            Vertex3D A = object->getPosition();
            Vertex3D B = Brakeza3D::get()->getComponentsManager()->getComponentCamera()->getCamera()->getPosition();

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


void ComponentGame::createObjects3DFromBSPMap() {
    EngineSetup *engineSetup = EngineSetup::getInstance();
    BSPMap *bspMap = ComponentsManager::get()->getComponentBSP()->getBSP();

    Logging::getInstance()->Log("BSP Num Entities: "+ std::to_string(bspMap->n_entities), "");
    Brakeza3D *brakeza3D = Brakeza3D::get();

    if (bspMap->n_entities > MAX_BSP_ENTITIES) {
        printf("Error: Entities overflow");
        exit(-1);
    }

    for (int i = 0; i < bspMap->n_entities; i++) {
        Logging::getInstance()->Log("BSPEntity: " + std::to_string(bspMap->entities[i].id), "");
        for (int j = 0 ; j < bspMap->entities[i].num_attributes ; j++ ) {
            Logging::getInstance()->Log("Key: '" + (std::string)bspMap->entities[i].attributes[j].key + "' - Value: '" + (std::string)bspMap->entities[i].attributes[j].value + "'", "");
        }
    }

    // Create Objects3D from BSP Entities
    for (int i = 0 ; i < bspMap->n_entities ; i++) {

        if (bspMap->hasEntityAttribute(i, "classname")) {
            char *classname = bspMap->getEntityValue(i, "classname");
            if (bspMap->hasEntityAttribute(i, "origin")) {
                char *value = bspMap->getEntityValue(i, "origin");
                Vertex3D pos = bspMap->parsePositionFromEntityAttribute(value);

                // light
                if (!strcmp(classname, "light")) {
                    Object3D *o = new Object3D();
                }

                // item cells
                if (!strcmp(classname, "item_cells")) {
                    auto *o = new ItemAmmoBody();
                    ComponentWeapons *weaponManager = ComponentsManager::get()->getComponentWeapons();
                    ComponentCollisions *componentCollisions = ComponentsManager::get()->getComponentCollisions();

                    Mesh3D *oTemplate = weaponManager->getAmmoTypeByClassname( classname )->getModelBox();

                    o->setLabel("ammo_cells");
                    o->copyFrom( oTemplate );
                    // Md2 Import for Quake2 axis adjust
                    o->setRotation( M3::getMatrixRotationForEulerAngles(90, 0, 0));
                    o->setAmmoTypeClassname( classname );
                    o->setPosition( pos );
                    o->makeSimpleRigidBody(0, pos, Vertex3D(1, 1, 1), componentCollisions->getDynamicsWorld() );
                    Brakeza3D::get()->addObject3D( o, o->getLabel() );
                }

                // item rockets
                if (!strcmp(classname, "item_rockets")) {
                    auto *o = new ItemAmmoBody();
                    Mesh3D *oTemplate = brakeza3D->getComponentsManager()->getComponentWeapons()->getAmmoTypeByClassname( classname )->getModelBox();

                    o->setLabel("ammo_rockets");
                    o->copyFrom( oTemplate );
                    // Md2 Import for Quake2 axis adjust
                    o->setRotation( M3::getMatrixRotationForEulerAngles(90, 0, 0));
                    o->setAmmoTypeClassname( classname );
                    o->setPosition( pos );
                    o->makeSimpleRigidBody(0, pos, Vertex3D(3, 3, 3), brakeza3D->getComponentsManager()->getComponentCollisions()->getDynamicsWorld() );
                    brakeza3D->addObject3D( o, o->getLabel() );
                }

                // item shells
                if (!strcmp(classname, "item_shells")) {
                    auto *o = new ItemAmmoBody();
                    Mesh3D *oTemplate = brakeza3D->getComponentsManager()->getComponentWeapons()->getAmmoTypeByClassname( classname )->getModelBox();

                    o->setLabel("ammo_shells");
                    o->copyFrom( oTemplate );
                    // Md2 Import for Quake2 axis adjust
                    o->setRotation( M3::getMatrixRotationForEulerAngles(90, 0, 0));
                    o->setAmmoTypeClassname( classname );
                    o->setPosition( pos );
                    o->makeSimpleRigidBody(0, pos, Vertex3D(3, 3, 3), brakeza3D->getComponentsManager()->getComponentCollisions()->getDynamicsWorld() );
                    brakeza3D->addObject3D( o, o->getLabel() );
                }

                // item spikes
                if (!strcmp(classname, "item_spikes")) {
                    auto *o = new ItemAmmoBody();
                    Mesh3D *oTemplate = brakeza3D->getComponentsManager()->getComponentWeapons()->getAmmoTypeByClassname( classname )->getModelBox();

                    o->setLabel("ammo_spikes");
                    o->setAmmoTypeClassname( classname );
                    o->copyFrom( oTemplate );

                    // Md2 Import for Quake2 axis adjust
                    o->setRotation( M3::getMatrixRotationForEulerAngles(90, 0, 0));

                    o->setPosition( pos );
                    o->makeSimpleRigidBody(0, pos, Vertex3D(3, 3, 3), brakeza3D->getComponentsManager()->getComponentCollisions()->getDynamicsWorld() );

                    brakeza3D->addObject3D( o, o->getLabel() );
                }

                // item_health
                /*if (!strcmp(classname, "item_health")) {
                    auto *o = new ItemHealthBody();
                    o->setLabel("health");
                    o->setPosition( pos );
                    o->loadTexture(EngineSetup::getInstance()->TEXTURES_FOLDER + "/" + EngineSetup::getInstance()->ITEM_FIRSTAID_ICON );
                    o->setDimensions( 3, 3 );
                    o->makeRigidBody(0, Vertex3D(1, 1, 1), brakeza3D->getComponentsManager()->getComponentCollisions()->getDynamicsWorld() );
                    brakeza3D->addObject3D( o, o->getLabel() );
                }*/

                // weapon wildcard
                std::string s1(classname);
                if (s1.find("weapon") != std::string::npos) {
                    auto *o = new ItemWeaponBody();

                    WeaponType *weapon = brakeza3D->getComponentsManager()->getComponentWeapons()->getWeaponTypeByClassname( classname );
                    if (weapon == NULL) {
                        Logging::getInstance()->Log("Error loading weapon by classname: " + s1, "ERROR");
                        continue;
                    }

                    o->setPosition( pos );
                    o->setLabel( weapon->label );
                    o->setWeaponClassname( classname );
                    o->copyFrom( weapon->getModel() );
                    // Md2 Import for Quake2 axis adjust
                    o->setRotation( M3::getMatrixRotationForEulerAngles(90, 0, 0));
                    o->makeSimpleRigidBody(0, pos, Vertex3D(2, 2, 2), brakeza3D->getComponentsManager()->getComponentCollisions()->getDynamicsWorld() );
                    brakeza3D->addObject3D( o, o->getLabel() );
                }

                // monster wildcard
                std::string s2(classname);
                if (s2.find("monster") != std::string::npos) {
                    NPCEnemyBody *enemyTemplate = EngineBuffers::getInstance()->getEnemyTemplateForClassname( classname );
                    if (enemyTemplate == NULL) continue;

                    // Angle Monster
                    int angle = 0;
                    if (bspMap->hasEntityAttribute(i, "angle")) {
                        char *s_angle = bspMap->getEntityValue(i, "angle");
                        angle = std::stoi(s_angle);
                    }

                    M3 rotMonster = M3::getMatrixRotationForEulerAngles(0, 90-angle, 0);

                    auto *enemyBody = new NPCEnemyBody();
                    enemyBody->setScale( enemyTemplate->getScale() );
                    enemyBody->rotationFixed = M3::getMatrixRotationForEulerAngles(90, 0, 0);
                    enemyBody->setPosition( pos );
                    enemyBody->setRotation( rotMonster );
                    enemyBody->setRespawnRotation( rotMonster );
                    enemyBody->setDrawOffset( enemyTemplate->getDrawOffset() );
                    enemyBody->setBoxShapeSize( enemyTemplate->getBoxShapeSize() );
                    enemyBody->setSpeed( enemyTemplate->getSpeed() );
                    enemyBody->setRange( enemyTemplate->getRange() );
                    enemyBody->setCadence( enemyTemplate->getCadence() );
                    enemyBody->setLabel("BSPEntity_" + std::to_string(i) + " (monster)");
                    enemyBody->addAnimation("swat_idle", "Cubex.fbx", enemyTemplate->getScale(), false);
                    enemyBody->addAnimation("swat_walk", "Cubex.fbx", enemyTemplate->getScale(), false );
                    enemyBody->addAnimation("swat_fire", "Cubex.fbx", enemyTemplate->getScale(), false );
                    enemyBody->addAnimation("swat_injuried", "Cubex.fbx", enemyTemplate->getScale(), false );
                    enemyBody->addAnimation("swat_dead", "Cubex.fbx", enemyTemplate->getScale(), true );
                    enemyBody->setAnimation(EngineSetup::SOLDIER_IDLE);
                    enemyBody->makeSimpleRigidBody(
                            0,
                            pos - enemyBody->getDrawOffset(),
                            enemyBody->getBoxShapeSize(),
                            Brakeza3D::get()->getComponentsManager()->getComponentCollisions()->getDynamicsWorld()
                    );
                    brakeza3D->addObject3D(enemyBody, enemyBody->getLabel() );
                }

                // armor wildcard
                std::string s3(classname);
                if (s2.find("armor") != std::string::npos) {
                    Object3D *o = new Object3D();
                    o->setPosition( pos );
                    Brakeza3D::get()->addObject3D(o, "BSPEntity_" + std::to_string(i) + " (armor)" );
                }

                // info_player_start
                if (!strcmp(classname, "info_player_start") ||
                    !strcmp(classname, "info_player_coop") ||
                    !strcmp(classname, "info_player_deathmatch")
                        ) {
                    Object3D *o = new Object3D();
                    o->setPosition( pos );
                    Brakeza3D::get()->addObject3D(o, "BSPEntity_" + std::to_string(i) + " (player_spawn)" );
                }

                // info teleport destination
                if (!strcmp(classname, "info_teleport_destination")) {
                    Object3D *o = new Object3D();
                    o->setPosition( pos );
                    Brakeza3D::get()->addObject3D(o, "BSPEntity_" + std::to_string(i) + " (teleport_destination)" );
                }

                // light_flame_large_yellow
                if (!strcmp(classname, "light_flame_large_yellow") || !strcmp(classname, "light_torch_small_walltorch") ) {
                    Object3D *o = new Object3D();
                    o->setPosition( pos );
                    Brakeza3D::get()->addObject3D(o, "BSPEntity_" + std::to_string(i) + " (light)");
                }

                // func_button
                if (!strcmp(classname, "func_button")) {
                    Object3D *o = new Object3D();
                    o->setPosition( pos );
                    Brakeza3D::get()->addObject3D(o, "BSPEntity_" + std::to_string(i) + " (func_button)");
                }
            }
        }
    }
}
