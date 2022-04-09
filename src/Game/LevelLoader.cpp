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
    setCurrentLevelIndex(levelIndex);
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

    setMusic(cJSON_GetObjectItemCaseSensitive(myDataJSON, "music")->valuestring);

    std::string levelName = cJSON_GetObjectItemCaseSensitive(myDataJSON, "name")->valuestring;
    this->setLevelName(levelName);

    std::string backgroundImage = cJSON_GetObjectItemCaseSensitive(myDataJSON, "backgroundImage")->valuestring;
    auto shaderBackground = dynamic_cast<ShaderImageBackground*> (ComponentsManager::get()->getComponentRender()->getShaderByType(EngineSetup::ShaderTypes::BACKGROUND));
    shaderBackground->setImage(new Image(EngineSetup::get()->IMAGES_FOLDER + backgroundImage));

    if (cJSON_GetObjectItemCaseSensitive(myDataJSON, "tutorialImage") != nullptr) {
        std::string tutorialImagePath = cJSON_GetObjectItemCaseSensitive(myDataJSON, "tutorialImage")->valuestring;
        this->tutorialImage->loadTGA(EngineSetup::get()->IMAGES_FOLDER + tutorialImagePath);
        this->hasTutorial = true;
    }

    bool allowGravitationalShield = cJSON_GetObjectItemCaseSensitive(myDataJSON, "allowGravitionalShield")->valueint;
    bool allowEnergyShield = cJSON_GetObjectItemCaseSensitive(myDataJSON, "allowEnergyShield")->valueint;

    ComponentsManager::get()->getComponentGame()->getPlayer()->setAllowGravitationalShields(allowGravitationalShield);
    ComponentsManager::get()->getComponentGame()->getPlayer()->setAllowEnergyShield(allowEnergyShield);

    cJSON *weaponsList = cJSON_GetObjectItemCaseSensitive(myDataJSON, "enemies");

    ComponentsManager::get()->getComponentCamera()->getCamera()->frustum->updateFrustum();
    auto camera = ComponentsManager::get()->getComponentCamera()->getCamera();

    cJSON *currentEnemy;
    cJSON_ArrayForEach(currentEnemy, weaponsList) {
        std::string name = cJSON_GetObjectItemCaseSensitive(currentEnemy, "name")->valuestring;
        std::string model = cJSON_GetObjectItemCaseSensitive(currentEnemy, "model")->valuestring;
        int stamina = cJSON_GetObjectItemCaseSensitive(currentEnemy, "stamina")->valueint;
        int speed = cJSON_GetObjectItemCaseSensitive(currentEnemy, "speed")->valueint;
        cJSON *motion = cJSON_GetObjectItemCaseSensitive(currentEnemy, "motion");
        cJSON *position = cJSON_GetObjectItemCaseSensitive(currentEnemy, "position");
        cJSON *weapon = cJSON_GetObjectItemCaseSensitive(currentEnemy, "weapon");

        int percentX = cJSON_GetObjectItemCaseSensitive(position, "x")->valueint;
        int percentY = cJSON_GetObjectItemCaseSensitive(position, "y")->valueint;

        int x = (percentX * EngineSetup::get()->screenWidth) / 100;
        int y = (percentY * EngineSetup::get()->screenHeight) / 100;

        Point2D  fixedPosition = Point2D(x, y);
        Vertex3D nearPlaneVertex = Transforms::Point2DToWorld(fixedPosition, camera);
        Vector3D ray(camera->getPosition(),nearPlaneVertex);

        EnemyBehavior *behavior;
        const int typeMotion = cJSON_GetObjectItemCaseSensitive(motion, "type")->valueint;
        switch(typeMotion) {
            case EnemyBehaviorTypes::BEHAVIOR_PATROL: {
                cJSON *fromJSON = cJSON_GetObjectItemCaseSensitive(motion, "from");
                Point2D fixedFromPosition = Point2D(
                    cJSON_GetObjectItemCaseSensitive(fromJSON, "x")->valueint,
                    cJSON_GetObjectItemCaseSensitive(fromJSON,"y")->valueint
                );
                Vertex3D nearPlaneFromVertex = Transforms::Point2DToWorld(fixedFromPosition, camera);
                Vertex3D from = Vector3D(camera->getPosition(), nearPlaneFromVertex).getComponent().getScaled(ComponentsManager::get()->getComponentGame()->Z_COORDINATE_GAMEPLAY);

                cJSON *toJSON = cJSON_GetObjectItemCaseSensitive(motion, "to");
                Point2D fixedToPosition = Point2D(
                        (cJSON_GetObjectItemCaseSensitive(toJSON, "x")->valueint * EngineSetup::get()->screenWidth) / 100,
                        (cJSON_GetObjectItemCaseSensitive(toJSON,"y")->valueint * EngineSetup::get()->screenHeight) / 100
                );
                Vertex3D nearPlaneToVertex = Transforms::Point2DToWorld(fixedToPosition, camera);
                Vertex3D to = Vector3D(camera->getPosition(), nearPlaneToVertex).getComponent().getScaled(ComponentsManager::get()->getComponentGame()->Z_COORDINATE_GAMEPLAY);

                float behaviorSpeed = (float) cJSON_GetObjectItemCaseSensitive(motion, "speed")->valuedouble;
                behavior = new EnemyBehaviorPatrol(from, to, behaviorSpeed);
                break;
            }
            case EnemyBehaviorTypes::BEHAVIOR_FOLLOW: {
                auto player = ComponentsManager::get()->getComponentGame()->getPlayer();
                int behaviorSpeed = cJSON_GetObjectItemCaseSensitive(motion, "speed")->valueint;
                int separation = cJSON_GetObjectItemCaseSensitive(motion, "separation")->valueint;
                behavior = new EnemyBehaviorFollow(player, behaviorSpeed, separation);
                break;
            }
            case EnemyBehaviorTypes::BEHAVIOR_CIRCLE: {
                cJSON *centerJSON = cJSON_GetObjectItemCaseSensitive(motion, "center");

                Point2D fixedCenterPosition = Point2D(
                    cJSON_GetObjectItemCaseSensitive(centerJSON, "x")->valueint,
                    cJSON_GetObjectItemCaseSensitive(centerJSON,"y")->valueint
                );
                Vertex3D nearPlaneToVertex = Transforms::Point2DToWorld(fixedCenterPosition, camera);
                Vertex3D center = Vector3D(camera->getPosition(), nearPlaneToVertex).getComponent().getScaled(ComponentsManager::get()->getComponentGame()->Z_COORDINATE_GAMEPLAY);

                float speed = cJSON_GetObjectItemCaseSensitive(motion, "speed")->valuedouble;
                float radius = cJSON_GetObjectItemCaseSensitive(motion, "radius")->valuedouble;
                behavior = new EnemyBehaviorCircle(center, speed, radius);
                break;
            }
        }

        auto *enemy = new EnemyGhost();
        enemy->setEnabled(true);
        enemy->setLabel("npc_" + ComponentsManager::get()->getComponentRender()->getUniqueGameObjectLabel());
        enemy->setEnableLights(false);
        enemy->setPosition(ray.getComponent().getScaled(ComponentsManager::get()->getComponentGame()->Z_COORDINATE_GAMEPLAY));
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

        auto weaponType = parseWeaponJSON(weapon);
        weaponType->setSoundChannel(enemy->getSoundChannel());

        Logging::getInstance()->Log("Loading enemy soundChannel: " + std::to_string(enemy->getSoundChannel()));

        enemy->setWeaponType(weaponType);

        auto respawner = new EnemyGhostRespawner(enemy, (int) Tools::random(1, 3));
        Brakeza3D::get()->addObject3D(respawner, "respawner_" + ComponentsManager::get()->getComponentRender()->getUniqueGameObjectLabel());
        respawners.push_back(respawner);
    }

    cJSON *items = cJSON_GetObjectItemCaseSensitive(myDataJSON, "items");
    cJSON *currentItem;
    cJSON_ArrayForEach(currentItem, items) {
        const int typeItem = cJSON_GetObjectItemCaseSensitive(currentItem, "type")->valueint;

        cJSON *position = cJSON_GetObjectItemCaseSensitive(currentItem, "position");
        int percentX = cJSON_GetObjectItemCaseSensitive(position, "x")->valueint;
        int percentY = cJSON_GetObjectItemCaseSensitive(position, "y")->valueint;
        int x = (percentX * EngineSetup::get()->screenWidth) / 100;
        int y = (percentY * EngineSetup::get()->screenHeight) / 100;

        Point2D  fixedPosition = Point2D(x, y);
        Vertex3D nearPlaneVertex = Transforms::Point2DToWorld(fixedPosition, camera);
        Vector3D ray(camera->getPosition(),nearPlaneVertex);

        Vertex3D p = ray.getComponent().getScaled(ComponentsManager::get()->getComponentGame()->Z_COORDINATE_GAMEPLAY);

        switch(typeItem) {
            case LevelInfoItemsTypes::ITEM_ENERGY: {
                this->makeItemEnergyGhost(p);
                break;
            }
            case LevelInfoItemsTypes::ITEM_HEALTH: {
                this->makeItemHealthGhost(p);
                break;
            }
            case LevelInfoItemsTypes::ITEM_WEAPON_INSTANT: {
                this->makeItemWeapon(WeaponTypes::WEAPON_INSTANT, p);
                break;
            }
            case LevelInfoItemsTypes::ITEM_WEAPON_PROJECTILE: {
                this->makeItemWeapon(WeaponTypes::WEAPON_PROJECTILE, p);
                break;
            }
            case LevelInfoItemsTypes::ITEM_WEAPON_SMART: {
                this->makeItemWeapon(WeaponTypes::WEAPON_SMART_PROJECTILE, p);
                break;
            }
        }
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
