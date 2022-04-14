//
// Created by eduardo on 20/3/22.
//

#include "../../include/Game/LevelLoader.h"
#include "../../include/Brakeza3D.h"
#include "../../include/Render/Transforms.h"
#include "../../include/Game/EnemyGhostRespawner.h"
#include "../../include/Game/EnemyBehaviorCircle.h"
#include "../../include/Game/EnemyBehaviorPatrol.h"
#include "../../include/Game/EnemyBehaviorFollow.h"
#include "../../include/Game/ItemHealthGhost.h"
#include "../../include/Game/ItemEnergyGhost.h"
#include "../../include/Game/ItemWeaponGhost.h"
#include "../../include/Game/EnemyGhostRespawnerEmissor.h"
#include "../../include/Game/BossEnemy.h"

#include <utility>

LevelLoader::LevelLoader(std::string filename)
{
    addLevel(filename);
    setLevelStartedToPlay(false);
    setCurrentLevelIndex(-1);
    countDown = new Counter();
    tutorialImage = new Image();
}

void LevelLoader::load(int levelIndex)
{
    Logging::getInstance()->Log("loading level: " + std::to_string(levelIndex));
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

int LevelLoader::getNumberLevelEnemies() const
{
    return respawners.size();
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

void LevelLoader::loadLevelFromJSON(std::string filePath)
{
    Logging::Log("Loading Enemies for level...", "LEVEL_LOADER");

    std::string sndPath = EngineSetup::get()->SOUNDS_FOLDER;

    respawners.resize(0);
    hasTutorial = false;
    if (tutorialImage != nullptr) {
        delete(tutorialImage);
        tutorialImage = new Image();
    }

    size_t file_size;
    const char *weaponsFile = Tools::readFile(filePath, file_size);
    cJSON *myDataJSON = cJSON_Parse(weaponsFile);

    if (myDataJSON == nullptr) {
        Logging::Log(filePath + " can't be loaded", "ERROR");
        return;
    }

    if (cJSON_GetObjectItemCaseSensitive(myDataJSON, "music") != nullptr) {
        setMusic(cJSON_GetObjectItemCaseSensitive(myDataJSON, "music")->valuestring);
        this->hasMusic = true;
    }

    setLevelName(cJSON_GetObjectItemCaseSensitive(myDataJSON, "name")->valuestring);

    std::string backgroundImage = cJSON_GetObjectItemCaseSensitive(myDataJSON, "backgroundImage")->valuestring;
    auto shaderBackground = dynamic_cast<ShaderImageBackground*> (ComponentsManager::get()->getComponentRender()->getShaderByType(EngineSetup::ShaderTypes::BACKGROUND));
    shaderBackground->setImage(new Image(EngineSetup::get()->IMAGES_FOLDER + backgroundImage));

    if (cJSON_GetObjectItemCaseSensitive(myDataJSON, "tutorialImage") != nullptr) {
        std::string tutorialImagePath = cJSON_GetObjectItemCaseSensitive(myDataJSON, "tutorialImage")->valuestring;
        this->tutorialImage->loadTGA(EngineSetup::get()->IMAGES_FOLDER + tutorialImagePath);
        this->hasTutorial = true;
    }

    ComponentsManager::get()->getComponentGame()->getPlayer()->setAllowGravitationalShields(
        cJSON_GetObjectItemCaseSensitive(myDataJSON, "allowGravitionalShield")->valueint
    );

    ComponentsManager::get()->getComponentGame()->getPlayer()->setAllowEnergyShield(
        cJSON_GetObjectItemCaseSensitive(myDataJSON, "allowEnergyShield")->valueint
    );

    ComponentsManager::get()->getComponentCamera()->getCamera()->frustum->updateFrustum();

    cJSON *enemiesList = cJSON_GetObjectItemCaseSensitive(myDataJSON, "enemies");
    cJSON *currentEnemy;
    cJSON_ArrayForEach(currentEnemy, enemiesList) {
        auto enemy = parseEnemyJSON(currentEnemy);

        auto respawner = new EnemyGhostRespawner(enemy, (int) Tools::random(1, 3));
        respawner->setPosition(enemy->getPosition());
        Brakeza3D::get()->addObject3D(respawner, "respawner_" + ComponentsManager::get()->getComponentRender()->getUniqueGameObjectLabel());
        respawners.push_back(respawner);
    }

    cJSON *items = cJSON_GetObjectItemCaseSensitive(myDataJSON, "items");
    cJSON *currentItem;
    cJSON_ArrayForEach(currentItem, items) {
        this->parseItemJSON(currentItem);
    }

    cJSON *emissors = cJSON_GetObjectItemCaseSensitive(myDataJSON, "emissors");
    cJSON *currentEmissor;
    cJSON_ArrayForEach(currentEmissor, emissors) {
        this->parseEmissorJSON(currentEmissor);
    }

    cJSON *bosses = cJSON_GetObjectItemCaseSensitive(myDataJSON, "bosses");
    cJSON *currentBoss;
    cJSON_ArrayForEach(currentBoss, bosses) {
        this->parseBossJSON(currentBoss);
    }
}

Weapon *LevelLoader::parseWeaponJSON(cJSON *weaponJson)
{
    int index = cJSON_GetObjectItemCaseSensitive(weaponJson, "index")->valueint;
    std::string name = cJSON_GetObjectItemCaseSensitive(weaponJson, "name")->valuestring;
    std::string modelProjectile = cJSON_GetObjectItemCaseSensitive(weaponJson, "modelProjectile")->valuestring;
    std::string model = cJSON_GetObjectItemCaseSensitive(weaponJson, "model")->valuestring;
    int amount = cJSON_GetObjectItemCaseSensitive(weaponJson, "amount")->valueint;
    int startAmount = cJSON_GetObjectItemCaseSensitive(weaponJson, "startAmount")->valueint;
    float speed = (float) cJSON_GetObjectItemCaseSensitive(weaponJson, "speed")->valuedouble;
    float dispersion = (float)cJSON_GetObjectItemCaseSensitive(weaponJson, "dispersion")->valuedouble;
    float accuracy = (float)cJSON_GetObjectItemCaseSensitive(weaponJson, "accuracy")->valuedouble;
    float damage = (float)cJSON_GetObjectItemCaseSensitive(weaponJson, "damage")->valuedouble;
    int type = cJSON_GetObjectItemCaseSensitive(weaponJson, "type")->valueint;
    float cadenceTime = (float) cJSON_GetObjectItemCaseSensitive(weaponJson, "cadenceTime")->valuedouble;
    bool stop = (bool) cJSON_GetObjectItemCaseSensitive(weaponJson, "stop")->valueint;
    float stopDuration = (float) cJSON_GetObjectItemCaseSensitive(weaponJson, "stopDuration")->valuedouble;
    float stopEvery = (float) cJSON_GetObjectItemCaseSensitive(weaponJson, "stopEvery")->valuedouble;
    bool available = (bool) cJSON_GetObjectItemCaseSensitive(weaponJson, "available")->valueint;

    auto weapon = new Weapon(name);
    weapon->getModel()->AssimpLoadGeometryFromFile(std::string(EngineSetup::get()->MODELS_FOLDER + model));
    weapon->getModelProjectile()->setFlatTextureColor(true);
    weapon->getModelProjectile()->setFlatColor(Color::cyan());
    weapon->getModelProjectile()->AssimpLoadGeometryFromFile(std::string(EngineSetup::get()->MODELS_FOLDER + modelProjectile));
    weapon->getModelProjectile()->setLabel("projectile_enemy_template" + std::to_string(index));
    weapon->getModelProjectile()->setScale(1);
    weapon->setAmmoAmount(amount);
    weapon->setStartAmmoAmount(startAmount);
    weapon->setSpeed(speed);
    weapon->setDamage(damage);
    weapon->setDispersion(dispersion);
    weapon->setAccuracy(accuracy);
    weapon->setCadenceTime(cadenceTime);
    weapon->setType(type);

    if (cJSON_GetObjectItemCaseSensitive(weaponJson, "icon") != nullptr) {
        weapon->setIconImage(EngineSetup::get()->ASSETS_FOLDER + cJSON_GetObjectItemCaseSensitive(weaponJson, "icon")->valuestring);
    }

    weapon->setStop(stop);
    weapon->setAvailable(available);

    if (stop) {
        Logging::getInstance()->Log("Arma con parada de " +std::to_string(stopDuration)+" cada "  + std::to_string(stopEvery));

        weapon->setStopDuration(stopDuration);
        weapon->setStopEvery(stopEvery);
    }

    return weapon;
}

void LevelLoader::startCountDown()
{
    this->countDown->setStep(3);
    this->countDown->setEnabled(true);

    for (auto respawner : respawners) {
        respawner->startCounter();
    }
}

const std::string &LevelLoader::getMusic() const {
    return music;
}

void LevelLoader::setMusic(const std::string &music) {
    LevelLoader::music = music;
}

const std::string &LevelLoader::getLevelName() const {
    return levelName;
}

void LevelLoader::setLevelName(const std::string &levelName) {
    LevelLoader::levelName = levelName;
}

Counter *LevelLoader::getCountDown() const {
    return countDown;
}

bool LevelLoader::isHasTutorial() const {
    return hasTutorial;
}

Image *LevelLoader::getTutorialImage() const {
    return tutorialImage;
}


void LevelLoader::makeItemHealthGhost(Vertex3D position)
{
    auto *healthItem = new ItemHealthGhost();
    healthItem->setLabel("item_level_health_" + ComponentsManager::get()->getComponentRender()->getUniqueGameObjectLabel());
    healthItem->setEnableLights(true);
    healthItem->setPosition(position);
    healthItem->setRotationFrameEnabled(true);
    healthItem->setRotationFrame(Vertex3D(0, 1, 0));
    healthItem->setStencilBufferEnabled(true);
    healthItem->setScale(1);
    healthItem->AssimpLoadGeometryFromFile(std::string(EngineSetup::get()->MODELS_FOLDER + "red_pill.fbx"));
    healthItem->makeGhostBody(ComponentsManager::get()->getComponentCollisions()->getDynamicsWorld(), healthItem, EngineSetup::collisionGroups::Health, EngineSetup::collisionGroups::Player);
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
    healthItem->setRotationFrame(Vertex3D(0, 1, 0));
    healthItem->setStencilBufferEnabled(true);
    healthItem->setScale(1);
    healthItem->AssimpLoadGeometryFromFile(std::string(EngineSetup::get()->MODELS_FOLDER + "pill.fbx"));
    healthItem->makeGhostBody(ComponentsManager::get()->getComponentCollisions()->getDynamicsWorld(), healthItem, EngineSetup::collisionGroups::Health, EngineSetup::collisionGroups::Player);
    healthItem->updateBulletFromMesh3D();
    Brakeza3D::get()->addObject3D(healthItem, healthItem->getLabel());
}

void LevelLoader::makeItemWeapon(int indexWeapon, Vertex3D position)
{
    auto weapons = ComponentsManager::get()->getComponentGame()->getWeapons();
    auto *weaponItem = new ItemWeaponGhost(weapons[indexWeapon]);
    weaponItem->setLabel("item_weapon_" + ComponentsManager::get()->getComponentRender()->getUniqueGameObjectLabel());
    weaponItem->setEnableLights(false);
    weaponItem->copyFrom(weapons[indexWeapon]->getModel());
    weaponItem->setPosition(position);
    weaponItem->makeGhostBody(ComponentsManager::get()->getComponentCollisions()->getDynamicsWorld(), weaponItem, EngineSetup::collisionGroups::Weapon, EngineSetup::collisionGroups::Player);
    weaponItem->setRotation(0, 0, 180);
    weaponItem->setRotationFrameEnabled(true);
    weaponItem->setRotationFrame(Vertex3D(0, 1, 0));
    weaponItem->setStencilBufferEnabled(true);
    weaponItem->setScale(1);
    weaponItem->updateBulletFromMesh3D();

    Brakeza3D::get()->addObject3D(weaponItem, weaponItem->getLabel());
}

bool LevelLoader::isLevelFinished() const {
    return levelFinished;
}

void LevelLoader::setLevelFinished(bool levelFinished) {
    LevelLoader::levelFinished = levelFinished;
}

EnemyGhost * LevelLoader::parseEnemyJSON(cJSON *enemyJSON)
{
    auto camera = ComponentsManager::get()->getComponentCamera()->getCamera();

    std::string name = cJSON_GetObjectItemCaseSensitive(enemyJSON, "name")->valuestring;
    std::string model = cJSON_GetObjectItemCaseSensitive(enemyJSON, "model")->valuestring;
    int stamina = cJSON_GetObjectItemCaseSensitive(enemyJSON, "stamina")->valueint;
    int speed = cJSON_GetObjectItemCaseSensitive(enemyJSON, "speed")->valueint;
    cJSON *motion = cJSON_GetObjectItemCaseSensitive(enemyJSON, "motion");
    cJSON *weapon = cJSON_GetObjectItemCaseSensitive(enemyJSON, "weapon");

    Vertex3D worldPosition = getVertex3DFromJSONPosition(cJSON_GetObjectItemCaseSensitive(enemyJSON, "position"));

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
                cJSON_GetObjectItemCaseSensitive(motion, "speed")->valueint,
                cJSON_GetObjectItemCaseSensitive(motion, "separation")->valueint
            );
            break;
        }
        case EnemyBehaviorTypes::BEHAVIOR_CIRCLE: {
            Vertex3D center = getVertex3DFromJSONPosition(cJSON_GetObjectItemCaseSensitive(motion, "center"));

            float speed = (float) cJSON_GetObjectItemCaseSensitive(motion, "speed")->valuedouble;
            float radius = (float) cJSON_GetObjectItemCaseSensitive(motion, "radius")->valuedouble;
            behavior = new EnemyBehaviorCircle(center, speed, radius);
            break;
        }
    }

    auto *enemy = new EnemyGhost();
    enemy->setEnabled(true);
    enemy->setLabel("npc_" + ComponentsManager::get()->getComponentRender()->getUniqueGameObjectLabel());
    enemy->setEnableLights(false);
    enemy->setPosition(worldPosition);
    enemy->setStencilBufferEnabled(true);
    enemy->setScale(1);
    enemy->setSpeed(speed);
    enemy->setStamina(stamina);
    enemy->setStartStamina(stamina);
    enemy->setEnableLights(true);
    enemy->AssimpLoadGeometryFromFile(std::string(EngineSetup::get()->MODELS_FOLDER + model));
    enemy->makeGhostBody(ComponentsManager::get()->getComponentCollisions()->getDynamicsWorld(), enemy, EngineSetup::collisionGroups::Enemy, EngineSetup::collisionGroups::AllFilter);
    enemy->setSoundChannel(respawners.size() + 2);

    if (typeMotion > 0) {
        enemy->setBehavior(behavior);
    }

    if (weapon != nullptr) {
        auto weaponType = parseWeaponJSON(weapon);
        weaponType->setSoundChannel(enemy->getSoundChannel());
        enemy->setWeaponType(weaponType);
    }

    return enemy;
}

Point2D LevelLoader::parsePositionJSON(cJSON *positionJSON)
{
    const int x = (cJSON_GetObjectItemCaseSensitive(positionJSON, "x")->valueint * EngineSetup::get()->screenWidth) / 100;
    const int y = (cJSON_GetObjectItemCaseSensitive(positionJSON, "y")->valueint * EngineSetup::get()->screenHeight) / 100;
    return Point2D(x, y);
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

    return ray.getComponent().getScaled(ComponentsManager::get()->getComponentGame()->Z_COORDINATE_GAMEPLAY);
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
        case LevelInfoItemsTypes::ITEM_WEAPON_INSTANT: {
            this->makeItemWeapon(WeaponTypes::WEAPON_INSTANT, position);
            break;
        }
        case LevelInfoItemsTypes::ITEM_WEAPON_PROJECTILE: {
            this->makeItemWeapon(WeaponTypes::WEAPON_PROJECTILE, position);
            break;
        }
        case LevelInfoItemsTypes::ITEM_WEAPON_SMART: {
            this->makeItemWeapon(WeaponTypes::WEAPON_SMART_PROJECTILE, position);
            break;
        }
    }
}

void LevelLoader::parseEmissorJSON(cJSON *emissorJSON)
{
    auto emissor = parseEnemyJSON(emissorJSON);

    auto enemyGenerated = parseEnemyJSON(cJSON_GetObjectItemCaseSensitive(emissorJSON, "enemy"));

    auto *enemiesEmissor = new EnemyGhostRespawnerEmissor(5, enemyGenerated);
    enemiesEmissor->copyFrom(emissor);
    enemiesEmissor->setStamina(emissor->getStamina());
    enemiesEmissor->setStartStamina(emissor->getStamina());
    enemiesEmissor->setPosition(emissor->getPosition());
    enemiesEmissor->makeGhostBody(ComponentsManager::get()->getComponentCollisions()->getDynamicsWorld(), enemiesEmissor, EngineSetup::collisionGroups::Enemy, EngineSetup::collisionGroups::AllFilter);

    Brakeza3D::get()->addObject3D(enemiesEmissor, "enemies_emissor_" + ComponentsManager::get()->getComponentRender()->getUniqueGameObjectLabel());

}

void LevelLoader::parseBossJSON(cJSON *bossJSON)
{
    const int typeItem = cJSON_GetObjectItemCaseSensitive(bossJSON, "type")->valueint;
    Vertex3D position = getVertex3DFromJSONPosition(cJSON_GetObjectItemCaseSensitive(bossJSON, "position"));

    switch(typeItem) {
        case BossesTypes::BOSS_LEVEL_10: {
            auto weapon = new Weapon("boss_weapon");
            weapon->getModel()->AssimpLoadGeometryFromFile(std::string(EngineSetup::get()->MODELS_FOLDER + "projectile_weapon_01.fbx"));
            weapon->getModelProjectile()->setFlatTextureColor(true);
            weapon->getModelProjectile()->setFlatColor(Color::cyan());
            weapon->getModelProjectile()->AssimpLoadGeometryFromFile(std::string(EngineSetup::get()->MODELS_FOLDER + "projectile_weapon_01.fbx"));
            weapon->getModelProjectile()->setLabel("projectile_enemy_template" + ComponentsManager::get()->getComponentRender()->getUniqueGameObjectLabel());
            weapon->getModelProjectile()->setScale(1);
            weapon->getModelProjectile()->setFlatTextureColor(true);
            weapon->getModelProjectile()->setFlatColor(Color::red());
            weapon->setAmmoAmount(1000);
            weapon->setStartAmmoAmount(1000);
            weapon->setSpeed(100);
            weapon->setDamage(1);
            weapon->setDispersion(100);
            weapon->setAccuracy(100);
            weapon->setCadenceTime(100);
            weapon->setType(WeaponTypes::WEAPON_PROJECTILE);
            weapon->setStop(false);
            weapon->setAvailable(true);

            auto boss = new BossEnemy();
            boss->setEnabled(true);
            boss->setLabel("npc_" + ComponentsManager::get()->getComponentRender()->getUniqueGameObjectLabel());
            boss->setEnableLights(false);
            boss->setPosition(position);
            boss->setStencilBufferEnabled(true);
            boss->setScale(1);
            boss->setSpeed(10);
            boss->setStamina(10000);
            boss->setStartStamina(10000);
            boss->setEnableLights(true);
            boss->AssimpLoadGeometryFromFile(std::string(EngineSetup::get()->MODELS_FOLDER + "spaceships/boss_green_01.fbx"));
            boss->makeGhostBody(ComponentsManager::get()->getComponentCollisions()->getDynamicsWorld(), boss, EngineSetup::collisionGroups::Enemy, EngineSetup::collisionGroups::AllFilter);

            Point2D from(10, 10);
            from.x = (from.x * EngineSetup::get()->screenWidth) / 100;
            from.y = (from.y * EngineSetup::get()->screenHeight) / 100;

            Point2D to(80, 10);
            to.x = ( to.x * EngineSetup::get()->screenWidth) / 100;
            to.y = ( to.y * EngineSetup::get()->screenHeight) / 100;

            boss->setBehavior(new EnemyBehaviorPatrol(
                getWorldPositionFromScreenPoint(from),
                getWorldPositionFromScreenPoint(to),
                1
            ));
            boss->setWeaponType(weapon);
            boss->setSoundChannel(-1);

            auto projectileEmissor = new AmmoProjectileBodyEmissor(0.25, weapon);
            projectileEmissor->setPosition(boss->getPosition());
            projectileEmissor->setRotation(M3::getMatrixRotationForEulerAngles(90, 0, 0));
            projectileEmissor->setRotationFrameEnabled(true);
            projectileEmissor->setRotationFrame(Vertex3D(0, 1, 0));
            projectileEmissor->setStop(true);
            projectileEmissor->setStopDuration(1);
            projectileEmissor->setStopEvery(1);
            projectileEmissor->setLabel("boss_emissor_" + ComponentsManager::get()->getComponentRender()->getUniqueGameObjectLabel());
            Brakeza3D::get()->addObject3D(projectileEmissor, projectileEmissor->getLabel());

            boss->projectileEmissor = projectileEmissor;


            Brakeza3D::get()->addObject3D(boss, boss->getLabel());

            break;
        }
    }
}

bool LevelLoader::isHaveMusic() const {
    return hasMusic;
}

void LevelLoader::setHasMusic(bool hasMusic) {
    LevelLoader::hasMusic = hasMusic;
}
