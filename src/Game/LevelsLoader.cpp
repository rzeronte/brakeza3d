//
// Created by eduardo on 20/3/22.
//

#include "LevelsLoader.h"
#include "../../include/EngineSetup.h"
#include "../../include/Render/Logging.h"
#include "../../include/Brakeza3D.h"
#include "../../include/Render/Transforms.h"

#include <utility>


LevelsLoader::LevelsLoader(std::string filename)
{
    addLevel(filename);
    setLevelStartedToPlay(false);
    setCurrentLevelIndex(-1);
}

void LevelsLoader::load(int levelIndex)
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
                enemy->AssimpLoadGeometryFromFile(std::string(EngineSetup::get()->MODELS_FOLDER + "spaceship03.fbx"));
                enemy->makeGhostBody(ComponentsManager::get()->getComponentCollisions()->getDynamicsWorld(), enemy);

                auto weaponType = new WeaponType("weapon_" + enemy->getLabel());
                weaponType->getModelProjectile()->AssimpLoadGeometryFromFile(std::string(EngineSetup::get()->MODELS_FOLDER + "basic/icosphere.fbx"));
                weaponType->getModelProjectile()->setLabel("projectile_enemy_template");
                weaponType->getModelProjectile()->setScale(1);
                weaponType->setAmmoAmount(100);
                weaponType->setSpeed(500);
                weaponType->setDamage(10);
                weaponType->setDispersion(10);
                weaponType->setAvailable(true);
                weaponType->setAccuracy(100);
                weaponType->setCadenceTime(3);
                weaponType->setIconImage(EngineSetup::get()->HUD_FOLDER + "cold-fire.png");

                enemy->setWeaponType(weaponType);
                Brakeza3D::get()->addObject3D(enemy, enemy->getLabel());
                numberLevelEnemies++;
            }
        }
    }
}

void LevelsLoader::addLevel(std::string filename)
{
    this->images.push_back(new Image(std::move(filename)));
}

int LevelsLoader::getNumberLevelEnemies() const
{
    return numberLevelEnemies;
}

bool LevelsLoader::loadNext()
{

    if ((int) currentLevelIndex >= (int) images.size() - 1) {
        return false;
    }

    currentLevelIndex++;
    load(currentLevelIndex);

    return true;
}

bool LevelsLoader::isLevelStartedToPlay() const {
    return levelStartedToPlay;
}

void LevelsLoader::setLevelStartedToPlay(bool levelStartedToPlay) {
    LevelsLoader::levelStartedToPlay = levelStartedToPlay;
}

void LevelsLoader::setCurrentLevelIndex(int currentLevelIndex) {
    LevelsLoader::currentLevelIndex = currentLevelIndex;
}

int LevelsLoader::getNumberLevel() {
    return images.size();
}
