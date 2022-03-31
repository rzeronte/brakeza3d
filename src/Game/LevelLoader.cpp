//
// Created by eduardo on 20/3/22.
//

#include "LevelLoader.h"
#include "../../include/Brakeza3D.h"
#include "../../include/Render/Transforms.h"
#include "../../include/Game/EnemyGhostRespawner.h"
#include "../../include/Game/EnemyBehaviorCircle.h"
#include "../../include/Game/EnemyBehaviorPatrol.h"
#include "../../include/Game/EnemyBehaviorFollow.h"

#include <utility>

LevelLoader::LevelLoader(std::string filename)
{
    addLevel(filename);
    setLevelStartedToPlay(false);
    setCurrentLevelIndex(-1);
}

void LevelLoader::load(int levelIndex)
{
    Logging::getInstance()->Log("loading level: " + std::to_string(levelIndex));

    setLevelStartedToPlay(true);

    this->currentLevelIndex = levelIndex;

    ComponentsManager::get()->getComponentGame()->getPlayer()->setKillsCounter(0);

    loadEnemiesFromJSON(levels[levelIndex]);
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

void LevelLoader::loadEnemiesFromJSON(std::string filePath)
{
    Logging::Log("Loading Enemies for level...", "LEVEL_LOADER");

    std::string sndPath = EngineSetup::get()->SOUNDS_FOLDER;

    respawners.resize(0);

    size_t file_size;
    const char *weaponsFile = Tools::readFile(filePath, file_size);
    cJSON *myDataJSON = cJSON_Parse(weaponsFile);

    if (myDataJSON == nullptr) {
        Logging::Log(filePath + " can't be loaded", "ERROR");
        return;
    }

    setMusic(cJSON_GetObjectItemCaseSensitive(myDataJSON, "music")->valuestring);

    std::string levelName = cJSON_GetObjectItemCaseSensitive(myDataJSON, "name")->valuestring;
    std::string backgroundImage = cJSON_GetObjectItemCaseSensitive(myDataJSON, "backgroundImage")->valuestring;


    auto shaderBackground = dynamic_cast<ShaderImageBackground*> (ComponentsManager::get()->getComponentRender()->getShaderByType(EngineSetup::ShaderTypes::BACKGROUND));
    shaderBackground->setImage(new Image(EngineSetup::get()->IMAGES_FOLDER + backgroundImage));

    cJSON *weaponsList = cJSON_GetObjectItemCaseSensitive(myDataJSON, "enemies");

    ComponentsManager::get()->getComponentCamera()->getCamera()->frustum->updateFrustum();
    auto camera = ComponentsManager::get()->getComponentCamera()->getCamera();

    cJSON *currentEnemy;
    cJSON_ArrayForEach(currentEnemy, weaponsList) {
        std::string name = cJSON_GetObjectItemCaseSensitive(currentEnemy, "name")->valuestring;
        std::string model = cJSON_GetObjectItemCaseSensitive(currentEnemy, "model")->valuestring;
        std::string fireSound = cJSON_GetObjectItemCaseSensitive(currentEnemy, "fireSound")->valuestring;
        int stamina = cJSON_GetObjectItemCaseSensitive(currentEnemy, "stamina")->valueint;
        int speed = cJSON_GetObjectItemCaseSensitive(currentEnemy, "speed")->valueint;
        cJSON *motion = cJSON_GetObjectItemCaseSensitive(currentEnemy, "motion");
        cJSON *position = cJSON_GetObjectItemCaseSensitive(currentEnemy, "position");
        cJSON *weapon = cJSON_GetObjectItemCaseSensitive(currentEnemy, "weapon");

        int x = cJSON_GetObjectItemCaseSensitive(position, "x")->valueint;
        int y = cJSON_GetObjectItemCaseSensitive(position, "y")->valueint;

        Point2D  fixedPosition = Point2D(x, y);
        Vertex3D nearPlaneVertex = Transforms::Point2DToWorld(fixedPosition, camera);
        Vector3D ray(camera->getPosition(),nearPlaneVertex);

        EnemyBehavior *behavior;
        switch(cJSON_GetObjectItemCaseSensitive(motion, "type")->valueint) {
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
                    cJSON_GetObjectItemCaseSensitive(toJSON, "x")->valueint,
                    cJSON_GetObjectItemCaseSensitive(toJSON,"y")->valueint
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
        enemy->AssimpLoadGeometryFromFile(std::string(EngineSetup::get()->MODELS_FOLDER + model));
        enemy->makeGhostBody(ComponentsManager::get()->getComponentCollisions()->getDynamicsWorld(), enemy, EngineSetup::collisionGroups::Enemy, EngineSetup::collisionGroups::AllFilter);
        enemy->setSoundChannel(respawners.size() + 2);
        enemy->setBehavior(behavior);

        auto weaponType = parseWeaponJSON(weapon);
        weaponType->setSoundChannel(enemy->getSoundChannel());

        Logging::getInstance()->Log("Loading enemy soundChannel: " + std::to_string(enemy->getSoundChannel()));

        enemy->setWeaponType(weaponType);

        auto respawner = new EnemyGhostRespawner(enemy, (int) Tools::random(1, 3));
        Brakeza3D::get()->addObject3D(respawner, "respawner_" + ComponentsManager::get()->getComponentRender()->getUniqueGameObjectLabel());
        respawners.push_back(respawner);

        //Logging::getInstance()->Log(cJSON_GetObjectItemCaseSensitive(position, "y")->valuestring);
        //SoundPackageItemType selectedType = SoundPackageItemType::SOUND;
        //Logging::Log("Loading file sound " + std::string(fireSound->valuestring), "SOUNDS");
        //BUFFERS->soundPackage->addItem(sndPath + fireSound->valuestring, fireSound->valuestring, selectedType);
    }
}

Weapon *LevelLoader::parseWeaponJSON(cJSON *weaponJson)
{
    int index = cJSON_GetObjectItemCaseSensitive(weaponJson, "index")->valueint;
    std::string icon = cJSON_GetObjectItemCaseSensitive(weaponJson, "icon")->valuestring;
    std::string name = cJSON_GetObjectItemCaseSensitive(weaponJson, "name")->valuestring;
    std::string modelProjectile = cJSON_GetObjectItemCaseSensitive(weaponJson, "modelProjectile")->valuestring;
    std::string model = cJSON_GetObjectItemCaseSensitive(weaponJson, "model")->valuestring;
    int amount = cJSON_GetObjectItemCaseSensitive(weaponJson, "amount")->valueint;
    float speed = cJSON_GetObjectItemCaseSensitive(weaponJson, "speed")->valuedouble;
    float dispersion = cJSON_GetObjectItemCaseSensitive(weaponJson, "dispersion")->valuedouble;
    float accuracy = cJSON_GetObjectItemCaseSensitive(weaponJson, "accuracy")->valuedouble;
    float damage = cJSON_GetObjectItemCaseSensitive(weaponJson, "damage")->valuedouble;
    int type = cJSON_GetObjectItemCaseSensitive(weaponJson, "type")->valueint;
    float cadenceTime = (float) cJSON_GetObjectItemCaseSensitive(weaponJson, "cadenceTime")->valuedouble;
    bool stop = (bool) cJSON_GetObjectItemCaseSensitive(weaponJson, "stop")->valueint;
    float stopDuration = (float) cJSON_GetObjectItemCaseSensitive(weaponJson, "stopDuration")->valuedouble;
    float stopEvery = (float) cJSON_GetObjectItemCaseSensitive(weaponJson, "stopEvery")->valuedouble;

    auto weapon = new Weapon(name);
    weapon->getModel()->AssimpLoadGeometryFromFile(std::string(EngineSetup::get()->MODELS_FOLDER + model));
    weapon->getModelProjectile()->setFlatTextureColor(true);
    weapon->getModelProjectile()->setFlatColor(Color::fuchsia());
    weapon->getModelProjectile()->AssimpLoadGeometryFromFile(std::string(EngineSetup::get()->MODELS_FOLDER + modelProjectile));
    weapon->getModelProjectile()->setLabel("projectile_enemy_template" + std::to_string(index));
    weapon->getModelProjectile()->setScale(1);
    weapon->setAmmoAmount(amount);
    weapon->setStartAmmoAmount(amount);
    weapon->setSpeed(speed);
    weapon->setDamage(damage);
    weapon->setDispersion(dispersion);
    weapon->setAvailable(true);
    weapon->setAccuracy(accuracy);
    weapon->setCadenceTime(cadenceTime);
    weapon->setType(type);
    weapon->setIconImage(EngineSetup::get()->HUD_FOLDER + icon);
    weapon->setStop(stop);

    if (stop) {
        Logging::getInstance()->Log("Arma con parada de " +std::to_string(stopDuration)+" cada "  + std::to_string(stopEvery));

        weapon->setStopDuration(stopDuration);
        weapon->setStopEvery(stopEvery);
    }

    return weapon;
}

void LevelLoader::startRespawners()
{
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
