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

#include <utility>

LevelLoader::LevelLoader(std::string filename)
{
    stats = new LevelStats();

    addLevel(std::move(filename));
    setLevelStartedToPlay(false);
    setCurrentLevelIndex(-1);
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
    Logging::Log("Loading Enemies for Level: %s", filePath.c_str());

    enemiesEmitter.resize(0);

    hasTutorial = false;

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

    if (cJSON_GetObjectItemCaseSensitive(jsonContentFile, "tutorialImage") != nullptr) {
        //delete tutorialImage;
        //tutorialImage = new Image(EngineSetup::get()->IMAGES_FOLDER + cJSON_GetObjectItemCaseSensitive(jsonContentFile, "tutorialImage")->valuestring);
        //hasTutorial = true;
    }

    ComponentsManager::get()->getComponentGame()->getPlayer()->setAllowGravitationalShields(
        cJSON_GetObjectItemCaseSensitive(jsonContentFile, "allowGravitionalShield")->valueint
    );

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
        Brakeza3D::get()->addObject3D(respawner, "respawner_" + ComponentsManager::get()->getComponentRender()->getUniqueGameObjectLabel());
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
        Brakeza3D::get()->addObject3D(respawner, "asteroid_" + ComponentsManager::get()->getComponentRender()->getUniqueGameObjectLabel());
        enemiesEmitter.push_back(respawner);
    }

    free(contentFile);
    cJSON_Delete(jsonContentFile);
}

Weapon *LevelLoader::parseWeaponJSON(cJSON *weaponJson)
{
    int index = cJSON_GetObjectItemCaseSensitive(weaponJson, "index")->valueint;

    Logging::Log("Loading Weapon with index: %d", index);

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
        (bool) cJSON_GetObjectItemCaseSensitive(weaponJson, "available")->valueint
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

bool LevelLoader::isHasTutorial() const {
    return hasTutorial;
}

Image* LevelLoader::getTutorialImage() const {
    return tutorialImage;
}

void LevelLoader::makeItemHealthGhost(Vertex3D position)
{
    auto *healthItem = new ItemHealthGhost();
    healthItem->setLabel("item_level_health_" + ComponentsManager::get()->getComponentRender()->getUniqueGameObjectLabel());
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
}

void LevelLoader::makeItemEnergyGhost(Vertex3D position)
{
    auto *healthItem = new ItemEnergyGhost();
    healthItem->setLabel("item_level_energy_" + ComponentsManager::get()->getComponentRender()->getUniqueGameObjectLabel());
    healthItem->setEnableLights(true);
    healthItem->setPosition(position);
    healthItem->setRotationFrameEnabled(true);
    healthItem->setRotationFrame(Tools::randomVertex().getScaled(0.5));
    healthItem->setStencilBufferEnabled(true);
    healthItem->setScale(1);
    healthItem->AssimpLoadGeometryFromFile(std::string(EngineSetup::get()->MODELS_FOLDER + "pill.fbx"));
    healthItem->makeSimpleGhostBody(
        Vertex3D(500, 500, 500),
        ComponentsManager::get()->getComponentCollisions()->getDynamicsWorld(),
        EngineSetup::collisionGroups::Weapon,
        EngineSetup::collisionGroups::Player
    );

    healthItem->updateBulletFromMesh3D();
    Brakeza3D::get()->addObject3D(healthItem, healthItem->getLabel());
}

void LevelLoader::makeItemWeapon(int indexWeapon, Vertex3D position)
{
    auto weapons = ComponentsManager::get()->getComponentGame()->getWeapons();
    auto *weaponItem = new ItemWeaponGhost(weapons[indexWeapon]);
    weaponItem->setLabel("item_weapon_" + ComponentsManager::get()->getComponentRender()->getUniqueGameObjectLabel());
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

    Vertex3D worldPosition = getVertex3DFromJSONPosition(cJSON_GetObjectItemCaseSensitive(enemyJSON, "position"));

    enemy->setRewards(reward);

    this->setBehaviorFromJSON(motion, enemy);
    this->setLasersForEnemy(lasers, enemy);

    enemy->setEnabled(true);
    enemy->setLabel("npc_" + ComponentsManager::get()->getComponentRender()->getUniqueGameObjectLabel());
    enemy->setEnableLights(enableLights);
    enemy->setPosition(worldPosition);
    enemy->setStencilBufferEnabled(true);
    enemy->onStart();
    enemy->setScale(1);
    enemy->setStamina(stamina);
    enemy->setStartStamina(stamina);
    if (animated) {
        enemy->AssimpLoadAnimation(EngineSetup::get()->MODELS_FOLDER + model);
    } else {
        enemy->AssimpLoadGeometryFromFile(std::string(EngineSetup::get()->MODELS_FOLDER + model));
    }
    enemy->updateBoundingBox();
    enemy->makeSimpleGhostBody(
        enemy->getAabb().size().getScaled(0.5),
        ComponentsManager::get()->getComponentCollisions()->getDynamicsWorld(),
        EngineSetup::collisionGroups::Enemy,
        EngineSetup::collisionGroups::Projectile | EngineSetup::collisionGroups::Player
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
}

void LevelLoader::setBehaviorFromJSON(cJSON *motion, EnemyGhost *enemy)
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
            Vertex3D from = getVertex3DFromJSONPosition(cJSON_GetObjectItemCaseSensitive(motion, "from"));
            Vertex3D to = getVertex3DFromJSONPosition(cJSON_GetObjectItemCaseSensitive(motion, "to"));

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
            Vertex3D center = getVertex3DFromJSONPosition(cJSON_GetObjectItemCaseSensitive(motion, "center"));

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
                Vertex3D position = getVertex3DFromJSONPosition(currentPoint);
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

    projectileEmitter->setLabel("boss_emitter_" + ComponentsManager::get()->getComponentRender()->getUniqueGameObjectLabel());

    enemy->setProjectileEmitter(projectileEmitter);
}

Point2D LevelLoader::convertPointPercentRelativeToScreen(Point2D point)
{
    return Point2D(
        ( point.x * EngineSetup::get()->screenWidth) / 100,
        ( point.y * EngineSetup::get()->screenHeight) / 100
    );
}

Vertex3D LevelLoader::getVertex3DFromJSONPosition(cJSON *positionJSON)
{
    Point2D fixedPosition = parsePositionJSON(positionJSON);
    return getWorldPositionFromScreenPoint(fixedPosition);

}

Vertex3D LevelLoader::getWorldPositionFromScreenPoint(Point2D fixedPosition)
{
    auto camera = ComponentsManager::get()->getComponentCamera()->getCamera();

    Vertex3D nearPlaneVertex = Transforms::Point2DToWorld(fixedPosition, camera);
    Vector3D ray(camera->getPosition(),nearPlaneVertex);

    return ray.getComponent().getScaled(Z_COORDINATE_GAMEPLAY);
}

void LevelLoader::parseItemJSON(cJSON *itemJSON)
{
    const int typeItem = cJSON_GetObjectItemCaseSensitive(itemJSON, "type")->valueint;

    Vertex3D position = getVertex3DFromJSONPosition(cJSON_GetObjectItemCaseSensitive(itemJSON, "position"));

    switch(typeItem) {
        case LevelInfoItemsTypes::ITEM_ENERGY: {
            this->makeItemEnergyGhost(position);
            break;
        }
        case LevelInfoItemsTypes::ITEM_HEALTH: {
            this->makeItemHealthGhost(position);
            break;
        }
        case LevelInfoItemsTypes::ITEM_WEAPON_PROJECTILE: {
            this->makeItemWeapon(WeaponTypes::WEAPON_PROJECTILE, position);
            break;
        }
        case LevelInfoItemsTypes::ITEM_WEAPON_BOMB: {
            this->makeItemWeapon(WeaponTypes::WEAPON_BOMB, position);
            break;
        }
        case LevelInfoItemsTypes::ITEM_WEAPON_LASER: {
            this->makeItemWeapon(WeaponTypes::WEAPON_LASER_RAY, position);
            break;
        }
        case LevelInfoItemsTypes::ITEM_WEAPON_SMART: {
            this->makeItemWeapon(WeaponTypes::WEAPON_LASER_PROJECTILE, position);
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

    Brakeza3D::get()->addObject3D(respawner, "respawner_" + ComponentsManager::get()->getComponentRender()->getUniqueGameObjectLabel());
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

    Vertex3D worldPosition = getVertex3DFromJSONPosition(cJSON_GetObjectItemCaseSensitive(asteroidJSON, "position"));

    EnemyBehavior *behavior;
    const int typeMotion = cJSON_GetObjectItemCaseSensitive(motion, "type")->valueint;
    switch(typeMotion) {
        case EnemyBehaviorTypes::BEHAVIOR_PATROL: {
            Vertex3D from = getVertex3DFromJSONPosition(cJSON_GetObjectItemCaseSensitive(motion, "from"));
            Vertex3D to = getVertex3DFromJSONPosition(cJSON_GetObjectItemCaseSensitive(motion, "to"));

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
            Vertex3D center = getVertex3DFromJSONPosition(cJSON_GetObjectItemCaseSensitive(motion, "center"));

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
    asteroid->setLabel("npc_" + ComponentsManager::get()->getComponentRender()->getUniqueGameObjectLabel());
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
