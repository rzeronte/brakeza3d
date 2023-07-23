//
// Created by eduardo on 20/3/22.
//

#include "LevelLoader.h"
#include "../include/Brakeza3D.h"
#include "src/enemies/EnemyGhostEmitter.h"
#include "src/enemies/behaviors/EnemyBehaviorCircle.h"
#include "src/enemies/behaviors/EnemyBehaviorPatrol.h"
#include "src/enemies/behaviors/EnemyBehaviorFollow.h"
#include "src/items/ItemHealthGhost.h"
#include "src/items/ItemEnergyGhost.h"
#include "src/items/ItemWeaponGhost.h"
#include "src/bosses/BossEnemy.h"
#include "src/enemies/behaviors/EnemyBehaviorRandom.h"
#include "src/enemies/AsteroidEnemyGhost.h"
#include "src/bosses/BossLevel10.h"
#include "src/bosses/BossLevel20.h"
#include "src/enemies/behaviors/EnemyBehaviorPath.h"
#include "src/bosses/BossLevel30.h"
#include "LevelStats.h"
#include "src/items/EnemyDialog.h"

#include <utility>

LevelLoader::LevelLoader(std::string filename)
:
    waitingToWin(Counter(3))
{
    stats = new LevelStats();

    addLevel(std::move(filename));
    setLevelStartedToPlay(false);
    setCurrentLevelIndex(-1);

    waitingToWin.setEnabled(false);
}

void LevelLoader::load(int levelIndex)
{
    stats->reset();

    Logging::Log("loading level: %d", levelIndex);
    setLevelStartedToPlay(true);
    setLevelFinished(false);
    setCurrentLevelIndex(levelIndex);
    setHasMusic(false);

    ComponentsManager::get()->getComponentGame()->getPlayer()->setKillsCounter(0);
    ComponentsManager::get()->getComponentHUD()->setRadioMessagesCounter(0);

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

bool LevelLoader::loadNext()
{

    if ((int) currentLevelIndex >= (int) levels.size() - 1) {
        return false;
    }

    currentLevelIndex++;
    load(currentLevelIndex);

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
    Logging::Message("*************************************");
    Logging::Message("Loading Level: %s", filePath.c_str());
    Logging::Message("*************************************");

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

    setEndLevel(false);
    if (cJSON_GetObjectItemCaseSensitive(jsonContentFile, "endLevel") != nullptr) {
        setEndLevel(cJSON_GetObjectItemCaseSensitive(jsonContentFile, "endLevel"));
    }

    ComponentsManager::get()->getComponentGame()->getShaderBackgroundImage()->setImage(
       EngineSetup::get()->IMAGES_FOLDER + cJSON_GetObjectItemCaseSensitive(jsonContentFile, "backgroundImage")->valuestring
    );

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

    auto c = parseColorJSON(cJSON_GetObjectItemCaseSensitive(jsonContentFile, "color"));

    ComponentsManager::get()->getComponentGame()->getPlayer()->getLight()->setColorSpecularity(c);
    ComponentsManager::get()->getComponentCamera()->getCamera()->getFrustum()->updateFrustum();

    cJSON *currentEnemyJSON;
    cJSON_ArrayForEach(currentEnemyJSON, cJSON_GetObjectItemCaseSensitive(jsonContentFile, "enemies")) {
        auto enemy = new EnemyGhost();
        parseEnemyJSON(currentEnemyJSON, enemy);

        auto respawner = new EnemyGhostEmitter(enemy, 3);
        respawner->setPosition(enemy->getPosition());
        Brakeza3D::get()->addObject3D(respawner, Brakeza3D::uniqueObjectLabel("rewpawner"));
        enemiesEmitter.push_back(respawner);
    }

    cJSON *currentItem;
    cJSON_ArrayForEach(currentItem, cJSON_GetObjectItemCaseSensitive(jsonContentFile, "items")) {
        this->parseItemJSON(currentItem);
    }

    cJSON *currentBoss;
    cJSON_ArrayForEach(currentBoss, cJSON_GetObjectItemCaseSensitive(jsonContentFile, "bosses")) {
        this->parseBossJSON(currentBoss);
    }

    cJSON *currentAsteroid;
    cJSON_ArrayForEach(currentAsteroid, cJSON_GetObjectItemCaseSensitive(jsonContentFile, "asteroids")) {
        auto asteroid = this->parseAsteroidJSON(currentAsteroid);
        auto respawner = new EnemyGhostEmitter(asteroid, 3);
        respawner->setPosition(asteroid->getPosition());
        Brakeza3D::get()->addObject3D(respawner, Brakeza3D::uniqueObjectLabel("asteroid"));
        enemiesEmitter.push_back(respawner);
    }

    cJSON *currentBackground;
    cJSON_ArrayForEach(currentBackground, cJSON_GetObjectItemCaseSensitive(jsonContentFile, "background")) {
        this->parseBackgroundItem(currentBackground);
    }

    free(contentFile);
    cJSON_Delete(jsonContentFile);
}

Weapon *LevelLoader::parseWeaponJSON(cJSON *weaponJson)
{
    int index = cJSON_GetObjectItemCaseSensitive(weaponJson, "index")->valueint;

    Logging::Message("Loading Weapon with index: %d", index);

    bool selectable = false;
    if (cJSON_GetObjectItemCaseSensitive(weaponJson, "selectable") != nullptr) {
        selectable = (bool) cJSON_GetObjectItemCaseSensitive(weaponJson, "selectable")->valueint;
    }

    return new Weapon(
        cJSON_GetObjectItemCaseSensitive(weaponJson, "name")->valuestring,
        std::string(EngineSetup::get()->MODELS_FOLDER + cJSON_GetObjectItemCaseSensitive(weaponJson, "model")->valuestring),
        std::string(EngineSetup::get()->MODELS_FOLDER + cJSON_GetObjectItemCaseSensitive(weaponJson, "modelProjectile")->valuestring),
        EngineSetup::get()->ASSETS_FOLDER + cJSON_GetObjectItemCaseSensitive(weaponJson, "icon")->valuestring,
        parseColorJSON(cJSON_GetObjectItemCaseSensitive(weaponJson, "color")),
        (bool) cJSON_GetObjectItemCaseSensitive(weaponJson, "flatTexture")->valueint,
        (bool) cJSON_GetObjectItemCaseSensitive(weaponJson, "enableLights")->valueint,
        cJSON_GetObjectItemCaseSensitive(weaponJson, "amount")->valueint,
        cJSON_GetObjectItemCaseSensitive(weaponJson, "startAmount")->valueint,
        (float) cJSON_GetObjectItemCaseSensitive(weaponJson, "damage")->valuedouble,
        cJSON_GetObjectItemCaseSensitive(weaponJson, "speed")->valueint,
        cJSON_GetObjectItemCaseSensitive(weaponJson, "dispersion")->valueint,
        (float) cJSON_GetObjectItemCaseSensitive(weaponJson, "accuracy")->valuedouble,
        (float) cJSON_GetObjectItemCaseSensitive(weaponJson, "cadenceTime")->valuedouble,
        (bool) cJSON_GetObjectItemCaseSensitive(weaponJson, "stop")->valueint,
        (float) cJSON_GetObjectItemCaseSensitive(weaponJson, "stopEvery")->valuedouble,
        (float) cJSON_GetObjectItemCaseSensitive(weaponJson, "stopDuration")->valuedouble,
        cJSON_GetObjectItemCaseSensitive(weaponJson, "type")->valueint,
        (bool) cJSON_GetObjectItemCaseSensitive(weaponJson, "available")->valueint,
        selectable
    );
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
    healthItem->makeGhostBody(
        ComponentsManager::get()->getComponentCollisions()->getDynamicsWorld(),
        healthItem,
        EngineSetup::collisionGroups::Health,
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
        Vertex3D(500, 500, 500),
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
        Vertex3D(500, 500, 500),
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
    auto weapons = ComponentsManager::get()->getComponentGame()->getWeapons();

    bool frameBox = false;
    if (indexWeapon == WEAPON_SHIELD || indexWeapon == WEAPON_REFLECTION || indexWeapon == WEAPON_BOMB) {
        frameBox = true;
    }

    auto *weaponItem = new ItemWeaponGhost(weapons[indexWeapon], frameBox);
    weaponItem->setLabel(Brakeza3D::uniqueObjectLabel("itemWeapon"));
    weaponItem->setEnableLights(false);
    weaponItem->clone(weapons[indexWeapon]->getModel());
    weaponItem->setPosition(position);
    weaponItem->makeSimpleGhostBody(
        Vertex3D(500, 500, 500),
        ComponentsManager::get()->getComponentCollisions()->getDynamicsWorld(),
        EngineSetup::collisionGroups::Weapon,
        EngineSetup::collisionGroups::Player
    );
    weaponItem->setRotation(0, 0, 180);
    weaponItem->setRotationFrameEnabled(true);
    weaponItem->setRotationFrame(Tools::randomVertex().getScaled(0.5));
    weaponItem->setStencilBufferEnabled(true);
    weaponItem->setScale(1);
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

    Vertex3D worldPosition = getVertex3DFromJSONPosition(cJSON_GetObjectItemCaseSensitive(enemyJSON, "position"), Z_COORDINATE_GAMEPLAY);

    enemy->setRewards(reward);
    enemy->setName(name);

    if (motion != nullptr) {
        this->setBehaviorFromJSON(motion, enemy, Z_COORDINATE_GAMEPLAY);
    }

    this->setLasersForEnemy(lasers, enemy);

    enemy->setEnabled(true);
    enemy->setLabel(Brakeza3D::uniqueObjectLabel("npc"));
    enemy->setEnableLights(enableLights);
    enemy->setPosition(worldPosition);
    enemy->setStencilBufferEnabled(true);
    enemy->onStart();
    enemy->setScale(1);
    enemy->setStamina(stamina);
    enemy->setStartStamina(stamina);

    if (cJSON_GetObjectItemCaseSensitive(enemyJSON, "avatar") != nullptr) {
        std::string avatar = cJSON_GetObjectItemCaseSensitive(enemyJSON, "avatar")->valuestring;
        enemy->setAvatar(new Image(EngineSetup::get()->ICONS_FOLDER + avatar));
    }

    if (animated) {
        enemy->AssimpLoadAnimation(EngineSetup::get()->MODELS_FOLDER + model);
    } else {
        enemy->AssimpLoadGeometryFromFile(std::string(EngineSetup::get()->MODELS_FOLDER + model));
    }
    enemy->updateBoundingBox();
    enemy->makeSimpleGhostBody(
        enemy->getAabb().size().getScaled(0.3f),
        ComponentsManager::get()->getComponentCollisions()->getDynamicsWorld(),
        EngineSetup::collisionGroups::Enemy,
        EngineSetup::collisionGroups::Projectile | EngineSetup::collisionGroups::Player | EngineSetup::collisionGroups::ProjectileEnemy
    );
    enemy->setSoundChannel((int) enemiesEmitter.size() + 2);

    if (weapon != nullptr) {
        auto weaponType = parseWeaponJSON(weapon);
        weaponType->setSoundChannel(enemy->getSoundChannel());
        enemy->setWeapon(weaponType);
    }

    if (emitter != nullptr) {
        this->setProjectileEmitterForEnemy(emitter, enemy);
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
    const int typeMotion = cJSON_GetObjectItemCaseSensitive(motion, "type")->valueint;

    switch(typeMotion) {
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

            float behaviorSpeed = (float) cJSON_GetObjectItemCaseSensitive(motion, "speed")->valuedouble;
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
                cJSON_GetObjectItemCaseSensitive(motion, "speed")->valueint,
                cJSON_GetObjectItemCaseSensitive(motion, "separation")->valueint
            );
            behavior->setEnabled(false);
            enemy->setBehavior(behavior);
            break;
        }
        case EnemyBehaviorTypes::BEHAVIOR_CIRCLE: {
            Vertex3D center = getVertex3DFromJSONPosition(cJSON_GetObjectItemCaseSensitive(motion, "center"), depth);

            float speed = (float) cJSON_GetObjectItemCaseSensitive(motion, "speed")->valuedouble;
            float radius = (float) cJSON_GetObjectItemCaseSensitive(motion, "radius")->valuedouble;

            auto behavior = new EnemyBehaviorCircle(center, speed, radius);
            enemy->setBehavior(behavior);
            break;
        }
        case EnemyBehaviorTypes::BEHAVIOR_RANDOM: {
            float speed = (float) cJSON_GetObjectItemCaseSensitive(motion, "speed")->valuedouble;

            auto behavior = new EnemyBehaviorRandom(speed);
            enemy->setBehavior(behavior);
            break;
        }
        case EnemyBehaviorTypes::BEHAVIOR_PATH: {
            float speed = (float) cJSON_GetObjectItemCaseSensitive(motion, "speed")->valuedouble;
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
    }
}

Point2D LevelLoader::parsePositionJSON(cJSON *positionJSON)
{
    const int x = (cJSON_GetObjectItemCaseSensitive(positionJSON, "x")->valueint * EngineSetup::get()->screenWidth) / 100;
    const int y = (cJSON_GetObjectItemCaseSensitive(positionJSON, "y")->valueint * EngineSetup::get()->screenHeight) / 100;

    return Point2D(x, y);
}

Vertex3D LevelLoader::parseVertex3DJSON(cJSON *vertex3DJSON)
{
    const auto x = (float) cJSON_GetObjectItemCaseSensitive(vertex3DJSON, "x")->valuedouble;
    const auto y = (float) cJSON_GetObjectItemCaseSensitive(vertex3DJSON, "y")->valuedouble;
    const auto z = (float) cJSON_GetObjectItemCaseSensitive(vertex3DJSON, "z")->valuedouble;

    return Vertex3D(x, y, z);
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

Point2D LevelLoader::convertPointPercentRelativeToScreen(Point2D point)
{
    return Point2D(
        ( point.x * EngineSetup::get()->screenWidth) / 100,
        ( point.y * EngineSetup::get()->screenHeight) / 100
    );
}

Vertex3D LevelLoader::getVertex3DFromJSONPosition(cJSON *positionJSON, float depth)
{
    Point2D fixedPosition = parsePositionJSON(positionJSON);
    return getPositionFromScreenPoint(fixedPosition, depth);
}

Vertex3D LevelLoader::getPositionFromScreenPoint(Point2D fixedPosition, float depth)
{
    auto camera = ComponentsManager::get()->getComponentCamera()->getCamera();

    Vertex3D nearPlaneVertex = Transforms::Point2DToWorld(fixedPosition, camera);
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
            auto item = this->makeItemEnergyGhost(position);
            item->setHasTutorial(help);
            item->setTutorialIndex(helpIndex);
            break;
        }
        case LevelInfoItemsTypes::ITEM_HEALTH: {
            auto item = this->makeItemHealthGhost(position);
            item->setHasTutorial(help);
            item->setTutorialIndex(helpIndex);
            break;
        }
        case LevelInfoItemsTypes::ITEM_WEAPON_PROJECTILE: {
            auto item = this->makeItemWeapon(WeaponTypes::WEAPON_PROJECTILE, position);
            item->setHasTutorial(help);
            item->setTutorialIndex(helpIndex);
            break;
        }
        case LevelInfoItemsTypes::ITEM_WEAPON_RAYLIGHT: {
            auto item = this->makeItemWeapon(WeaponTypes::WEAPON_RAYLIGHT, position);
            item->setHasTutorial(help);
            item->setTutorialIndex(helpIndex);
            break;
        }
        case LevelInfoItemsTypes::ITEM_WEAPON_LASER: {
            auto item = this->makeItemWeapon(WeaponTypes::WEAPON_LASER, position);
            item->setHasTutorial(help);
            item->setTutorialIndex(helpIndex);
            break;
        }
        case LevelInfoItemsTypes::ITEM_WEAPON_BOMB: {
            auto item = this->makeItemWeapon(WeaponTypes::WEAPON_BOMB, position);
            item->setHasTutorial(help);
            item->setTutorialIndex(helpIndex);
            break;
        }
        case LevelInfoItemsTypes::ITEM_WEAPON_REFLECTION: {
            auto item = this->makeItemWeapon(WeaponTypes::WEAPON_REFLECTION, position);
            item->setHasTutorial(help);
            item->setTutorialIndex(helpIndex);
            break;
        }
        case LevelInfoItemsTypes::ITEM_SHIELD: {
            auto item = this->makeItemWeapon(WeaponTypes::WEAPON_SHIELD, position);
            item->setHasTutorial(help);
            item->setTutorialIndex(helpIndex);
            break;
        }
        case LevelInfoItemsTypes::ITEM_HUMAN: {
            auto item = this->makeItemHuman(position);
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
        case BossesTypes::BOSS_LEVEL_20: {
            boss = new BossLevel20();
            break;
        }
        case BossesTypes::BOSS_LEVEL_30: {
            boss = new BossLevel30();
            boss->load();
            break;
        }
    }

    parseEnemyJSON(bossJSON, boss);

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

AsteroidEnemyGhost* LevelLoader::parseAsteroidJSON(cJSON *asteroidJSON)
{
    std::string name = cJSON_GetObjectItemCaseSensitive(asteroidJSON, "name")->valuestring;
    std::string model = cJSON_GetObjectItemCaseSensitive(asteroidJSON, "model")->valuestring;
    int stamina = cJSON_GetObjectItemCaseSensitive(asteroidJSON, "stamina")->valueint;
    bool explode = cJSON_GetObjectItemCaseSensitive(asteroidJSON, "explode")->valueint;
    int speed = cJSON_GetObjectItemCaseSensitive(asteroidJSON, "speed")->valueint;
    cJSON *motion = cJSON_GetObjectItemCaseSensitive(asteroidJSON, "motion");

    Vertex3D worldPosition = getVertex3DFromJSONPosition(cJSON_GetObjectItemCaseSensitive(asteroidJSON, "position"), Z_COORDINATE_GAMEPLAY);

    EnemyBehavior *behavior;
    const int typeMotion = cJSON_GetObjectItemCaseSensitive(motion, "type")->valueint;
    switch(typeMotion) {
        case EnemyBehaviorTypes::BEHAVIOR_PATROL: {
            Vertex3D from = getVertex3DFromJSONPosition(cJSON_GetObjectItemCaseSensitive(motion, "from"), Z_COORDINATE_GAMEPLAY);
            Vertex3D to = getVertex3DFromJSONPosition(cJSON_GetObjectItemCaseSensitive(motion, "to"), Z_COORDINATE_GAMEPLAY);

            float behaviorSpeed = (float) cJSON_GetObjectItemCaseSensitive(motion, "speed")->valuedouble;
            behavior = new EnemyBehaviorPatrol(from, to, behaviorSpeed);
            break;
        }
        case EnemyBehaviorTypes::BEHAVIOR_FOLLOW: {
            behavior = new EnemyBehaviorFollow(
                ComponentsManager::get()->getComponentGame()->getPlayer(),
                (float) cJSON_GetObjectItemCaseSensitive(motion, "speed")->valueint,
                (float) cJSON_GetObjectItemCaseSensitive(motion, "separation")->valueint
            );
            break;
        }
        case EnemyBehaviorTypes::BEHAVIOR_CIRCLE: {
            Vertex3D center = getVertex3DFromJSONPosition(cJSON_GetObjectItemCaseSensitive(motion, "center"), Z_COORDINATE_GAMEPLAY);

            behavior = new EnemyBehaviorCircle(
                center,
                (float) cJSON_GetObjectItemCaseSensitive(motion, "speed")->valuedouble,
                (float) cJSON_GetObjectItemCaseSensitive(motion, "radius")->valuedouble
            );
            break;
        }
        case EnemyBehaviorTypes::BEHAVIOR_RANDOM: {
            float speed = (float) cJSON_GetObjectItemCaseSensitive(motion, "speed")->valuedouble;
            behavior = new EnemyBehaviorRandom(speed);
            break;
        }
    }

    auto *asteroid = new AsteroidEnemyGhost();
    asteroid->setEnabled(true);
    asteroid->setLabel(Brakeza3D::uniqueObjectLabel("npc"));
    asteroid->setEnableLights(false);
    asteroid->setPosition(worldPosition);
    asteroid->setStencilBufferEnabled(true);
    asteroid->setScale(1);
    asteroid->setStamina(stamina);
    asteroid->setStartStamina(stamina);
    asteroid->setEnableLights(true);
    asteroid->AssimpLoadGeometryFromFile(std::string(EngineSetup::get()->MODELS_FOLDER + model));
    asteroid->makeGhostBody(
        ComponentsManager::get()->getComponentCollisions()->getDynamicsWorld(),
        asteroid,
        EngineSetup::collisionGroups::Enemy,
        EngineSetup::collisionGroups::Player
    );
    asteroid->setSoundChannel(enemiesEmitter.size() + 2);
    asteroid->setExplode(explode);

    if (explode) {
        std::string modelPartitions = cJSON_GetObjectItemCaseSensitive(asteroidJSON, "modelPartitions")->valuestring;
        auto mesh = new Mesh3D();
        mesh->AssimpLoadGeometryFromFile(std::string(EngineSetup::get()->MODELS_FOLDER + modelPartitions));
        asteroid->setModelPartitions(mesh);

        int numberPartitions = cJSON_GetObjectItemCaseSensitive(asteroidJSON, "numberPartitions")->valueint;
        asteroid->setExplodeNumberPartitions(numberPartitions);

    }

    if (typeMotion > 0) {
        asteroid->setBehavior(behavior);
    }

    return asteroid;
}

Color LevelLoader::parseColorJSON(cJSON *color)
{
    return Color(
        cJSON_GetObjectItemCaseSensitive(color, "r")->valueint,
        cJSON_GetObjectItemCaseSensitive(color, "g")->valueint,
        cJSON_GetObjectItemCaseSensitive(color, "b")->valueint
    );
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

void LevelLoader::parseBackgroundItem(cJSON *object)
{
    std::string name = cJSON_GetObjectItemCaseSensitive(object, "name")->valuestring;
    std::string model = cJSON_GetObjectItemCaseSensitive(object, "model")->valuestring;
    cJSON *rotationJSON = cJSON_GetObjectItemCaseSensitive(object, "rotation");
    float scale = (float) cJSON_GetObjectItemCaseSensitive(object, "scale")->valuedouble;
    cJSON *positionJSON = cJSON_GetObjectItemCaseSensitive(object, "position");
    cJSON *motion = cJSON_GetObjectItemCaseSensitive(object, "motion");

    auto mesh = new Mesh3D();
    mesh->setScale(scale);
    mesh->setRotation(M3::getMatrixRotationForEulerAngles(
        (float) cJSON_GetObjectItemCaseSensitive(rotationJSON, "x")->valueint,
        (float) cJSON_GetObjectItemCaseSensitive(rotationJSON, "y")->valueint,
        (float) cJSON_GetObjectItemCaseSensitive(rotationJSON, "z")->valueint
    ));
    mesh->setRotationFrameEnabled(true);
    mesh->setRotationFrame(parseVertex3DJSON(cJSON_GetObjectItemCaseSensitive(object, "rotationFrame")));
    mesh->AssimpLoadGeometryFromFile(EngineSetup::get()->MODELS_FOLDER + model);

    auto position = getVertex3DFromJSONPosition(positionJSON, (float) cJSON_GetObjectItemCaseSensitive(positionJSON, "z")->valueint);

    mesh->setPosition(position);

    if (motion != nullptr) {
        setBehaviorFromJSON(motion, mesh, position.z);
    }
    objectsBackground.emplace_back(mesh);

    Brakeza3D::get()->addObject3D(mesh, name);
}

void LevelLoader::removeBackgroundObjects()
{
    for (auto o : objectsBackground) {
        o->setRemoved(true);
    }
    objectsBackground.clear();
}

void LevelLoader::moveBackgroundObjects(Vertex3D offset)
{
    for (auto o : objectsBackground) {
        o->addToPosition(offset);
    }
}

void LevelLoader::parseMessageJSON(cJSON *message, EnemyGhost *enemy)
{
    auto componentGame = ComponentsManager::get()->getComponentGame();

    std::string text = cJSON_GetObjectItemCaseSensitive(message, "text")->valuestring;

    auto stamina = (float) cJSON_GetObjectItemCaseSensitive(message, "stamina")->valuedouble;

    auto dialog = new EnemyDialog(490, 40, stamina, text.c_str(), 5, componentGame->getFontGame(), componentGame->getPrimaryColor());

    enemy->dialogs.push_back(dialog);
}

std::vector<Image*> &LevelLoader::getTutorials() {
    return levelHistories;
}

void LevelLoader::drawCurrentTutorialImage(float alpha)
{
    this->levelHistories[currentTutorialIndex]->drawFlatAlpha(0, 0, alpha);
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
