//
// Created by darkhead on 9/1/20.
//

#include "../../headers/Components/ComponentBSP.h"
#include "../../headers/Components/ComponentWeapons.h"
#include "../../headers/ComponentsManager.h"

ComponentBSP::ComponentBSP()
{
    bsp = new BSPMap();
}

void ComponentBSP::onStart()
{
    std::cout << "ComponentBSP onStart" << std::endl;

    loadMapsFromJSON();

    //loadWeaponsJSON();
    //loadEnemiesJSON();

    cJSON *firstMap = cJSON_GetArrayItem(mapsJSONList, 0);
    cJSON *nameMap  = cJSON_GetObjectItemCaseSensitive(firstMap, "name");
    bsp->Initialize(nameMap->valuestring, "palette.lmp", camera);
    setCameraInBSPStartPosition();

    bspCollider = new BSPCollider();
}

void ComponentBSP::preUpdate()
{
    if ( bsp->isLoaded() ) {
        dleaf_t *leaf = bsp->FindLeaf(camera->getPosition(), true );
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

    camera->setPosition( bspOriginalPosition );
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
        cJSON *reloads    = cJSON_GetObjectItemCaseSensitive(currentAmmoType, "reloads");
        cJSON *model_proj = cJSON_GetObjectItemCaseSensitive(currentAmmoType, "model_projectile");
        cJSON *scale_proj = cJSON_GetObjectItemCaseSensitive(currentAmmoType, "scale_projectile");
        cJSON *model_box  = cJSON_GetObjectItemCaseSensitive(currentAmmoType, "model_box");
        cJSON *scale_box  = cJSON_GetObjectItemCaseSensitive(currentAmmoType, "scale_box");

        auto* ammoType = new AmmoType();
        ammoType->setAmount(startAmmo->valueint);
        ammoType->setName(name->valuestring);
        ammoType->setClassname(classname->valuestring);
        ammoType->setReloads(reloads->valueint);
        ammoType->setReloadAmount( startAmmo->valueint );
        ammoType->getModelProjectile()->AssimpLoadAnimation( EngineSetup::getInstance()->MODELS_FOLDER + model_proj->valuestring );
        ammoType->getModelProjectile()->setScale( scale_proj->valuedouble );
        ammoType->getModelBox()->setLabel( std::string(name->valuestring)+ "_model_box");
        ammoType->getModelBox()->setScale( scale_box->valuedouble);
        ammoType->getModelBox()->AssimpLoadGeometryFromFile( EngineSetup::getInstance()->MODELS_FOLDER + model_box->valuestring );

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
        cJSON *damage        = cJSON_GetObjectItemCaseSensitive(currentWeapon, "damage");
        cJSON *cadence       = cJSON_GetObjectItemCaseSensitive(currentWeapon, "cadence");
        cJSON *damageRadius  = cJSON_GetObjectItemCaseSensitive(currentWeapon, "damage_radius");
        cJSON *speed         = cJSON_GetObjectItemCaseSensitive(currentWeapon, "speed");
        cJSON *soundMark     = cJSON_GetObjectItemCaseSensitive(currentWeapon, "mark_sound");
        cJSON *accuracy      = cJSON_GetObjectItemCaseSensitive(currentWeapon, "accuracy");
        cJSON *dispersion    = cJSON_GetObjectItemCaseSensitive(currentWeapon, "dispersion");
        cJSON *iconHUD       = cJSON_GetObjectItemCaseSensitive(currentWeapon, "icon_hud");
        cJSON *sniper        = cJSON_GetObjectItemCaseSensitive(currentWeapon, "sniper");
        cJSON *index         = cJSON_GetObjectItemCaseSensitive(currentWeapon, "index");
        cJSON *model         = cJSON_GetObjectItemCaseSensitive(currentWeapon, "model");
        cJSON *scale         = cJSON_GetObjectItemCaseSensitive(currentWeapon, "scale");
        cJSON *sndEmptyLabel = cJSON_GetObjectItemCaseSensitive(currentWeapon, "sound_empty_label");

        Logging::getInstance()->Log("Loading weapon " + std::string(name->valuestring), "WEAPONS");

        // WeaponType attributes
        weaponManager->addWeaponType(name->valuestring);
        WeaponType* weaponType = weaponManager->getWeaponTypeByLabel(name->valuestring);
        weaponType->setAvailable( true );
        weaponType->setClassname( classname->valuestring );
        weaponType->setAmmoType(weaponManager->ammoTypes[ammoIndex->valueint]);
        weaponType->setIndex( index->valueint );
        weaponType->setDamage( damage->valuedouble );
        weaponType->setDamageRadius( damageRadius->valuedouble );
        weaponType->setSpeed( (float) speed->valuedouble );
        weaponType->setAccuracy( accuracy->valuedouble );
        weaponType->setDispersion( dispersion->valueint );
        weaponType->loadIconHUD(std::string(name->valuestring) + "/" + std::string(iconHUD->valuestring));
        weaponType->getModel()->setLabel( std::string(name->valuestring) + "_model" );
        weaponType->getModel()->setScale( scale->valuedouble );
        weaponType->getModel()->AssimpLoadGeometryFromFile( EngineSetup::getInstance()->MODELS_FOLDER + model->valuestring );
        weaponType->counterCadence->setStep( cadence->valuedouble );
        weaponType->setSoundEmptyLabel( sndEmptyLabel->valuestring);

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
            cJSON *label      = cJSON_GetObjectItemCaseSensitive(currentWeaponAnimation, "label");
            cJSON *model      = cJSON_GetObjectItemCaseSensitive(currentWeaponAnimation, "model");
            cJSON *scale      = cJSON_GetObjectItemCaseSensitive(currentWeaponAnimation, "scale");
            cJSON *stopEnd    = cJSON_GetObjectItemCaseSensitive(currentWeaponAnimation, "stop_end");
            cJSON *looping    = cJSON_GetObjectItemCaseSensitive(currentWeaponAnimation, "looping");
            cJSON *projectile = cJSON_GetObjectItemCaseSensitive(currentWeaponAnimation, "projectile");
            cJSON *sound      = cJSON_GetObjectItemCaseSensitive(currentWeaponAnimation, "sound");

            Logging::getInstance()->Log("Loading JSON Weapon Animation: " + std::string(label->valuestring) + ", status:" + std::to_string(status->valueint) );

            weaponManager->getWeaponTypeByLabel(name->valuestring)->addAnimation(
                    std::string(label->valuestring),
                    std::string(model->valuestring),
                    scale->valuedouble,
                    stopEnd->valueint
            );

            weaponManager->getWeaponTypeByLabel(name->valuestring)->getWeaponSounds().emplace_back(sound->valuestring);
            weaponManager->getWeaponTypeByLabel(name->valuestring)->setWeaponAnimation(0);
        }
    }

    weaponManager->setCurrentWeaponIndex(EngineSetup::WeaponsTypes::PISTOL);
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
        cJSON *soundFire        = cJSON_GetObjectItemCaseSensitive(currentEnemy, "fire_sound");
        cJSON *soundMark        = cJSON_GetObjectItemCaseSensitive(currentEnemy, "mark_sound");
        cJSON *defaultAnimation = cJSON_GetObjectItemCaseSensitive(currentEnemy, "default_animation");
        cJSON *scale            = cJSON_GetObjectItemCaseSensitive(currentEnemy, "scale_mesh");

        NPCEnemyBody *newEnemy = new NPCEnemyBody();
        newEnemy->setDamage( (float) damage->valuedouble );
        newEnemy->setCadence( (float) cadence->valuedouble );
        newEnemy->setRange( (float) range->valuedouble );
        newEnemy->setSpeed( (float) speed->valuedouble );
        newEnemy->setPosition(Vertex3D(0, 0, 0) );
        newEnemy->setRotation(M3() );
        newEnemy->setLabel( name->valuestring);
        newEnemy->setClassname( classname->valuestring );
        newEnemy->setScale( scale->valuedouble );

        cJSON *draw_offset   = cJSON_GetObjectItemCaseSensitive(currentEnemy, "draw_offset" );
        cJSON *draw_offset_x = cJSON_GetObjectItemCaseSensitive(draw_offset, "x" );
        cJSON *draw_offset_y = cJSON_GetObjectItemCaseSensitive(draw_offset, "y" );
        cJSON *draw_offset_z = cJSON_GetObjectItemCaseSensitive(draw_offset, "z" );
        newEnemy->setDrawOffset( Vertex3D(
                draw_offset_x->valuedouble,
                draw_offset_y->valuedouble,
                draw_offset_z->valuedouble
        ));

        cJSON *box_shape_size   = cJSON_GetObjectItemCaseSensitive(currentEnemy, "box_shape_size");
        cJSON *box_shape_size_x = cJSON_GetObjectItemCaseSensitive(box_shape_size, "x" );
        cJSON *box_shape_size_y = cJSON_GetObjectItemCaseSensitive(box_shape_size, "y" );
        cJSON *box_shape_size_z = cJSON_GetObjectItemCaseSensitive(box_shape_size, "z" );
        newEnemy->setBoxShapeSize( Vertex3D(
                box_shape_size_x->valuedouble,
                box_shape_size_y->valuedouble,
                box_shape_size_z->valuedouble
        ));

        // animation's enemy loop
        cJSON *enemyAnimationsJSONList = cJSON_GetObjectItemCaseSensitive(currentEnemy, "animations" );
        cJSON *currentEnemyAnimation;
        cJSON_ArrayForEach(currentEnemyAnimation, enemyAnimationsJSONList) {
            cJSON *path           = cJSON_GetObjectItemCaseSensitive(currentEnemyAnimation, "path");
            cJSON *label          = cJSON_GetObjectItemCaseSensitive(currentEnemyAnimation, "label");
            cJSON *remove_at_end  = cJSON_GetObjectItemCaseSensitive(currentEnemyAnimation, "remove_at_end");

            newEnemy->addAnimation( label->valuestring, path->valuestring, newEnemy->getScale(), remove_at_end->valueint );
        }
        newEnemy->setAnimation( defaultAnimation->valueint );
        BUFFERS->enemyTemplates.push_back(newEnemy);
    }
}

void ComponentBSP::setCamera(Camera3D *camera) {
    ComponentBSP::camera = camera;
}

BSPCollider *ComponentBSP::getBSPCollider() const {
    return bspCollider;
}

