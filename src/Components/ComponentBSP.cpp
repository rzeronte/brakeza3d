//
// Created by darkhead on 9/1/20.
//

#include "../../headers/Components/ComponentBSP.h"
#include "../../headers/Components/ComponentWeapons.h"
#include "../../headers/Game/NPCEnemyBody.h"
#include "../../headers/EngineBuffers.h"
#include "../../headers/ComponentsManager.h"

ComponentBSP::ComponentBSP()
{
    bsp = new BSPMap();
}

void ComponentBSP::onStart()
{
    std::cout << "ComponentBSP onStart" << std::endl;

    loadMapsFromJSON();
    loadEnemiesJSON();
    loadWeaponsJSON();

    cJSON *firstMap = cJSON_GetArrayItem(mapsJSONList, 0);
    cJSON *nameMap  = cJSON_GetObjectItemCaseSensitive(firstMap, "name");
    bsp->Initialize(nameMap->valuestring, "palette.lmp", camera);
    setCameraInBSPStartPosition();
}

void ComponentBSP::preUpdate()
{
    if ( bsp->isLoaded() ) {
        bspleaf_t *leaf = bsp->FindLeaf( *camera->getPosition(), true );
        bsp->setVisibleSet( leaf );
    }
}

void ComponentBSP::onUpdate()
{

}

void ComponentBSP::postUpdate()
{

}

void ComponentBSP::onEnd()
{

}

void ComponentBSP::onSDLPollEvent(SDL_Event *event, bool &finish)
{

}

BSPMap *ComponentBSP::getBSP() const {
    return bsp;
}

void ComponentBSP::loadMapsFromJSON()
{
    size_t file_size;
    const char* mapsFile = Tools::readFile(SETUP->CONFIG_FOLDER + SETUP->CFG_MAPS, file_size);
    cJSON *myDataJSON = cJSON_Parse(mapsFile);
    if (myDataJSON == NULL) {
        //Logging::getInstance()->Log("maps.json can't be loaded", "ERROR");
        return;
    }

    cJSON *currentMap = NULL;
    mapsJSONList = cJSON_GetObjectItemCaseSensitive(myDataJSON, "maps" );
    int sizeMaps = cJSON_GetArraySize(mapsJSONList);

    if (sizeMaps <= 0) {
        //Logging::getInstance()->Log("maps.json is empty", "ERROR");
    }

    cJSON_ArrayForEach(currentMap, mapsJSONList) {
        cJSON *nameMap = cJSON_GetObjectItemCaseSensitive(currentMap, "name");

        if (cJSON_IsString(nameMap)) {
            //Logging::getInstance()->Log("Map JSON detected: " + std::string(nameMap->valuestring));
        }
    }
}

void ComponentBSP::initParallelBSP(const char *bspFilename, std::vector<Triangle*> *frameTriangles)
{
    //this->loadingBSP = new std::thread(ParallellInitBSP, bspFilename, frameTriangles);
}


void ComponentBSP::setCameraInBSPStartPosition()
{
    // Load start position from BSP
    Vertex3D bspOriginalPosition = getBSP()->getStartMapPosition();

    int entityID = getBSP()->getIndexOfFirstEntityByClassname("info_player_start");
    btTransform initialTransform;
    initialTransform.setOrigin( btVector3(bspOriginalPosition.x, bspOriginalPosition.y, bspOriginalPosition.z) );

    char *angle = getBSP()->getEntityValue(entityID, "angle");
    int angleInt = atoi( std::string(angle).c_str() );

    camera->yaw   = 90 - angleInt;
    camera->pitch = 0;
    camera->roll  = 0;

    btCollisionObject *a;
    camera->setPosition( bspOriginalPosition );

    btPairCachingGhostObject *kinematicGhost = camera->kinematicController->getGhostObject();
    kinematicGhost->setWorldTransform(initialTransform);
}

void ComponentBSP::loadWeaponsJSON()
{
    ComponentWeapons *weaponManager = ComponentsManager::get()->getComponentWeapons();

    size_t file_size;
    std::string filePath = SETUP->CONFIG_FOLDER + SETUP->CFG_WEAPONS;
    const char* mapsFile = Tools::readFile(filePath, file_size);
    cJSON *myDataJSON = cJSON_Parse(mapsFile);

    if (myDataJSON == NULL) {
        Logging::getInstance()->Log(filePath + " can't be loaded", "ERROR");
        return;
    }

    // Ammo Types
    ammoTypesJSONList = cJSON_GetObjectItemCaseSensitive(myDataJSON, "ammo" );
    int sizeAmmoTypesList = cJSON_GetArraySize(ammoTypesJSONList);

    if (sizeAmmoTypesList > 0) {
        Logging::getInstance()->Log(filePath + " have " + std::to_string(sizeAmmoTypesList) + " ammoTypes", "WEAPONS");
    } else {
        Logging::getInstance()->Log(filePath + " is empty for ammoTypes", "ERROR");
    }

    cJSON *currentAmmoType;
    cJSON_ArrayForEach(currentAmmoType, ammoTypesJSONList) {
        cJSON *name       = cJSON_GetObjectItemCaseSensitive(currentAmmoType, "name");
        cJSON *classname  = cJSON_GetObjectItemCaseSensitive(currentAmmoType, "classname");
        cJSON *startAmmo  = cJSON_GetObjectItemCaseSensitive(currentAmmoType, "start_ammo");

        auto* ammoType = new AmmoType();
        ammoType->setAmount(startAmmo->valueint);
        ammoType->setName(name->valuestring);
        ammoType->setClassname(classname->valuestring);
        weaponManager->ammoTypes.push_back(ammoType);
        Logging::getInstance()->Log("Loading ammoType: " + ammoType->getClassname(), "ERROR");

    }

    // Weapons Types
    weaponsJSONList = cJSON_GetObjectItemCaseSensitive(myDataJSON, "weapons" );
    int sizeWeaponsList = cJSON_GetArraySize(weaponsJSONList);

    if (sizeWeaponsList > 0) {
        Logging::getInstance()->Log(filePath + " have " + std::to_string(sizeWeaponsList) + " weapons", "WEAPONS");
    } else {
        Logging::getInstance()->Log(filePath + " is empty for weapons", "ERROR");
    }

    // weapons loop
    cJSON *currentWeapon;
    cJSON_ArrayForEach(currentWeapon, weaponsJSONList) {
        cJSON *ammoIndex     = cJSON_GetObjectItemCaseSensitive(currentWeapon, "ammo_index");
        cJSON *name          = cJSON_GetObjectItemCaseSensitive(currentWeapon, "name");
        cJSON *classname     = cJSON_GetObjectItemCaseSensitive(currentWeapon, "classname");
        cJSON *billboardTex  = cJSON_GetObjectItemCaseSensitive(currentWeapon, "billboard_texture");
        cJSON *billboardW    = cJSON_GetObjectItemCaseSensitive(currentWeapon, "billboard_width");
        cJSON *billboardH    = cJSON_GetObjectItemCaseSensitive(currentWeapon, "billboard_height");
        cJSON *damage        = cJSON_GetObjectItemCaseSensitive(currentWeapon, "damage");
        cJSON *damageRadius  = cJSON_GetObjectItemCaseSensitive(currentWeapon, "damage_radius");
        cJSON *speed         = cJSON_GetObjectItemCaseSensitive(currentWeapon, "speed");
        cJSON *projectileW   = cJSON_GetObjectItemCaseSensitive(currentWeapon, "projectile_width");
        cJSON *projectileH   = cJSON_GetObjectItemCaseSensitive(currentWeapon, "projectile_height");
        cJSON *soundMark     = cJSON_GetObjectItemCaseSensitive(currentWeapon, "mark_sound");
        cJSON *accuracy      = cJSON_GetObjectItemCaseSensitive(currentWeapon, "accuracy");
        cJSON *dispersion    = cJSON_GetObjectItemCaseSensitive(currentWeapon, "dispersion");
        cJSON *iconHUD       = cJSON_GetObjectItemCaseSensitive(currentWeapon, "icon_hud");
        cJSON *sniper        = cJSON_GetObjectItemCaseSensitive(currentWeapon, "sniper");
        cJSON *casingTemp1   = cJSON_GetObjectItemCaseSensitive(currentWeapon, "casing1_sound");
        cJSON *casingTemp2   = cJSON_GetObjectItemCaseSensitive(currentWeapon, "casing2_sound");
        cJSON *casingTemp3   = cJSON_GetObjectItemCaseSensitive(currentWeapon, "casing3_sound");
        cJSON *index         = cJSON_GetObjectItemCaseSensitive(currentWeapon, "index");

        Logging::getInstance()->Log("Loading weapon " + std::string(name->valuestring), "WEAPONS");

        cJSON *keyDownHandle = cJSON_GetObjectItemCaseSensitive(currentWeapon, "key_down_handle");
        cJSON *keyUpHandle   = cJSON_GetObjectItemCaseSensitive(currentWeapon, "key_up_handle");

        cJSON *keyDownAnimationStatus = cJSON_GetObjectItemCaseSensitive(currentWeapon, "key_down_animation_status");
        cJSON *keyUpAnimationStatus   = cJSON_GetObjectItemCaseSensitive(currentWeapon, "key_up_animation_status");

        // WeaponType attributes
        weaponManager->addWeaponType(name->valuestring);
        weaponManager->getWeaponTypeByLabel(name->valuestring)->setAvailable( true );
        weaponManager->getWeaponTypeByLabel(name->valuestring)->setClassname( classname->valuestring );
        weaponManager->getWeaponTypeByLabel(name->valuestring)->setAmmoType(weaponManager->ammoTypes[ammoIndex->valueint]);
        weaponManager->getWeaponTypeByLabel(name->valuestring)->setIndex( index->valueint );
        weaponManager->getWeaponTypeByLabel(name->valuestring)->setBillboardTextureFile( billboardTex->valuestring );
        weaponManager->getWeaponTypeByLabel(name->valuestring)->setBillboardDimensions( billboardW->valuedouble, billboardH->valuedouble );
        weaponManager->getWeaponTypeByLabel(name->valuestring)->setDamage( damage->valuedouble );
        weaponManager->getWeaponTypeByLabel(name->valuestring)->setDamageRadius( damageRadius->valuedouble );
        weaponManager->getWeaponTypeByLabel(name->valuestring)->setSpeed( (float) speed->valuedouble );
        weaponManager->getWeaponTypeByLabel(name->valuestring)->setProjectileSize( projectileW->valuedouble, projectileH->valuedouble );
        weaponManager->getWeaponTypeByLabel(name->valuestring)->loadMarkSound( soundMark->valuestring );
        weaponManager->getWeaponTypeByLabel(name->valuestring)->loadCasingSound( casingTemp1->valuestring, 1 );
        weaponManager->getWeaponTypeByLabel(name->valuestring)->loadCasingSound( casingTemp2->valuestring, 2 );
        weaponManager->getWeaponTypeByLabel(name->valuestring)->loadCasingSound( casingTemp3->valuestring, 3 );
        weaponManager->getWeaponTypeByLabel(name->valuestring)->setAccuracy( accuracy->valuedouble );
        weaponManager->getWeaponTypeByLabel(name->valuestring)->setDispersion( dispersion->valueint );
        weaponManager->getWeaponTypeByLabel(name->valuestring)->loadIconHUD(std::string(name->valuestring) + "/" + std::string(iconHUD->valuestring));

        // WeaponType Keyboard Events
        weaponManager->getWeaponTypeByLabel(name->valuestring)->setKeyDownHandle(keyDownHandle->valueint );
        weaponManager->getWeaponTypeByLabel(name->valuestring)->setKeyUpHandle(keyUpHandle->valueint );
        weaponManager->getWeaponTypeByLabel(name->valuestring)->setKeyDownAnimationStatus( keyDownAnimationStatus->valueint );
        weaponManager->getWeaponTypeByLabel(name->valuestring)->setKeyUpAnimationStatus( keyUpAnimationStatus->valueint) ;

        // WeaponType SniperHUD
        weaponManager->getWeaponTypeByLabel(name->valuestring)->setSniper( sniper->valueint );
        if (sniper->valueint) {
            cJSON *sniperHUD = cJSON_GetObjectItemCaseSensitive(currentWeapon, "sniper_hud");
            weaponManager->getWeaponTypeByLabel(name->valuestring)->loadSniperHUD( sniperHUD->valuestring );
        }

        cJSON *fireMark   = cJSON_GetObjectItemCaseSensitive(currentWeapon, "fire_mark");
        cJSON *markPath   = cJSON_GetObjectItemCaseSensitive(fireMark, "path");
        cJSON *markFrames = cJSON_GetObjectItemCaseSensitive(fireMark, "frames");
        cJSON *markFps    = cJSON_GetObjectItemCaseSensitive(fireMark, "fps");
        cJSON *markW      = cJSON_GetObjectItemCaseSensitive(fireMark, "width");
        cJSON *markH      = cJSON_GetObjectItemCaseSensitive(fireMark, "height");

        Logging::getInstance()->Log("Creating weapon mark billboard for " + std::string(name->valuestring), "WEAPONS");

        weaponManager->getWeaponTypeByLabel(name->valuestring)->setupMarkTemplate(
                SETUP->WEAPONS_FOLDER + name->valuestring + "/" + markPath->valuestring,
                markFrames->valueint,
                markFps->valueint,
                (float) markW->valuedouble,
                (float) markH->valuedouble
        );

        // animation's weapon loop
        cJSON *weaponAnimationsJSONList;
        weaponAnimationsJSONList = cJSON_GetObjectItemCaseSensitive(currentWeapon, "animations" );

        cJSON *currentWeaponAnimation;
        cJSON_ArrayForEach(currentWeaponAnimation, weaponAnimationsJSONList) {
            cJSON *status     = cJSON_GetObjectItemCaseSensitive(currentWeaponAnimation, "status");
            cJSON *subfolder  = cJSON_GetObjectItemCaseSensitive(currentWeaponAnimation, "subfolder");
            cJSON *frames     = cJSON_GetObjectItemCaseSensitive(currentWeaponAnimation, "frames");
            cJSON *fps        = cJSON_GetObjectItemCaseSensitive(currentWeaponAnimation, "fps");
            cJSON *offsetX    = cJSON_GetObjectItemCaseSensitive(currentWeaponAnimation, "offsetX");
            cJSON *offsetY    = cJSON_GetObjectItemCaseSensitive(currentWeaponAnimation, "offsetY");
            cJSON *right      = cJSON_GetObjectItemCaseSensitive(currentWeaponAnimation, "right");
            cJSON *stopEnd    = cJSON_GetObjectItemCaseSensitive(currentWeaponAnimation, "stop_end");
            cJSON *time       = cJSON_GetObjectItemCaseSensitive(currentWeaponAnimation, "time");
            cJSON *sound      = cJSON_GetObjectItemCaseSensitive(currentWeaponAnimation, "sound");
            cJSON *looping    = cJSON_GetObjectItemCaseSensitive(currentWeaponAnimation, "looping");
            cJSON *next       = cJSON_GetObjectItemCaseSensitive(currentWeaponAnimation, "next");
            cJSON *projectile = cJSON_GetObjectItemCaseSensitive(currentWeaponAnimation, "projectile");

            Logging::getInstance()->Log("Loading JSON Weapon Animation: " + std::string(subfolder->valuestring) + ", status:" + std::to_string(status->valueint) +", next: " + std::to_string(next->valueint));

            weaponManager->getWeaponTypeByLabel(name->valuestring)->addAnimation(
                    std::string(name->valuestring) + "/" + std::string(subfolder->valuestring),
                    frames->valueint,
                    fps->valueint,
                    offsetX->valueint,
                    offsetY->valueint,
                    right->valueint,
                    stopEnd->valueint,
                    next->valueint,
                    looping->valueint,
                    projectile->valueint
            );

            Counter animationCounter = Counter();
            animationCounter.setStep( time->valuedouble );
            animationCounter.setEnabled( false );
            weaponManager->getWeaponTypeByLabel(name->valuestring)->fireCounters.push_back(animationCounter);

            Logging::getInstance()->Log("Adding fire animation timer with step: " + std::to_string(time->valuedouble));

            Mix_Chunk *animationSound = new Mix_Chunk();

            std::string pathSound = SETUP->WEAPONS_FOLDER + name->valuestring + "/sounds/" + sound->valuestring;

            if (strlen(sound->valuestring) > 0) {
                Logging::getInstance()->Log("Loading Fire Phases sound: " + pathSound);
                animationSound = Mix_LoadWAV( (pathSound).c_str() );
            } else {
                Logging::getInstance()->Log("Loading Fire Phases sound: Empty");
            }

            weaponManager->getWeaponTypeByLabel(name->valuestring)->fireSounds.push_back(animationSound);
        }
    }
}

void ComponentBSP::loadEnemiesJSON()
{
    size_t file_size;
    std::string filePath = SETUP->CONFIG_FOLDER + SETUP->CFG_ENEMIES;
    const char* enemiesFile = Tools::readFile(filePath, file_size);
    cJSON *myDataJSON = cJSON_Parse(enemiesFile);

    if (myDataJSON == NULL) {
        Logging::getInstance()->Log(filePath + " can't be loaded", "ERROR");
        return;
    }

    enemiesJSONList = cJSON_GetObjectItemCaseSensitive(myDataJSON, "enemies" );
    int sizeWeaponsList = cJSON_GetArraySize(enemiesJSONList);

    if (sizeWeaponsList > 0) {
        Logging::getInstance()->Log(filePath + " have " + std::to_string(sizeWeaponsList));
    } else {
        Logging::getInstance()->Log(filePath + " is empty", "ERROR");
    }

    // weapons loop
    cJSON *currentEnemy;
    cJSON_ArrayForEach(currentEnemy, enemiesJSONList) {
        cJSON *name             = cJSON_GetObjectItemCaseSensitive(currentEnemy, "name");
        cJSON *classname        = cJSON_GetObjectItemCaseSensitive(currentEnemy, "classname");
        cJSON *range            = cJSON_GetObjectItemCaseSensitive(currentEnemy, "range");
        cJSON *damage           = cJSON_GetObjectItemCaseSensitive(currentEnemy, "damage");
        cJSON *cadence          = cJSON_GetObjectItemCaseSensitive(currentEnemy, "cadence");
        cJSON *speed            = cJSON_GetObjectItemCaseSensitive(currentEnemy, "speed");
        cJSON *width            = cJSON_GetObjectItemCaseSensitive(currentEnemy, "width");
        cJSON *height           = cJSON_GetObjectItemCaseSensitive(currentEnemy, "height");
        cJSON *projectileW      = cJSON_GetObjectItemCaseSensitive(currentEnemy, "projectile_width");
        cJSON *projectileH      = cJSON_GetObjectItemCaseSensitive(currentEnemy, "projectile_height");
        cJSON *soundFire        = cJSON_GetObjectItemCaseSensitive(currentEnemy, "fire_sound");
        cJSON *soundMark        = cJSON_GetObjectItemCaseSensitive(currentEnemy, "mark_sound");
        cJSON *defaultAnimation = cJSON_GetObjectItemCaseSensitive(currentEnemy, "default_animation");

        NPCEnemyBody *newEnemy = new NPCEnemyBody();
        newEnemy->setDamage( (float) damage->valuedouble );
        newEnemy->setCadence( (float) cadence->valuedouble );
        newEnemy->setRange( (float) range->valuedouble );
        newEnemy->setSpeed( (float) speed->valuedouble );
        newEnemy->setPosition(Vertex3D(0, 0, 0) );
        newEnemy->setRotation(M3() );
        newEnemy->getBillboard()->setDimensions( (float) width->valuedouble, (float) height->valuedouble );
        newEnemy->setLabel( name->valuestring);
        newEnemy->setClassname( classname->valuestring );
        newEnemy->setAnimation( defaultAnimation->valueint );


        /*Logging::getInstance()->Log("Enemy JSON detected: name: " + std::string(name->valuestring) +
                                    ", classname: " + classname->valuestring +
                                    ", speed: " + std::to_string(speed->valuedouble) +
                                    ", w: " + std::to_string(width->valuedouble) +
                                    ", h: " + std::to_string(height->valuedouble)
        );*/

        // animation's enemy loop
        cJSON *enemyAnimationsJSONList = cJSON_GetObjectItemCaseSensitive(currentEnemy, "animations" );

        cJSON *currentEnemyAnimation;
        cJSON_ArrayForEach(currentEnemyAnimation, enemyAnimationsJSONList) {
            cJSON *path           = cJSON_GetObjectItemCaseSensitive(currentEnemyAnimation, "path");
            cJSON *frames         = cJSON_GetObjectItemCaseSensitive(currentEnemyAnimation, "frames");
            cJSON *fps            = cJSON_GetObjectItemCaseSensitive(currentEnemyAnimation, "fps");
            cJSON *zeroDirection  = cJSON_GetObjectItemCaseSensitive(currentEnemyAnimation, "zeroDirection");
            cJSON *maxTimes       = cJSON_GetObjectItemCaseSensitive(currentEnemyAnimation, "maxTimes");

            /*Logging::getInstance()->Log("Reading JSON Enemy Animation: " + std::string(path->valuestring)
                                        + ", maxTimes: " + std::to_string(maxTimes->valueint)
            );*/

            newEnemy->addAnimationDirectional2D(
                    SETUP->SPRITES_FOLDER + path->valuestring,
                    frames->valueint,
                    fps->valueint,
                    zeroDirection->valueint,
                    maxTimes->valueint
            );
        }

        BUFFERS->enemyTemplates.push_back(newEnemy);
    }
}

void ComponentBSP::setCamera(Camera3D *camera) {
    ComponentBSP::camera = camera;
}
