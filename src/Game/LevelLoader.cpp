//
// Created by eduardo on 20/3/22.
//

#include "LevelLoader.h"
#include "../../include/EngineSetup.h"
#include "../../include/Render/Logging.h"
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
    this->numberLevelEnemies = 0;

    ComponentsManager::get()->getComponentGame()->getPlayer()->setKillsCounter(0);

    auto *pixels = (Uint32 *) images[levelIndex]->getSurface()->pixels;

    ComponentsManager::get()->getComponentCamera()->getCamera()->frustum->updateFrustum();

    auto camera = ComponentsManager::get()->getComponentCamera()->getCamera();

    for (int y = 0; y < images[levelIndex]->getSurface()->h; y++) {
        for (int x = 0; x < images[levelIndex]->getSurface()->w; x++) {
            Color c(pixels[y * images[levelIndex]->getSurface()->w + x]);


            if (Color::red() == c) {
                Point2D  fixedPosition = Point2D(x, y);
                Vertex3D nearPlaneVertex = Transforms::Point2DToWorld(fixedPosition, camera);
                Vector3D ray(camera->getPosition(),nearPlaneVertex);

                auto *enemy = new EnemyGhost();
                enemy->setEnabled(true);
                enemy->setLabel("npc_" + ComponentsManager::get()->getComponentRender()->getUniqueGameObjectLabel());
                enemy->setEnableLights(false);
                enemy->setPosition(ray.getComponent().getScaled(ComponentsManager::get()->getComponentGame()->Z_COORDINATE_GAMEPLAY));
                enemy->setStencilBufferEnabled(true);
                enemy->setScale(1);
                enemy->setSpeed(100);
                enemy->setStamina(500);
                enemy->setStartStamina(500);
                enemy->AssimpLoadGeometryFromFile(std::string(EngineSetup::get()->MODELS_FOLDER + "spaceship03.fbx"));
                enemy->makeGhostBody(ComponentsManager::get()->getComponentCollisions()->getDynamicsWorld(), enemy);

                //enemy->setMotion(new EnemyBehaviorCircle(enemy->getPosition(), 0.7, 2000));
                //enemy->setMotion(new EnemyBehaviorPatrol(Vertex3D(-2500, -2800, 10000), Vertex3D(2500, -2800, 10000), 1000));
                enemy->setMotion(new EnemyBehaviorFollow(ComponentsManager::get()->getComponentGame()->getPlayer(), 5, 5000));

                auto weaponType = new Weapon("weapon_" + enemy->getLabel());
                weaponType->getModelProjectile()->AssimpLoadGeometryFromFile(std::string(EngineSetup::get()->MODELS_FOLDER + "basic/icosphere.fbx"));
                weaponType->getModelProjectile()->setLabel("projectile_enemy_template");
                weaponType->getModelProjectile()->setScale(1);
                weaponType->setAmmoAmount(0);
                weaponType->setSpeed(500);
                weaponType->setDamage(10);
                weaponType->setDispersion(10);
                weaponType->setAvailable(true);
                weaponType->setAccuracy(100);
                weaponType->setCadenceTime(1);
                weaponType->setIconImage(EngineSetup::get()->HUD_FOLDER + "cold-fire.png");

                enemy->setWeaponType(weaponType);

                auto respawner = new EnemyGhostRespawner(enemy, (int) Tools::random(1, 4));
                Brakeza3D::get()->addObject3D(respawner, "respawner_" + ComponentsManager::get()->getComponentRender()->getUniqueGameObjectLabel());
                numberLevelEnemies++;
            }
        }
    }
}

void LevelLoader::addLevel(std::string filename)
{
    this->images.push_back(new Image(std::move(filename)));
}

int LevelLoader::getNumberLevelEnemies() const
{
    return numberLevelEnemies;
}

bool LevelLoader::loadNext()
{

    if ((int) currentLevelIndex >= (int) images.size() - 1) {
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
    return (int) images.size();
}

int LevelLoader::getCurrentLevelIndex() const {
    return currentLevelIndex;
}
