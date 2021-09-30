#include "../../headers/Components/ComponentGame.h"
#include "../../headers/Components/ComponentCollisions.h"
#include "../../headers/Collisions/CollisionResolverBetweenProjectileAndBSPMap.h"
#include "../../headers/Collisions/CollisionResolverBetweenProjectileAndNPCEnemy.h"
#include "../../headers/Collisions/CollisionResolverBetweenCamera3DAndFuncDoor.h"
#include "../../headers/Collisions/CollisionResolverBetweenCamera3DAndFuncButton.h"
#include "../../headers/Collisions/CollisionResolverBetweenProjectileAndPlayer.h"
#include "../../headers/Collisions/CollisionResolverBetweenCamera3DAndItemWeapon.h"
#include "../../headers/Collisions/CollisionResolverBetweenCamera3DAndItemHealth.h"
#include "../../headers/Collisions/CollisionResolverBetweenCamera3DAndItemAmmo.h"
#include "../../headers/Collisions/CollisionResolverBetweenCamera3DAndTriggerMultiple.h"
#include "../../headers/Collisions/CollisionResolverBetweenCamera3DAndTriggerTeleport.h"

ComponentGame::ComponentGame() {
    player = new Player();
}

void ComponentGame::onStart() {
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

void ComponentGame::startFPS() {
    ComponentsManager::get()->getComponentCamera()->setIsFlyMode(true);

    LoadMapsFromJSON();
    setFirstMapNameFromJSON();

    ComponentsManager::get()->getComponentBSP()->initMap(currentMapName.c_str());

    LoadWeaponsJSON();
    ComponentsManager::get()->getComponentWeapons()->setCurrentWeaponIndex(EngineSetup::WeaponsTypes::PISTOL);
    ComponentsManager::get()->getComponentWeapons()->getCurrentWeaponType()->setWeaponAnimation(
            EngineSetup::WeaponsActions::IDLE);

    LoadEnemiesJSON();

    createObjects3DFromBSPEntities();   // enemies, items...
    createMesh3DAndGhostsFromHulls();   // collision triggers, doors, secret walls...
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
        Drawable::drawFireShader();
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

        if (collisionType == SETUP->CollisionResolverTypes::COLLISION_RESOLVER_PROJECTILE_AND_NPCENEMY) {
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

        if (collisionType == SETUP->CollisionResolverTypes::COLLISION_RESOLVER_CAMERA_AND_TRIGGER_MULTIPLE) {
            auto *resolver = new CollisionResolverBetweenCamera3DAndTriggerMultiple(
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

        if (collisionType == SETUP->CollisionResolverTypes::COLLISION_RESOLVER_CAMERA_AND_TRIGGER_TELEPORT) {
            auto *resolver = new CollisionResolverBetweenCamera3DAndTriggerTeleport(
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

        if (collisionType == SETUP->CollisionResolverTypes::COLLISION_RESOLVER_CAMERA_AND_FUNCDOOR) {
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

        if (collisionType == SETUP->CollisionResolverTypes::COLLISION_RESOLVER_CAMERA_AND_FUNCBUTTON) {
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

        if (collisionType == SETUP->CollisionResolverTypes::COLLISION_RESOLVER_PROJECTILE_AND_CAMERA) {
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

        if (collisionType == SETUP->CollisionResolverTypes::COLLISION_RESOLVER_ITEMWEAPON_AND_CAMERA) {
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

        if (collisionType == SETUP->CollisionResolverTypes::COLLISION_RESOLVER_ITEMHEALTH_AND_CAMERA) {
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

        if (collisionType == SETUP->CollisionResolverTypes::COLLISION_RESOLVER_ITEMAMMO_AND_CAMERA) {
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


void ComponentGame::createObjects3DFromBSPEntities() {
    EngineSetup *engineSetup = EngineSetup::getInstance();
    BSPMap *bspMap = ComponentsManager::get()->getComponentBSP()->getBSP();

    Logging::Log("BSP Num Entities: " + std::to_string(bspMap->n_entities), "");
    Brakeza3D *brakeza3D = Brakeza3D::get();

    if (bspMap->n_entities > MAX_BSP_ENTITIES) {
        printf("Error: Entities overflow");
        exit(-1);
    }

    for (int i = 0; i < bspMap->n_entities; i++) {
        Logging::Log("BSPEntity: " + std::to_string(bspMap->entities[i].id), "");
        for (int j = 0; j < bspMap->entities[i].num_attributes; j++) {
            Logging::Log(
                    "Key: '" + (std::string) bspMap->entities[i].attributes[j].key + "' - Value: '" +
                    (std::string) bspMap->entities[i].attributes[j].value + "'", "");
        }
    }

    // Create Objects3D from BSP Entities
    for (int i = 0; i < bspMap->n_entities; i++) {

        if (bspMap->hasEntityAttribute(i, "classname")) {
            char *classname = bspMap->getEntityValue(i, "classname");
            if (bspMap->hasEntityAttribute(i, "origin")) {
                char *value = bspMap->getEntityValue(i, "origin");
                Vertex3D pos = bspMap->parsePositionFromEntityAttribute(value);

                // light
                if (!strcmp(classname, "light")) {
                    auto *o = new Object3D();
                }

                // item cells
                if (!strcmp(classname, "item_cells")) {
                    auto *o = new ItemAmmoGhost();
                    ComponentWeapons *weaponManager = ComponentsManager::get()->getComponentWeapons();
                    ComponentCollisions *componentCollisions = ComponentsManager::get()->getComponentCollisions();

                    Mesh3D *oTemplate = weaponManager->getAmmoTypeByClassname(classname)->getModelBox();

                    o->setLabel("ammo_cells");
                    o->copyFrom(oTemplate);
                    // Md2 Import for Quake2 axis adjust
                    o->setRotation(M3::getMatrixRotationForEulerAngles(90, 0, 0));
                    o->setAmmoTypeClassname(classname);
                    o->setPosition(pos);
                    o->makeGhostBody(ComponentsManager::get()->getComponentCamera()->getCamera(),
                                     componentCollisions->getDynamicsWorld(), true, o);
                    Brakeza3D::get()->addObject3D(o, o->getLabel());
                }

                // item rockets
                if (!strcmp(classname, "item_rockets")) {
                    auto *o = new ItemAmmoGhost();
                    Mesh3D *oTemplate = brakeza3D->getComponentsManager()->getComponentWeapons()->getAmmoTypeByClassname(
                            classname)->getModelBox();
                    ComponentCollisions *componentCollisions = ComponentsManager::get()->getComponentCollisions();

                    o->setLabel("ammo_rockets");
                    o->copyFrom(oTemplate);
                    // Md2 Import for Quake2 axis adjust
                    o->setRotation(M3::getMatrixRotationForEulerAngles(90, 0, 0));
                    o->setAmmoTypeClassname(classname);
                    o->setPosition(pos);
                    o->makeGhostBody(ComponentsManager::get()->getComponentCamera()->getCamera(),
                                     componentCollisions->getDynamicsWorld(), true, o);
                    brakeza3D->addObject3D(o, o->getLabel());
                }

                // item shells
                if (!strcmp(classname, "item_shells")) {
                    auto *o = new ItemAmmoGhost();
                    Mesh3D *oTemplate = brakeza3D->getComponentsManager()->getComponentWeapons()->getAmmoTypeByClassname(
                            classname)->getModelBox();
                    ComponentCollisions *componentCollisions = ComponentsManager::get()->getComponentCollisions();

                    o->setLabel("ammo_shells");
                    o->copyFrom(oTemplate);
                    // Md2 Import for Quake2 axis adjust
                    o->setRotation(M3::getMatrixRotationForEulerAngles(90, 0, 0));
                    o->setAmmoTypeClassname(classname);
                    o->setPosition(pos);
                    o->makeGhostBody(ComponentsManager::get()->getComponentCamera()->getCamera(),
                                     componentCollisions->getDynamicsWorld(), true, o);
                    brakeza3D->addObject3D(o, o->getLabel());
                }

                // item spikes
                if (!strcmp(classname, "item_spikes")) {
                    auto *o = new ItemAmmoGhost();
                    Mesh3D *oTemplate = brakeza3D->getComponentsManager()->getComponentWeapons()->getAmmoTypeByClassname(
                            classname)->getModelBox();
                    ComponentCollisions *componentCollisions = ComponentsManager::get()->getComponentCollisions();

                    o->setLabel("ammo_spikes");
                    o->setAmmoTypeClassname(classname);
                    o->copyFrom(oTemplate);

                    // Md2 Import for Quake2 axis adjust
                    o->setRotation(M3::getMatrixRotationForEulerAngles(90, 0, 0));

                    o->setPosition(pos);
                    o->makeGhostBody(ComponentsManager::get()->getComponentCamera()->getCamera(),
                                     componentCollisions->getDynamicsWorld(), true, o);


                    brakeza3D->addObject3D(o, o->getLabel());
                }

                // item_health
                /*if (!strcmp(classname, "item_health")) {
                    auto *o = new ItemHealthGhost();
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
                    auto *o = new ItemWeaponGhost();
                    ComponentCollisions *componentCollisions = ComponentsManager::get()->getComponentCollisions();

                    WeaponType *weapon = brakeza3D->getComponentsManager()->getComponentWeapons()->getWeaponTypeByClassname(
                            classname);
                    if (weapon == nullptr) {
                        Logging::Log("Error loading weapon by classname: " + s1, "ERROR");
                        continue;
                    }

                    o->setPosition(pos);
                    o->setLabel(weapon->label);
                    o->setWeaponClassname(classname);
                    o->copyFrom(weapon->getModel());
                    // Md2 Import for Quake2 axis adjust
                    o->setRotation(M3::getMatrixRotationForEulerAngles(90, 0, 0));
                    o->makeGhostBody(ComponentsManager::get()->getComponentCamera()->getCamera(),
                                     componentCollisions->getDynamicsWorld(), true, o);
                    brakeza3D->addObject3D(o, o->getLabel());
                }

                // monster wildcard
                std::string s2(classname);
                if (s2.find("monster") != std::string::npos) {
                    NPCEnemyBody *enemyTemplate = EngineBuffers::getInstance()->getEnemyTemplateForClassname(classname);
                    if (enemyTemplate == nullptr) continue;

                    // Angle Monster
                    int angle = 0;
                    if (bspMap->hasEntityAttribute(i, "angle")) {
                        char *s_angle = bspMap->getEntityValue(i, "angle");
                        angle = std::stoi(s_angle);
                    }

                    M3 rotMonster = M3::getMatrixRotationForEulerAngles(0, 90 - (float) angle, 0);

                    auto *enemyBody = new NPCEnemyBody();
                    enemyBody->setScale(enemyTemplate->getScale());
                    enemyBody->rotationFixed = M3::getMatrixRotationForEulerAngles(90, 0, 0);
                    enemyBody->setPosition(pos);
                    enemyBody->setRotation(rotMonster);
                    enemyBody->setRespawnRotation(rotMonster);
                    enemyBody->setDrawOffset(enemyTemplate->getDrawOffset());
                    enemyBody->setBoxShapeSize(enemyTemplate->getBoxShapeSize());
                    enemyBody->setSpeed(enemyTemplate->getSpeed());
                    enemyBody->setRange(enemyTemplate->getRange());
                    enemyBody->setCadence(enemyTemplate->getCadence());
                    enemyBody->setLabel("BSPEntity_" + std::to_string(i) + " (monster)");
                    enemyBody->addAnimation("swat_idle", "Cubex.fbx", enemyTemplate->getScale(), false);
                    enemyBody->addAnimation("swat_walk", "Cubex.fbx", enemyTemplate->getScale(), false);
                    enemyBody->addAnimation("swat_fire", "Cubex.fbx", enemyTemplate->getScale(), false);
                    enemyBody->addAnimation("swat_injuried", "Cubex.fbx", enemyTemplate->getScale(), false);
                    enemyBody->addAnimation("swat_dead", "Cubex.fbx", enemyTemplate->getScale(), true);
                    enemyBody->setAnimation(EngineSetup::SOLDIER_IDLE);
                    enemyBody->makeSimpleRigidBody(
                            0,
                            pos - enemyBody->getDrawOffset(),
                            enemyBody->getBoxShapeSize(),
                            Brakeza3D::get()->getComponentsManager()->getComponentCollisions()->getDynamicsWorld()
                    );
                    brakeza3D->addObject3D(enemyBody, enemyBody->getLabel());
                }

                // armor wildcard
                std::string s3(classname);
                if (s2.find("armor") != std::string::npos) {
                    auto *o = new Object3D();
                    o->setPosition(pos);
                    Brakeza3D::get()->addObject3D(o, "BSPEntity_" + std::to_string(i) + " (armor)");
                }

                // info_player_start
                if (!strcmp(classname, "info_player_start") ||
                    !strcmp(classname, "info_player_coop") ||
                    !strcmp(classname, "info_player_deathmatch")
                        ) {
                    auto *o = new Object3D();
                    o->setPosition(pos);
                    Brakeza3D::get()->addObject3D(o, "BSPEntity_" + std::to_string(i) + " (player_spawn)");
                }

                // info teleport destination
                if (!strcmp(classname, "info_teleport_destination")) {
                    auto *o = new Object3D();
                    o->setPosition(pos);
                    Brakeza3D::get()->addObject3D(o, "BSPEntity_" + std::to_string(i) + " (teleport_destination)");
                }

                // light_flame_large_yellow
                if (!strcmp(classname, "light_flame_large_yellow") ||
                    !strcmp(classname, "light_torch_small_walltorch")) {
                    auto *o = new Object3D();
                    o->setPosition(pos);
                    Brakeza3D::get()->addObject3D(o, "BSPEntity_" + std::to_string(i) + " (light)");
                }

                // func_button
                if (!strcmp(classname, "func_button")) {
                    auto *o = new Object3D();
                    o->setPosition(pos);
                    Brakeza3D::get()->addObject3D(o, "BSPEntity_" + std::to_string(i) + " (func_button)");
                }
            }
        }
    }
}

void ComponentGame::LoadMapsFromJSON() {
    size_t file_size;
    const char *mapsFile = Tools::readFile(SETUP->CONFIG_FOLDER + SETUP->CFG_MAPS, file_size);
    cJSON *myDataJSON = cJSON_Parse(mapsFile);
    if (myDataJSON == nullptr) {
        //Logging::Log("maps.json can't be loaded", "ERROR");
        return;
    }

    cJSON *currentMap = nullptr;
    mapsJSONList = cJSON_GetObjectItemCaseSensitive(myDataJSON, "maps");
    int sizeMaps = cJSON_GetArraySize(mapsJSONList);

    if (sizeMaps <= 0) {
        //Logging::Log("maps.json is empty", "ERROR");
    }

    cJSON_ArrayForEach(currentMap, mapsJSONList) {
        cJSON *nameMap = cJSON_GetObjectItemCaseSensitive(currentMap, "name");

        if (cJSON_IsString(nameMap)) {
            //Logging::Log("Map JSON detected: " + std::string(nameMap->valuestring));
        }
    }
}

void ComponentGame::setFirstMapNameFromJSON() {
    // load first item
    cJSON *firstMapJSON = cJSON_GetArrayItem(mapsJSONList, 0);
    cJSON *nameMap = cJSON_GetObjectItemCaseSensitive(firstMapJSON, "name");
    this->currentMapName = nameMap->valuestring;
    Logging::Log(this->currentMapName, "FirstMap");
}

void ComponentGame::LoadWeaponsJSON() {
    ComponentWeapons *weaponManager = ComponentsManager::get()->getComponentWeapons();

    size_t file_size;
    std::string filePath = SETUP->CONFIG_FOLDER + SETUP->CFG_WEAPONS;
    const char *mapsFile = Tools::readFile(filePath, file_size);
    cJSON *myDataJSON = cJSON_Parse(mapsFile);

    if (myDataJSON == nullptr) {
        Logging::Log(filePath + " can't be loaded", "ERROR");
        return;
    }

    // Ammo Types
    ammoTypesJSONList = cJSON_GetObjectItemCaseSensitive(myDataJSON, "ammo");
    int sizeAmmoTypesList = cJSON_GetArraySize(ammoTypesJSONList);

    if (sizeAmmoTypesList > 0) {
        Logging::Log(filePath + " have " + std::to_string(sizeAmmoTypesList) + " ammoTypes", "WEAPONS");
    } else {
        Logging::Log(filePath + " is empty for ammoTypes", "ERROR");
    }

    cJSON *currentAmmoType;
    cJSON_ArrayForEach(currentAmmoType, ammoTypesJSONList) {
        cJSON *name = cJSON_GetObjectItemCaseSensitive(currentAmmoType, "name");
        cJSON *classname = cJSON_GetObjectItemCaseSensitive(currentAmmoType, "classname");
        cJSON *startAmmo = cJSON_GetObjectItemCaseSensitive(currentAmmoType, "start_ammo");
        cJSON *reloads = cJSON_GetObjectItemCaseSensitive(currentAmmoType, "reloads");
        cJSON *model_proj = cJSON_GetObjectItemCaseSensitive(currentAmmoType, "model_projectile");
        cJSON *scale_proj = cJSON_GetObjectItemCaseSensitive(currentAmmoType, "scale_projectile");
        cJSON *model_box = cJSON_GetObjectItemCaseSensitive(currentAmmoType, "model_box");
        cJSON *scale_box = cJSON_GetObjectItemCaseSensitive(currentAmmoType, "scale_box");

        auto *ammoType = new AmmoType();
        ammoType->setAmount(startAmmo->valueint);
        ammoType->setName(name->valuestring);
        ammoType->setClassname(classname->valuestring);
        ammoType->setReloads(reloads->valueint);
        ammoType->setReloadAmount(startAmmo->valueint);
        ammoType->getModelProjectile()->AssimpLoadAnimation(
                EngineSetup::getInstance()->MODELS_FOLDER + model_proj->valuestring);
        ammoType->getModelProjectile()->setScale((float)scale_proj->valuedouble);
        ammoType->getModelBox()->setLabel(std::string(name->valuestring) + "_model_box");
        ammoType->getModelBox()->setScale((float)scale_box->valuedouble);
        ammoType->getModelBox()->AssimpLoadGeometryFromFile(
                EngineSetup::getInstance()->MODELS_FOLDER + model_box->valuestring);

        weaponManager->ammoTypes.push_back(ammoType);

        Logging::Log("Loading ammoType: " + ammoType->getClassname(), "ERROR");
    }

    // Weapons Types
    weaponsJSONList = cJSON_GetObjectItemCaseSensitive(myDataJSON, "weapons");
    int sizeWeaponsList = cJSON_GetArraySize(weaponsJSONList);

    if (sizeWeaponsList > 0) {
        Logging::Log(filePath + " have " + std::to_string(sizeWeaponsList) + " weapons", "WEAPONS");
    } else {
        Logging::Log(filePath + " is empty for weapons", "ERROR");
    }

    // weapons loop
    cJSON *currentWeapon;
    cJSON_ArrayForEach(currentWeapon, weaponsJSONList) {
        cJSON *ammoIndex = cJSON_GetObjectItemCaseSensitive(currentWeapon, "ammo_index");
        cJSON *name = cJSON_GetObjectItemCaseSensitive(currentWeapon, "name");
        cJSON *classname = cJSON_GetObjectItemCaseSensitive(currentWeapon, "classname");
        cJSON *damage = cJSON_GetObjectItemCaseSensitive(currentWeapon, "damage");
        cJSON *cadence = cJSON_GetObjectItemCaseSensitive(currentWeapon, "cadence");
        cJSON *damageRadius = cJSON_GetObjectItemCaseSensitive(currentWeapon, "damage_radius");
        cJSON *speed = cJSON_GetObjectItemCaseSensitive(currentWeapon, "speed");
        cJSON *accuracy = cJSON_GetObjectItemCaseSensitive(currentWeapon, "accuracy");
        cJSON *dispersion = cJSON_GetObjectItemCaseSensitive(currentWeapon, "dispersion");
        cJSON *iconHUD = cJSON_GetObjectItemCaseSensitive(currentWeapon, "icon_hud");
        cJSON *sniper = cJSON_GetObjectItemCaseSensitive(currentWeapon, "sniper");
        cJSON *index = cJSON_GetObjectItemCaseSensitive(currentWeapon, "index");
        cJSON *model = cJSON_GetObjectItemCaseSensitive(currentWeapon, "model");
        cJSON *scale = cJSON_GetObjectItemCaseSensitive(currentWeapon, "scale");
        cJSON *sndEmptyLabel = cJSON_GetObjectItemCaseSensitive(currentWeapon, "sound_empty_label");

        Logging::Log("Loading weapon " + std::string(name->valuestring), "WEAPONS");

        // WeaponType attributes
        weaponManager->addWeaponType(name->valuestring);
        WeaponType *weaponType = weaponManager->getWeaponTypeByLabel(name->valuestring);
        weaponType->setAvailable(true);
        weaponType->setClassname(classname->valuestring);
        weaponType->setAmmoType(weaponManager->ammoTypes[ammoIndex->valueint]);
        weaponType->setIndex(index->valueint);
        weaponType->setDamage((float)damage->valuedouble);
        weaponType->setDamageRadius((float)damageRadius->valuedouble);
        weaponType->setSpeed((float) speed->valuedouble);
        weaponType->setAccuracy((float)accuracy->valuedouble);
        weaponType->setDispersion((float)dispersion->valueint);
        weaponType->loadIconHUD(std::string(name->valuestring) + "/" + std::string(iconHUD->valuestring));
        weaponType->getModel()->setLabel(std::string(name->valuestring) + "_model");
        weaponType->getModel()->setScale((float)scale->valuedouble);
        weaponType->getModel()->AssimpLoadGeometryFromFile(
                EngineSetup::getInstance()->MODELS_FOLDER + model->valuestring);
        weaponType->counterCadence->setStep((float)cadence->valuedouble);
        weaponType->setSoundEmptyLabel(sndEmptyLabel->valuestring);

        // WeaponType SniperHUD
        weaponManager->getWeaponTypeByLabel(name->valuestring)->setSniper(sniper->valueint);
        if (sniper->valueint) {
            cJSON *sniperHUD = cJSON_GetObjectItemCaseSensitive(currentWeapon, "sniper_hud");
            weaponManager->getWeaponTypeByLabel(name->valuestring)->loadSniperHUD(sniperHUD->valuestring);
        }

        // animation's weapon loop
        cJSON *weaponAnimationsJSONList;
        weaponAnimationsJSONList = cJSON_GetObjectItemCaseSensitive(currentWeapon, "animations");

        cJSON *currentWeaponAnimation;
        cJSON_ArrayForEach(currentWeaponAnimation, weaponAnimationsJSONList) {
            cJSON *status = cJSON_GetObjectItemCaseSensitive(currentWeaponAnimation, "status");
            cJSON *label = cJSON_GetObjectItemCaseSensitive(currentWeaponAnimation, "label");
            cJSON *animationModel = cJSON_GetObjectItemCaseSensitive(currentWeaponAnimation, "animationModel");
            cJSON *animationScale = cJSON_GetObjectItemCaseSensitive(currentWeaponAnimation, "animationScale");
            cJSON *stopEnd = cJSON_GetObjectItemCaseSensitive(currentWeaponAnimation, "stop_end");
            cJSON *looping = cJSON_GetObjectItemCaseSensitive(currentWeaponAnimation, "looping");
            cJSON *projectile = cJSON_GetObjectItemCaseSensitive(currentWeaponAnimation, "projectile");
            cJSON *sound = cJSON_GetObjectItemCaseSensitive(currentWeaponAnimation, "sound");

            Logging::Log(
                    "Loading JSON Weapon Animation: " + std::string(label->valuestring) + ", status:" +
                    std::to_string(status->valueint), "WeaponLoading");

            weaponManager->getWeaponTypeByLabel(name->valuestring)->addAnimation(
                    std::string(label->valuestring),
                    std::string(animationModel->valuestring),
                    (float) animationScale->valuedouble,
                    stopEnd->valueint
            );

            weaponManager->getWeaponTypeByLabel(name->valuestring)->getWeaponSounds().emplace_back(sound->valuestring);
            weaponManager->getWeaponTypeByLabel(name->valuestring)->setWeaponAnimation(0);
        }
    }

    weaponManager->setCurrentWeaponIndex(EngineSetup::WeaponsTypes::EMPTY);
}


void ComponentGame::LoadEnemiesJSON() {
    size_t file_size;
    std::string filePath = SETUP->CONFIG_FOLDER + SETUP->CFG_ENEMIES;
    const char *enemiesFile = Tools::readFile(filePath, file_size);
    cJSON *myDataJSON = cJSON_Parse(enemiesFile);

    if (myDataJSON == nullptr) {
        Logging::Log(filePath + " can't be loaded", "ERROR");
        return;
    }

    enemiesJSONList = cJSON_GetObjectItemCaseSensitive(myDataJSON, "enemies");
    int sizeWeaponsList = cJSON_GetArraySize(enemiesJSONList);

    if (sizeWeaponsList <= 0) {
        Logging::Log(filePath + " is empty", "ERROR");
    }

    // weapons loop
    cJSON *currentEnemy;
    cJSON_ArrayForEach(currentEnemy, enemiesJSONList) {
        cJSON *name = cJSON_GetObjectItemCaseSensitive(currentEnemy, "name");
        cJSON *classname = cJSON_GetObjectItemCaseSensitive(currentEnemy, "classname");
        cJSON *range = cJSON_GetObjectItemCaseSensitive(currentEnemy, "range");
        cJSON *damage = cJSON_GetObjectItemCaseSensitive(currentEnemy, "damage");
        cJSON *cadence = cJSON_GetObjectItemCaseSensitive(currentEnemy, "cadence");
        cJSON *speed = cJSON_GetObjectItemCaseSensitive(currentEnemy, "speed");
        cJSON *soundFire = cJSON_GetObjectItemCaseSensitive(currentEnemy, "fire_sound");
        cJSON *defaultAnimation = cJSON_GetObjectItemCaseSensitive(currentEnemy, "default_animation");
        cJSON *scale = cJSON_GetObjectItemCaseSensitive(currentEnemy, "scale_mesh");

        auto *newEnemy = new NPCEnemyBody();
        newEnemy->setDamage((float) damage->valuedouble);
        newEnemy->setCadence((float) cadence->valuedouble);
        newEnemy->setRange((float) range->valuedouble);
        newEnemy->setSpeed((float) speed->valuedouble);
        newEnemy->setPosition(Vertex3D(0, 0, 0));
        newEnemy->setRotation(M3());
        newEnemy->setLabel(name->valuestring);
        newEnemy->setClassname(classname->valuestring);
        newEnemy->setScale(scale->valuedouble);

        cJSON *draw_offset = cJSON_GetObjectItemCaseSensitive(currentEnemy, "draw_offset");
        cJSON *draw_offset_x = cJSON_GetObjectItemCaseSensitive(draw_offset, "x");
        cJSON *draw_offset_y = cJSON_GetObjectItemCaseSensitive(draw_offset, "y");
        cJSON *draw_offset_z = cJSON_GetObjectItemCaseSensitive(draw_offset, "z");
        newEnemy->setDrawOffset(Vertex3D(
                draw_offset_x->valuedouble,
                draw_offset_y->valuedouble,
                draw_offset_z->valuedouble
        ));

        cJSON *box_shape_size = cJSON_GetObjectItemCaseSensitive(currentEnemy, "box_shape_size");
        cJSON *box_shape_size_x = cJSON_GetObjectItemCaseSensitive(box_shape_size, "x");
        cJSON *box_shape_size_y = cJSON_GetObjectItemCaseSensitive(box_shape_size, "y");
        cJSON *box_shape_size_z = cJSON_GetObjectItemCaseSensitive(box_shape_size, "z");
        newEnemy->setBoxShapeSize(Vertex3D(
                box_shape_size_x->valuedouble,
                box_shape_size_y->valuedouble,
                box_shape_size_z->valuedouble
        ));

        // animation's enemy loop
        cJSON *enemyAnimationsJSONList = cJSON_GetObjectItemCaseSensitive(currentEnemy, "animations");
        cJSON *currentEnemyAnimation;
        cJSON_ArrayForEach(currentEnemyAnimation, enemyAnimationsJSONList) {
            cJSON *path = cJSON_GetObjectItemCaseSensitive(currentEnemyAnimation, "path");
            cJSON *label = cJSON_GetObjectItemCaseSensitive(currentEnemyAnimation, "label");
            cJSON *remove_at_end = cJSON_GetObjectItemCaseSensitive(currentEnemyAnimation, "remove_at_end");

            newEnemy->addAnimation(label->valuestring, path->valuestring, newEnemy->getScale(),
                                   remove_at_end->valueint);
        }
        newEnemy->setAnimation(defaultAnimation->valueint);
        BUFFERS->enemyTemplates.push_back(newEnemy);
    }
}

void ComponentGame::createMesh3DAndGhostsFromHulls() {
    BSPMap *mapBSP = ComponentsManager::get()->getComponentBSP()->getBSP();

    int numHulls = mapBSP->getNumModels();

    for (int m = 1; m < numHulls; m++) {
        model_t *hull = mapBSP->getModel(m);
        int indexModel = m;

        std::string targetName = "*" + std::to_string(indexModel);
        int entityIndex = mapBSP->getIndexOfFirstEntityByModel(targetName.c_str());

        if (entityIndex >= 1) {
            char *classname = mapBSP->getEntityValue(entityIndex, "classname");
            if (!strcmp(classname, "func_door")) {
                this->makeDoorGhost(m, entityIndex, true, hull);
                continue;
            }
        }

        this->makeMesh3DGhost(m, entityIndex, false, hull);
    }
}

void ComponentGame::makeDoorGhost(int indexModel, int entityIndex, bool enabled, model_t *hull) {
    BSPMap *mapBSP = ComponentsManager::get()->getComponentBSP()->getBSP();
    Camera3D *camera = ComponentsManager::get()->getComponentCamera()->getCamera();

    // Buscamos entidades que figuren con el modelo

    auto *ghost = new DoorGhost();
    ghost->setEnabled(enabled);

    if (entityIndex >= 1) {
        ghost->setBspEntityIndex(entityIndex);
    }

    ghost->setPosition(mapBSP->getPosition());
    ghost->setRotation(mapBSP->getRotation());

    getTrianglesHull(ghost, hull);

    ghost->makeGhostBody(camera, Brakeza3D::get()->getComponentsManager()->getComponentCollisions()->getDynamicsWorld(),
                         true, ghost);
    ghost->getGhostObject()->setUserPointer(ghost);
    Brakeza3D::get()->addObject3D(ghost, "hull_" + std::to_string(indexModel) + " (DoorGhost)");
}

void ComponentGame::makeMesh3DGhost(int indexModel, int entityIndex, bool enabled, model_t *hull) {
    BSPMap *mapBSP = ComponentsManager::get()->getComponentBSP()->getBSP();
    Camera3D *camera = ComponentsManager::get()->getComponentCamera()->getCamera();

    // Buscamos entidades que figuren con el modelo

    auto *ghost = new Mesh3DGhost();
    if (entityIndex >= 1) {
        ghost->setBspEntityIndex(entityIndex);
    }
    ghost->setEnabled(enabled);
    ghost->setPosition(mapBSP->getPosition());
    ghost->setRotation(mapBSP->getRotation());

    getTrianglesHull(ghost, hull);

    ghost->makeGhostBody(camera, Brakeza3D::get()->getComponentsManager()->getComponentCollisions()->getDynamicsWorld(),
                         true, ghost);
    ghost->getGhostObject()->setUserPointer(ghost);
    Brakeza3D::get()->addObject3D(ghost, "hull_" + std::to_string(indexModel) + " (Mesh3DGhost)");
}


void ComponentGame::getTrianglesHull(Mesh3DGhost *mesh, model_t *hull) {
    BSPMap *mapBSP = ComponentsManager::get()->getComponentBSP()->getBSP();

    for (int surface = hull->firstsurf; surface < hull->firstsurf + hull->numsurf; surface++) {

        const int offset = mapBSP->surface_triangles[surface].offset;
        const int num = mapBSP->surface_triangles[surface].num;

        for (int i = offset; i < offset + num; i++) {
            Triangle *t = mapBSP->model_triangles[i];
            t->parent = mesh;
            mesh->modelTriangles.push_back(t);
        }
    }

    mesh->updateBoundingBox();
}

