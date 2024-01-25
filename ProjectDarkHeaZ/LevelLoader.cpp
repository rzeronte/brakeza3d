//
// Created by eduardo on 20/3/22.
//

#include "LevelLoader.h"
#include "../include/Brakeza3D.h"
#include "Enemies/EnemyGhostEmitter.h"
#include "Behaviors/EnemyBehaviorCircle.h"
#include "Behaviors/EnemyBehaviorPatrol.h"
#include "Behaviors/EnemyBehaviorFollow.h"
#include "Items/ItemHealthGhost.h"
#include "Items/ItemEnergyGhost.h"
#include "Items/ItemWeaponGhost.h"
#include "Bosses/BossEnemy.h"
#include "Behaviors/EnemyBehaviorRandom.h"
#include "Bosses/BossLevel10.h"
#include "Bosses/BossLevel15.h"
#include "Behaviors/EnemyBehaviorPath.h"
#include "Bosses/BossLevel30.h"
#include "LevelStats.h"
#include "Items/EnemyDialog.h"
#include "Weapons/WeaponProjectiles.h"
#include "Weapons/WeaponLaser.h"
#include "Weapons/WeaponRayLight.h"
#include "Weapons/WeaponBomb.h"
#include "Weapons/WeaponReflection.h"
#include "Weapons/WeaponShield.h"
#include "Bosses/BossLevel25.h"
#include "Bosses/BossLevel20.h"

#include <utility>

LevelLoader::LevelLoader(std::string filename)
:
    waitingToWin(Counter(3)),
    stats(new LevelStats())
{
    addLevel(std::move(filename));
    setLevelStartedToPlay(false);
    setCurrentLevelIndex(-1);
    waitingToWin.setEnabled(false);
}

void LevelLoader::load(int levelIndex)
{
    stats->reset();

    Logging::Message("Loading level: %d", levelIndex);

    setLevelStartedToPlay(true);
    setLevelFinished(false);
    setCurrentLevelIndex(levelIndex);
    setHasMusic(false);

    loadLevelFromJSON(levels[levelIndex]);
}

void LevelLoader::addLevel(std::string filename)
{
    this->levels.push_back(std::move(filename));
}

void LevelLoader::loadPrevious()
{
    if (currentLevelIndex > 0) {
        currentLevelIndex--;
    }
    load(currentLevelIndex);
}

void LevelLoader::reload()
{
    load(currentLevelIndex);
    updateConfig(currentLevelIndex);
}

bool LevelLoader::loadNext()
{
    if ((int) currentLevelIndex >= (int) levels.size() - 1) {
        return false;
    }

    currentLevelIndex++;
    load(currentLevelIndex);

    updateConfig(currentLevelIndex);
    return true;
}

bool LevelLoader::isLevelStartedToPlay() const {
    return levelStartedToPlay;
}

void LevelLoader::setLevelStartedToPlay(bool index) {
    LevelLoader::levelStartedToPlay = index;
}

void LevelLoader::setCurrentLevelIndex(int index) {
    LevelLoader::currentLevelIndex = index;
}

int LevelLoader::size() {
    return (int) levels.size();
}

int LevelLoader::getCurrentLevelIndex() const {
    return currentLevelIndex;
}

void LevelLoader::loadLevelFromJSON(const std::string& filePath)
{
    Logging::head("Loading Level: %s", filePath.c_str());

    enemiesEmitter.resize(0);

    for (auto tutorial: levelHistories) {
        delete tutorial;
    }
    levelHistories.resize(0);

    for (auto help: levelTutorials) {
        delete help;
    }
    levelTutorials.resize(0);

    size_t file_size;
    auto contentFile = Tools::readFile(filePath, file_size);
    cJSON *jsonContentFile = cJSON_Parse(contentFile);

    if (jsonContentFile == nullptr) {
        Logging::Log("Can't be loaded: %s", filePath.c_str());
        return;
    }

    if (cJSON_GetObjectItemCaseSensitive(jsonContentFile, "music") != nullptr) {
        setMusic(cJSON_GetObjectItemCaseSensitive(jsonContentFile, "music")->valuestring);
        this->hasMusic = true;
    }

    setLevelName(cJSON_GetObjectItemCaseSensitive(jsonContentFile, "name")->valuestring);

    if (cJSON_GetObjectItemCaseSensitive(jsonContentFile, "scene") != nullptr) {
        setLevelScene(cJSON_GetObjectItemCaseSensitive(jsonContentFile, "scene")->valuestring);
        ComponentsManager::get()->getComponentRender()->getSceneLoader().loadScene(levelScene);
    }

    setEndLevel(false);
    if (cJSON_GetObjectItemCaseSensitive(jsonContentFile, "endLevel") != nullptr) {
        setEndLevel(cJSON_GetObjectItemCaseSensitive(jsonContentFile, "endLevel")->valueint);
    }

    ComponentsManager::get()->getComponentGame()->getShaderBackgroundImage()->getImage().setImage(
       EngineSetup::get()->IMAGES_FOLDER + cJSON_GetObjectItemCaseSensitive(jsonContentFile, "backgroundImage")->valuestring
    );

    if (cJSON_GetObjectItemCaseSensitive(jsonContentFile, "foregroundImage") != nullptr) {
        ComponentsManager::get()->getComponentGame()->getShaderForegroundImage()->setEnabled(true);
        auto fileForeground = EngineSetup::get()->IMAGES_FOLDER + cJSON_GetObjectItemCaseSensitive(jsonContentFile, "foregroundImage")->valuestring;
        ComponentsManager::get()->getComponentGame()->getShaderForegroundImage()->getImage().setImage(fileForeground);
    } else {
        ComponentsManager::get()->getComponentGame()->getShaderForegroundImage()->setEnabled(false);
    }

    if (cJSON_GetObjectItemCaseSensitive(jsonContentFile, "histories") != nullptr) {
        cJSON *currentHistory;
        cJSON_ArrayForEach(currentHistory, cJSON_GetObjectItemCaseSensitive(jsonContentFile, "histories")) {
            levelHistories.push_back(new Image(EngineSetup::get()->IMAGES_FOLDER + currentHistory->valuestring));
        }
        currentTutorialIndex = 0;
    }

    if (cJSON_GetObjectItemCaseSensitive(jsonContentFile, "tutorials") != nullptr) {
        cJSON *currentLevelTutorial;
        cJSON_ArrayForEach(currentLevelTutorial, cJSON_GetObjectItemCaseSensitive(jsonContentFile, "tutorials")) {
            levelTutorials.push_back(new Image(EngineSetup::get()->IMAGES_FOLDER + currentLevelTutorial->valuestring));
        }
    }

    ComponentsManager::get()->getComponentGame()->getPlayer()->setAllowEnergyShield(
        cJSON_GetObjectItemCaseSensitive(jsonContentFile, "allowEnergyShield")->valueint
    );

    if (cJSON_GetObjectItemCaseSensitive(jsonContentFile, "message") != nullptr) {
        parseMainMessageJSON(cJSON_GetObjectItemCaseSensitive(jsonContentFile, "message"));
    } else {
        mainMessage = nullptr;
    }

    auto c = parseColorJSON(cJSON_GetObjectItemCaseSensitive(jsonContentFile, "color"));

    ComponentsManager::get()->getComponentGame()->getPlayer()->getWeaponLight()->setColor(c);
    ComponentsManager::get()->getComponentGame()->getPlayer()->getWeaponLight()->setColorSpecular(c);

    cJSON *currentEnemyJSON;
    cJSON_ArrayForEach(currentEnemyJSON, cJSON_GetObjectItemCaseSensitive(jsonContentFile, "enemies")) {
        auto enemy = new EnemyGhost();
        parseEnemyJSON(currentEnemyJSON, enemy);

        auto respawner = new EnemyGhostEmitter(enemy, 3);
        respawner->setPosition(enemy->getPosition());
        Brakeza3D::get()->addObject3D(respawner, Brakeza3D::uniqueObjectLabel("respawner"));
        enemiesEmitter.push_back(respawner);
    }

    cJSON *currentItem;
    cJSON_ArrayForEach(currentItem, cJSON_GetObjectItemCaseSensitive(jsonContentFile, "items")) {
        LevelLoader::parseItemJSON(currentItem);
    }

    cJSON *currentBoss;
    cJSON_ArrayForEach(currentBoss, cJSON_GetObjectItemCaseSensitive(jsonContentFile, "bosses")) {
        this->parseBossJSON(currentBoss);
    }

    Logging::head("End loading Level: %s", filePath.c_str());

    free(contentFile);
    cJSON_Delete(jsonContentFile);
}

Weapon *LevelLoader::parseWeaponJSON(Object3D *parent, cJSON *weaponJson, bool isPlayer)
{
    int index = cJSON_GetObjectItemCaseSensitive(weaponJson, "index")->valueint;

    Logging::head("Loading weapon (%d)", index);

    bool selectable = false;
    if (cJSON_GetObjectItemCaseSensitive(weaponJson, "selectable") != nullptr) {
        selectable = (bool) cJSON_GetObjectItemCaseSensitive(weaponJson, "selectable")->valueint;
    }

    auto *rayLight = new RayLight(
        true,
        parent,
        parent->AxisDown().getNormalize(),
        parent->AxisDown().getScaled(0.1),
        500,
        0,
        Color::green(),
        Color::green(),
        EngineSetup::collisionGroups::Projectile,
        EngineSetup::collisionGroups::Enemy
    );

    auto type = cJSON_GetObjectItemCaseSensitive(weaponJson, "type")->valuestring;

    WeaponAttributes attributes = {
        parent,
        cJSON_GetObjectItemCaseSensitive(weaponJson, "name")->valuestring,
        std::string(EngineSetup::get()->MODELS_FOLDER +
        cJSON_GetObjectItemCaseSensitive(weaponJson, "model")->valuestring),
        std::string(EngineSetup::get()->MODELS_FOLDER +
        cJSON_GetObjectItemCaseSensitive(weaponJson, "modelProjectile")->valuestring),
        EngineSetup::get()->ASSETS_FOLDER + cJSON_GetObjectItemCaseSensitive(weaponJson, "icon")->valuestring,
        parseColorJSON(cJSON_GetObjectItemCaseSensitive(weaponJson, "color")),
        (bool) cJSON_GetObjectItemCaseSensitive(weaponJson, "flatTexture")->valueint,
        (bool) cJSON_GetObjectItemCaseSensitive(weaponJson, "enableLights")->valueint,
        cJSON_GetObjectItemCaseSensitive(weaponJson, "amount")->valueint,
        cJSON_GetObjectItemCaseSensitive(weaponJson, "startAmount")->valueint,
        (float) cJSON_GetObjectItemCaseSensitive(weaponJson, "damage")->valuedouble,
        (float) cJSON_GetObjectItemCaseSensitive(weaponJson, "speed")->valuedouble,
        cJSON_GetObjectItemCaseSensitive(weaponJson, "dispersion")->valueint,
        (float) cJSON_GetObjectItemCaseSensitive(weaponJson, "accuracy")->valuedouble,
        (float) cJSON_GetObjectItemCaseSensitive(weaponJson, "cadenceTime")->valuedouble,
        (bool) cJSON_GetObjectItemCaseSensitive(weaponJson, "stop")->valueint,
        (float) cJSON_GetObjectItemCaseSensitive(weaponJson, "stopEvery")->valuedouble,
        (float) cJSON_GetObjectItemCaseSensitive(weaponJson, "stopDuration")->valuedouble,
        (bool) cJSON_GetObjectItemCaseSensitive(weaponJson, "available")->valueint,
        selectable,
        rayLight
    };


    switch(WeaponTypesMapping[type]) {
        case WeaponTypes::WEAPON_PROJECTILE: {
            return new WeaponProjectiles(attributes);
        }
        case WeaponTypes::WEAPON_LASER: {
            return new WeaponLaser(attributes);
        }
        case WeaponTypes::WEAPON_RAYLIGHT: {
            auto ray = new WeaponRayLight(attributes);
            ray->setIsForPlayer(isPlayer);
            return ray;
        }
        case WeaponTypes::WEAPON_BOMB: {
            return new WeaponBomb(attributes);
        }
        case WeaponTypes::WEAPON_REFLECTION: {
            return  new WeaponReflection(attributes);
        }
        case WeaponTypes::WEAPON_SHIELD: {
            return new WeaponShield(attributes);
        }
    }
}

void LevelLoader::startCountDown()
{
    this->countDown.setStep(COUNTDOWN_TO_START);
    this->countDown.setEnabled(true);

    for (auto enemyEmitter : enemiesEmitter) {
        enemyEmitter->startCounter();
    }
}

const std::string &LevelLoader::getMusic() const {
    return music;
}

void LevelLoader::setMusic(const std::string &value) {
    LevelLoader::music = value;
}

const std::string &LevelLoader::getLevelName() const {
    return levelName;
}

void LevelLoader::setLevelName(const std::string &value) {
    LevelLoader::levelName = value;
}

Counter * LevelLoader::getCountDown() {
    return &countDown;
}

ItemHealthGhost* LevelLoader::makeItemHealthGhost(Vertex3D position)
{
    auto *healthItem = new ItemHealthGhost();
    healthItem->setLabel(Brakeza3D::uniqueObjectLabel("itemLevelHealth"));
    healthItem->setEnableLights(true);
    healthItem->setPosition(position);
    healthItem->setRotationFrameEnabled(true);
    healthItem->setRotationFrame(Tools::randomVertex());
    healthItem->setStencilBufferEnabled(true);
    healthItem->setScale(1);
    healthItem->AssimpLoadGeometryFromFile(std::string(EngineSetup::get()->MODELS_FOLDER + "red_pill.fbx"));
    healthItem->makeSimpleGhostBody(
            Vertex3D(0.5, 0.5, 0.5),
            ComponentsManager::get()->getComponentCollisions()->getDynamicsWorld(),
            EngineSetup::collisionGroups::Weapon,
            EngineSetup::collisionGroups::Player
    );
    healthItem->updateBulletFromMesh3D();

    Brakeza3D::get()->addObject3D(healthItem, healthItem->getLabel());

    return healthItem;
}

ItemHumanGhost* LevelLoader::makeItemHuman(Vertex3D position)
{
    auto *human = new ItemHumanGhost();
    human->setLabel(Brakeza3D::uniqueObjectLabel("itemLevelHuman"));
    human->setEnableLights(true);
    human->setPosition(position);
    human->onStart();
    human->setRotationFrameEnabled(true);
    human->setRotationFrame(Tools::randomVertex().getScaled(0.5));
    human->setStencilBufferEnabled(true);
    human->setScale(1);
    human->AssimpLoadGeometryFromFile(std::string(EngineSetup::get()->MODELS_FOLDER + "astronaut.fbx"));
    human->makeSimpleGhostBody(
        Vertex3D(0.5, 0.5, 0.5),
        ComponentsManager::get()->getComponentCollisions()->getDynamicsWorld(),
        EngineSetup::collisionGroups::Player,
        EngineSetup::collisionGroups::Enemy | EngineSetup::collisionGroups::ProjectileEnemy | EngineSetup::collisionGroups::Player
    );

    human->updateBulletFromMesh3D();
    Brakeza3D::get()->addObject3D(human, human->getLabel());

    return human;
}

ItemEnergyGhost* LevelLoader::makeItemEnergyGhost(Vertex3D position)
{
    auto *energyItem = new ItemEnergyGhost();
    energyItem->setLabel(Brakeza3D::uniqueObjectLabel("itemLevelEnergy"));
    energyItem->setEnableLights(true);
    energyItem->setPosition(position);
    energyItem->setRotationFrameEnabled(true);
    energyItem->setRotationFrame(Tools::randomVertex().getScaled(0.5));
    energyItem->setStencilBufferEnabled(true);
    energyItem->setScale(1);
    energyItem->AssimpLoadGeometryFromFile(std::string(EngineSetup::get()->MODELS_FOLDER + "pill.fbx"));
    energyItem->makeSimpleGhostBody(
        Vertex3D(0.5, 0.5, 0.5),
        ComponentsManager::get()->getComponentCollisions()->getDynamicsWorld(),
        EngineSetup::collisionGroups::Weapon,
        EngineSetup::collisionGroups::Player
    );

    energyItem->updateBulletFromMesh3D();
    Brakeza3D::get()->addObject3D(energyItem, energyItem->getLabel());

    return energyItem;
}

ItemWeaponGhost* LevelLoader::makeItemWeapon(int indexWeapon, Vertex3D position)
{
    auto weapons = ComponentsManager::get()->getComponentGame()->getLevelLoader()->getWeapons();

    auto *weaponItem = new ItemWeaponGhost(weapons[indexWeapon]);
    weaponItem->setLabel(Brakeza3D::uniqueObjectLabel("itemWeapon"));
    weaponItem->setEnableLights(false);
    weaponItem->clone(weapons[indexWeapon]->getModel());
    weaponItem->setPosition(position);
    weaponItem->makeSimpleGhostBody(
        Vertex3D(0.5, 0.5, 0.5),
        ComponentsManager::get()->getComponentCollisions()->getDynamicsWorld(),
        EngineSetup::collisionGroups::Weapon,
        EngineSetup::collisionGroups::Player
    );
    weaponItem->setRotation(0, 0, 180);
    weaponItem->setRotationFrameEnabled(true);
    weaponItem->setRotationFrame(Tools::randomVertex().getScaled(0.5));
    weaponItem->setStencilBufferEnabled(true);
    weaponItem->updateBulletFromMesh3D();

    Brakeza3D::get()->addObject3D(weaponItem, weaponItem->getLabel());

    return weaponItem;
}

bool LevelLoader::isLevelFinished() const {
    return levelFinished;
}

void LevelLoader::setLevelFinished(bool value) {
    LevelLoader::levelFinished = value;
}

void LevelLoader::parseEnemyJSON(cJSON *enemyJSON, EnemyGhost *enemy)
{
    std::string name = cJSON_GetObjectItemCaseSensitive(enemyJSON, "name")->valuestring;
    std::string model = cJSON_GetObjectItemCaseSensitive(enemyJSON, "model")->valuestring;
    bool enableLights = (bool) cJSON_GetObjectItemCaseSensitive(enemyJSON, "enableLights")->valueint;
    auto stamina = (float) cJSON_GetObjectItemCaseSensitive(enemyJSON, "stamina")->valueint;
    int reward = cJSON_GetObjectItemCaseSensitive(enemyJSON, "reward")->valueint;
    int animated = cJSON_GetObjectItemCaseSensitive(enemyJSON, "animated")->valueint;
    cJSON *motion = cJSON_GetObjectItemCaseSensitive(enemyJSON, "motion");
    cJSON *weapon = cJSON_GetObjectItemCaseSensitive(enemyJSON, "weapon");
    cJSON *emitter = cJSON_GetObjectItemCaseSensitive(enemyJSON, "emitter");
    cJSON *lasers = cJSON_GetObjectItemCaseSensitive(enemyJSON, "lasers");

    Vertex3D worldPosition = getVertex3DFromJSONPosition(cJSON_GetObjectItemCaseSensitive(enemyJSON, "position"), Z_COORDINATE_GAMEPLAY + 1);
    enemy->setMultiScene(true);
    enemy->setRewards(reward);
    enemy->setName(name);

    if (motion != nullptr) {
        LevelLoader::setBehaviorFromJSON(motion, enemy, Z_COORDINATE_GAMEPLAY + 1);
    }

    LevelLoader::setLasersForEnemy(lasers, enemy);

    enemy->setEnabled(true);
    enemy->setLabel(Brakeza3D::uniqueObjectLabel("NPC"));
    enemy->setEnableLights(enableLights);
    enemy->setPosition(worldPosition);
    enemy->setStencilBufferEnabled(true);
    enemy->onStart();
    enemy->setScale(1);
    enemy->setStamina(stamina);
    enemy->setStartStamina(stamina);

    if (cJSON_GetObjectItemCaseSensitive(enemyJSON, "explode")) {
        enemy->setExplode(true);
        enemy->setExplodeDamage((float)cJSON_GetObjectItemCaseSensitive(enemyJSON, "explode")->valuedouble);
    }

    if (cJSON_GetObjectItemCaseSensitive(enemyJSON, "avatar") != nullptr) {
        std::string avatar = cJSON_GetObjectItemCaseSensitive(enemyJSON, "avatar")->valuestring;
        enemy->setAvatar(new Image(EngineSetup::get()->ICONS_FOLDER + avatar));
    }

    if (cJSON_GetObjectItemCaseSensitive(enemyJSON, "avatarHud") != nullptr) {
        std::string avatarHud = cJSON_GetObjectItemCaseSensitive(enemyJSON, "avatarHud")->valuestring;
        enemy->setAvatarHud(new Image(EngineSetup::get()->ICONS_FOLDER + avatarHud));
    }

    if (animated) {
        enemy->AssimpLoadAnimation(EngineSetup::get()->MODELS_FOLDER + model);
    } else {
        enemy->AssimpLoadGeometryFromFile(std::string(EngineSetup::get()->MODELS_FOLDER + model));
    }

    enemy->updateBoundingBox();
    Vertex3D aabbSize = enemy->getAabb().size().getScaled(0.3f);

    if (cJSON_GetObjectItemCaseSensitive(enemyJSON, "size") != nullptr) {
        aabbSize = parseVertex3DJSON(cJSON_GetObjectItemCaseSensitive(enemyJSON, "size"));
    }

    enemy->makeSimpleGhostBody(
        aabbSize,
        ComponentsManager::get()->getComponentCollisions()->getDynamicsWorld(),
        EngineSetup::collisionGroups::Enemy,
        EngineSetup::collisionGroups::Projectile | EngineSetup::collisionGroups::Player | EngineSetup::collisionGroups::ProjectileEnemy
    );
    enemy->setSoundChannel((int) enemiesEmitter.size() + 2);

    if (weapon != nullptr) {
        auto weaponType = parseWeaponJSON(enemy, weapon, false);
        weaponType->setEnabled(true);
        weaponType->setSoundChannel(enemy->getSoundChannel());
        enemy->setWeapon(weaponType);
    }

    if (emitter != nullptr) {
        LevelLoader::setProjectileEmitterForEnemy(emitter, enemy);
    }

    if (cJSON_GetObjectItemCaseSensitive(enemyJSON, "messages") != nullptr) {
        cJSON *currentMessage;
        cJSON_ArrayForEach(currentMessage, cJSON_GetObjectItemCaseSensitive(enemyJSON, "messages")) {
            parseMessageJSON(currentMessage, enemy);
        }
    }
}

void LevelLoader::setBehaviorFromJSON(cJSON *motion, Object3D *enemy, float depth)
{
    std::string typeMotion = cJSON_GetObjectItemCaseSensitive(motion, "type")->valuestring;

    switch(behaviorTypes[typeMotion]) {
        case EnemyBehaviorTypes::ROTATE_FRAME: {
            enemy->setRotationFrameEnabled(true);

            cJSON *rotationJSON = cJSON_GetObjectItemCaseSensitive(motion, "rotation");
            enemy->setRotationFrame(Tools::randomVertex().getScaled(0.1));
            if (rotationJSON != nullptr) {
                enemy->setRotationFrame(parseVertex3DJSON(rotationJSON));
            }
            break;
        }
        case EnemyBehaviorTypes::BEHAVIOR_PATROL: {
            Vertex3D from = getVertex3DFromJSONPosition(cJSON_GetObjectItemCaseSensitive(motion, "from"), depth);
            Vertex3D to = getVertex3DFromJSONPosition(cJSON_GetObjectItemCaseSensitive(motion, "to"), depth);

            auto behaviorSpeed = (float) cJSON_GetObjectItemCaseSensitive(motion, "speed")->valuedouble;
            auto behavior = new EnemyBehaviorPatrol(from, to, behaviorSpeed);
            enemy->setBehavior(behavior);

            cJSON *rotationJSON = cJSON_GetObjectItemCaseSensitive(motion, "rotation");
            if (rotationJSON != nullptr) {
                enemy->setRotationFrameEnabled(true);
                enemy->setRotationFrame(parseVertex3DJSON(rotationJSON));
            }
            break;
        }
        case EnemyBehaviorTypes::BEHAVIOR_FOLLOW: {
            auto behavior = new EnemyBehaviorFollow(
                ComponentsManager::get()->getComponentGame()->getPlayer(),
                (float) cJSON_GetObjectItemCaseSensitive(motion, "speed")->valueint,
                (float) cJSON_GetObjectItemCaseSensitive(motion, "separation")->valueint
            );
            behavior->setEnabled(false);
            enemy->setBehavior(behavior);
            break;
        }
        case EnemyBehaviorTypes::BEHAVIOR_CIRCLE: {
            Vertex3D center = getVertex3DFromJSONPosition(cJSON_GetObjectItemCaseSensitive(motion, "center"), depth);

            auto speed = (float) cJSON_GetObjectItemCaseSensitive(motion, "speed")->valuedouble;
            auto radius = (float) cJSON_GetObjectItemCaseSensitive(motion, "radius")->valuedouble;

            auto behavior = new EnemyBehaviorCircle(center, speed, radius);
            enemy->setBehavior(behavior);
            break;
        }
        case EnemyBehaviorTypes::BEHAVIOR_RANDOM: {
            auto speed = (float) cJSON_GetObjectItemCaseSensitive(motion, "speed")->valuedouble;

            auto behavior = new EnemyBehaviorRandom(speed);
            enemy->setBehavior(behavior);
            break;
        }
        case EnemyBehaviorTypes::BEHAVIOR_PATH: {
            auto speed = (float) cJSON_GetObjectItemCaseSensitive(motion, "speed")->valuedouble;
            auto behavior = new EnemyBehaviorPath(speed);

            auto pointsJSON = cJSON_GetObjectItemCaseSensitive(motion, "points");

            cJSON *currentPoint;
            cJSON_ArrayForEach(currentPoint, pointsJSON) {
                Vertex3D position = getVertex3DFromJSONPosition(currentPoint, depth);
                behavior->addPoint(position);
            }
            behavior->start();
            enemy->setBehavior(behavior);
            break;
        }
        case BEHAVIOR_NONE:
            break;
    }
}

Point2D LevelLoader::parsePositionJSON(cJSON *positionJSON)
{
    const int x = (cJSON_GetObjectItemCaseSensitive(positionJSON, "x")->valueint * EngineSetup::get()->screenWidth) / 100;
    const int y = (cJSON_GetObjectItemCaseSensitive(positionJSON, "y")->valueint * EngineSetup::get()->screenHeight) / 100;

    return {x, y};
}

Vertex3D LevelLoader::parseVertex3DJSON(cJSON *vertex3DJSON)
{
    const auto x = (float) cJSON_GetObjectItemCaseSensitive(vertex3DJSON, "x")->valuedouble;
    const auto y = (float) cJSON_GetObjectItemCaseSensitive(vertex3DJSON, "y")->valuedouble;
    const auto z = (float) cJSON_GetObjectItemCaseSensitive(vertex3DJSON, "z")->valuedouble;

    return {x, y, z};
}

void LevelLoader::setLasersForEnemy(cJSON *lasers, EnemyGhost *enemy)
{
    if (lasers == nullptr) {
        return;
    }

    cJSON *currentLaserJSON;
    cJSON_ArrayForEach(currentLaserJSON, lasers) {
        addLasersForEnemy(currentLaserJSON, enemy);
    }
}

void LevelLoader::addLasersForEnemy(cJSON *laser, EnemyGhost *enemy)
{
    auto direction = parseVertex3DJSON(cJSON_GetObjectItemCaseSensitive(laser, "direction"));
    auto length = cJSON_GetObjectItemCaseSensitive(laser, "length")->valueint;

    enemy->addFixedLaser(new ProjectileRay(
        enemy,
        enemy->getPosition(),
        (float) cJSON_GetObjectItemCaseSensitive(laser, "damage")->valueint,
        direction.getScaled((float) length),
        direction.getScaled((float) length),
        EngineSetup::collisionGroups::ProjectileEnemy,
        EngineSetup::collisionGroups::Player,
        0,
        parseColorJSON(cJSON_GetObjectItemCaseSensitive(laser, "color")),
        0.001f,
        true
    ));
}

void LevelLoader::setProjectileEmitterForEnemy(cJSON *emitter, EnemyGhost *enemy)
{
    cJSON *rotation = cJSON_GetObjectItemCaseSensitive(emitter, "rotation");
    cJSON *rotationFrame = cJSON_GetObjectItemCaseSensitive(emitter, "rotationFrame");
    int type = cJSON_GetObjectItemCaseSensitive(emitter, "type")->valueint;

    auto projectileEmitter = new AmmoProjectileBodyEmitter(
        (ProjectileBodyEmmitterType) type,
        (float) cJSON_GetObjectItemCaseSensitive(emitter, "cadenceTime")->valuedouble,
        (bool) cJSON_GetObjectItemCaseSensitive(emitter, "stop")->valueint,
        (float) cJSON_GetObjectItemCaseSensitive(emitter, "stopDuration")->valuedouble,
        (float) cJSON_GetObjectItemCaseSensitive(emitter, "stopEvery")->valuedouble,
        enemy->getWeapon(),
        parseColorJSON(cJSON_GetObjectItemCaseSensitive(emitter, "color"))
    );

    projectileEmitter->setPosition(enemy->getPosition());
    projectileEmitter->setRotationFrameEnabled(true);

    projectileEmitter->setRotation(M3::getMatrixRotationForEulerAngles(
        (float) cJSON_GetObjectItemCaseSensitive(rotation, "x")->valueint,
        (float) cJSON_GetObjectItemCaseSensitive(rotation, "y")->valueint,
        (float) cJSON_GetObjectItemCaseSensitive(rotation, "z")->valueint
    ));

    projectileEmitter->setRotationFrame(Vertex3D(
        (float) cJSON_GetObjectItemCaseSensitive(rotationFrame, "x")->valueint,
        (float) cJSON_GetObjectItemCaseSensitive(rotationFrame, "y")->valueint,
        (float) cJSON_GetObjectItemCaseSensitive(rotationFrame, "z")->valueint
    ));

    projectileEmitter->setLabel(Brakeza3D::uniqueObjectLabel("bossProjectileEmitter"));

    enemy->setProjectileEmitter(projectileEmitter);
}

Vertex3D LevelLoader::getVertex3DFromJSONPosition(cJSON *positionJSON, float depth)
{
    Point2D fixedPosition = parsePositionJSON(positionJSON);
    return getPositionFromScreenPoint(fixedPosition, depth);
}

Vertex3D LevelLoader::getPositionFromScreenPoint(Point2D fixedPosition, float depth)
{
    auto camera = ComponentsManager::get()->getComponentCamera()->getCamera();

    Vertex3D nearPlaneVertex = Transforms::Point2DToWorld(fixedPosition);

    Vector3D ray(camera->getPosition(),nearPlaneVertex);

    return ray.getComponent().getScaled(depth);
}

void LevelLoader::parseItemJSON(cJSON *itemJSON)
{
    const int typeItem = cJSON_GetObjectItemCaseSensitive(itemJSON, "type")->valueint;

    Vertex3D position = getVertex3DFromJSONPosition(cJSON_GetObjectItemCaseSensitive(itemJSON, "position"), Z_COORDINATE_GAMEPLAY);

    bool help = false;
    int helpIndex = -1;

    if (cJSON_GetObjectItemCaseSensitive(itemJSON, "help") != nullptr) {
        help = true;
        helpIndex = cJSON_GetObjectItemCaseSensitive(itemJSON, "help")->valueint;
    }

    switch(typeItem) {
        case LevelInfoItemsTypes::ITEM_ENERGY: {
            auto item = LevelLoader::makeItemEnergyGhost(position);
            item->setHasTutorial(help);
            item->setTutorialIndex(helpIndex);
            break;
        }
        case LevelInfoItemsTypes::ITEM_HEALTH: {
            auto item = LevelLoader::makeItemHealthGhost(position);
            item->setHasTutorial(help);
            item->setTutorialIndex(helpIndex);
            break;
        }
        case LevelInfoItemsTypes::ITEM_WEAPON_PROJECTILE: {
            auto item = LevelLoader::makeItemWeapon(WeaponTypes::WEAPON_PROJECTILE, position);
            item->setHasTutorial(help);
            item->setTutorialIndex(helpIndex);
            break;
        }
        case LevelInfoItemsTypes::ITEM_WEAPON_RAYLIGHT: {
            auto item = LevelLoader::makeItemWeapon(WeaponTypes::WEAPON_RAYLIGHT, position);
            item->setHasTutorial(help);
            item->setTutorialIndex(helpIndex);
            break;
        }
        case LevelInfoItemsTypes::ITEM_WEAPON_LASER: {
            auto item = LevelLoader::makeItemWeapon(WeaponTypes::WEAPON_LASER, position);
            item->setHasTutorial(help);
            item->setTutorialIndex(helpIndex);
            break;
        }
        case LevelInfoItemsTypes::ITEM_WEAPON_BOMB: {
            auto item = LevelLoader::makeItemWeapon(WeaponTypes::WEAPON_BOMB, position);
            item->setHasTutorial(help);
            item->setTutorialIndex(helpIndex);
            break;
        }
        case LevelInfoItemsTypes::ITEM_WEAPON_REFLECTION: {
            auto item = LevelLoader::makeItemWeapon(WeaponTypes::WEAPON_REFLECTION, position);
            item->setHasTutorial(help);
            item->setTutorialIndex(helpIndex);
            break;
        }
        case LevelInfoItemsTypes::ITEM_SHIELD: {
            auto item = LevelLoader::makeItemWeapon(WeaponTypes::WEAPON_SHIELD, position);
            item->setHasTutorial(help);
            item->setTutorialIndex(helpIndex);
            break;
        }
        case LevelInfoItemsTypes::ITEM_HUMAN: {
            auto item = LevelLoader::makeItemHuman(position);
            item->setHasTutorial(help);
            item->setTutorialIndex(helpIndex);
            break;
        }
        case LevelInfoItemsTypes::ITEM_SALVAGE_SPACESHIP: {
            auto item = LevelLoader::makeSalvageSpaceship(position);
            item->setHasTutorial(help);
            item->setTutorialIndex(helpIndex);
            break;
        }
        default: {
            Logging::Log("Item not found in parsing JSON");
            exit(-1);
        }
    }
}

void LevelLoader::parseBossJSON(cJSON *bossJSON)
{
    const int typeBoss = cJSON_GetObjectItemCaseSensitive(bossJSON, "type")->valueint;

    BossEnemy *boss;
    switch(typeBoss) {
        default:
        case BossesTypes::BOSS_LEVEL_10: {
            boss = new BossLevel10();
            break;
        }
        case BossesTypes::BOSS_LEVEL_15: {
            boss = new BossLevel15();
            break;
        }
        case BossesTypes::BOSS_LEVEL_20: {
            boss = new BossLevel20();
            break;
        }
        case BossesTypes::BOSS_LEVEL_25: {
            boss = new BossLevel25();
            break;
        }
        case BossesTypes::BOSS_LEVEL_30: {
            boss = new BossLevel30();
            break;
        }
    }

    parseEnemyJSON(bossJSON, boss);
    boss->onStart();
    auto respawner = new EnemyGhostEmitter(boss, 3);
    respawner->setPosition(boss->getPosition());

    Brakeza3D::get()->addObject3D(respawner, Brakeza3D::uniqueObjectLabel("respawner"));
    enemiesEmitter.push_back(respawner);
}

bool LevelLoader::isHaveMusic() const {
    return hasMusic;
}

void LevelLoader::setHasMusic(bool value)
{
    LevelLoader::hasMusic = value;
}

Color LevelLoader::parseColorJSON(cJSON *color)
{
    return {
    (float) cJSON_GetObjectItemCaseSensitive(color, "r")->valueint/255,
    (float) cJSON_GetObjectItemCaseSensitive(color, "g")->valueint/255,
    (float) cJSON_GetObjectItemCaseSensitive(color, "b")->valueint/255
    };
}

const std::vector<std::string> &LevelLoader::getLevels() const {
    return levels;
}

bool LevelLoader::isEndLevel() const {
    return endLevel;
}

void LevelLoader::setEndLevel(bool value) {
    LevelLoader::endLevel = value;
}

LevelStats *LevelLoader::getStats() const {
    return stats;
}

void LevelLoader::removeBackgroundObjects()
{
    for (auto o : Brakeza3D::get()->getSceneObjects()) {
        if (o->isBelongToScene()) {
            o->setRemoved(true);
        }
    }
}

void LevelLoader::moveBackgroundObjects(Vertex3D offset)
{
    for (auto o : Brakeza3D::get()->getSceneObjects()) {
        if (o->isBelongToScene()) {
            o->addToPosition(offset);
        }
    }
}

void LevelLoader::parseMessageJSON(cJSON *message, EnemyGhost *enemy)
{
    std::string text = cJSON_GetObjectItemCaseSensitive(message, "text")->valuestring;
    std::string sound = cJSON_GetObjectItemCaseSensitive(message, "sound")->valuestring;

    auto stamina = (float) cJSON_GetObjectItemCaseSensitive(message, "stamina")->valuedouble;

    auto dialog = new EnemyDialog(
        enemy,
        enemy->getAvatarHud(),
        enemy->getAvatar(),
        stamina,
        text.c_str(),
        sound.c_str(),
        enemy->getName().c_str(),
        ComponentsManager::get()->getComponentWindow()->getFontDefault()
    );

    enemy->getDialogs().push_back(dialog);
}

void LevelLoader::parseMainMessageJSON(cJSON *message)
{
    std::string from = cJSON_GetObjectItemCaseSensitive(message, "from")->valuestring;
    std::string text = cJSON_GetObjectItemCaseSensitive(message, "text")->valuestring;
    std::string sound = cJSON_GetObjectItemCaseSensitive(message, "sound")->valuestring;
    std::string avatarHud = cJSON_GetObjectItemCaseSensitive(message, "avatarHud")->valuestring;
    auto stamina = (float) cJSON_GetObjectItemCaseSensitive(message, "stamina")->valuedouble;

    mainMessage = new EnemyDialog(
        nullptr,
        new Image(EngineSetup::get()->ICONS_FOLDER + avatarHud),
        new Image(EngineSetup::get()->ICONS_FOLDER + avatarHud),
        stamina,
        text.c_str(),
        sound.c_str(),
        from.c_str(),
        ComponentsManager::get()->getComponentWindow()->getFontAlternative()
    );
}

std::vector<Image*> &LevelLoader::getTutorials() {
    return levelHistories;
}

void LevelLoader::drawCurrentTutorialImage(float alpha)
{
    this->levelHistories[currentTutorialIndex]->drawFlatAlpha(0, 0, alpha, ComponentsManager::get()->getComponentWindow()->getForegroundFramebuffer());
}

void LevelLoader::increaseTutorialImage()
{
    if (currentTutorialIndex + 1 < (int) levelHistories.size()) {
        currentTutorialIndex++;
    }
}

void LevelLoader::decreaseTutorialImage()
{
    if (currentTutorialIndex > 0) {
        currentTutorialIndex--;
    }
}

int LevelLoader::getCurrentTutorialIndex() const {
    return currentTutorialIndex;
}

Counter *LevelLoader::getWaitingToWin() {
    return &waitingToWin;
}

SalvageSpaceship* LevelLoader::makeSalvageSpaceship(Vertex3D position)
{
    auto salvage = new SalvageSpaceship();
    salvage->setRotation(10, 10, 0);
    salvage->setEnabled(true);
    salvage->setAlpha(255);
    salvage->setEnableLights(true);
    salvage->setStencilBufferEnabled(true);
    salvage->AssimpLoadGeometryFromFile(std::string(EngineSetup::get()->MODELS_FOLDER + "/spaceships/salvage.fbx"));
    salvage->setPosition(position + Vertex3D(0, 0, 2000));
    salvage->makeSimpleGhostBody(
        Vertex3D(1, 1, 1),
        ComponentsManager::get()->getComponentCollisions()->getDynamicsWorld(),
        EngineSetup::collisionGroups::Weapon,
        EngineSetup::collisionGroups::Player
    );
    salvage->updateBulletFromMesh3D();

    Brakeza3D::get()->addObject3D(salvage, "salvage");

    return salvage;
}

void LevelLoader::LoadConfig()
{
    Logging::head("Loading Project DarkHeaZ (%s)", "project-darkheaz.json");

    const std::string filePath = EngineSetup::get()->CONFIG_FOLDER + EngineSetup::get()->DARKHEAZ_MAIN_CONFIG;

    size_t file_size;
    auto contentFile = Tools::readFile(filePath, file_size);

    cJSON *myDataJSON = cJSON_Parse(contentFile);

    if (myDataJSON == nullptr) {
        Logging::Message("[Load Config] Can't be loaded: %s", filePath.c_str());
        exit(-1);
    }

    int level = cJSON_GetObjectItemCaseSensitive(myDataJSON, "level")->valueint;

    if (level >= 0) {
        currentLevelIndex = level - 1;
    }
}

void LevelLoader::updateConfig(int level) {

    std::string filePathStr = EngineSetup::get()->CONFIG_FOLDER + EngineSetup::get()->DARKHEAZ_MAIN_CONFIG;
    const char* file_path = filePathStr.c_str();

    cJSON *json = cJSON_CreateObject();

    cJSON_AddNumberToObject(json, "level", level);

    char *output_string = cJSON_Print(json);

    FILE *f = fopen(file_path, "w");
    if (f == nullptr) {
        Logging::Message("Error opening file for writing");
        cJSON_Delete(json);
        free(output_string);
        fclose(f);
        return;
    }

    fprintf(f, "%s", output_string);
    fclose(f);

    cJSON_Delete(json);
    free(output_string);
}

EnemyDialog *LevelLoader::getMainMessage() const
{
    return mainMessage;
}

void LevelLoader::setLevelScene(const std::string &levelScene) {
    LevelLoader::levelScene = levelScene;
}

void LevelLoader::LoadJSONWeapons()
{
    std::string sndPath = EngineSetup::get()->SOUNDS_FOLDER;
    std::string filePath = EngineSetup::get()->CONFIG_FOLDER + EngineSetup::get()->CFG_WEAPONS;

    Logging::Message("Loading weapons from file %s", filePath.c_str());

    size_t file_size;
    auto contentFile = Tools::readFile(filePath, file_size);

    cJSON *myDataJSON = cJSON_Parse(contentFile);

    if (myDataJSON == nullptr) {
        Logging::Message("[Load Weapons] Can't be loaded: %s", filePath.c_str());
        exit(-1);
    }

    auto player = ComponentsManager::get()->getComponentGame()->getPlayer();

    cJSON *currentWeapon;
    cJSON_ArrayForEach(currentWeapon, cJSON_GetObjectItemCaseSensitive(myDataJSON, "weapons")) {
        auto weapon = LevelLoader::parseWeaponJSON(player, currentWeapon, true);
        weapon->setEnabled(false);
        weapon->setSoundChannel(1);
        weapons.push_back(weapon);
    }

    for (auto weapon : weapons) {
        player->addWeapon(weapon);
    }

    player->setWeaponTypeByIndex(0);

    free (contentFile);
    cJSON_Delete(myDataJSON);
}

const std::vector<Weapon *> &LevelLoader::getWeapons() const
{
    return weapons;
}

LevelLoader::~LevelLoader()
{
    for (auto weapon: weapons) {
        delete weapon;
    }
}