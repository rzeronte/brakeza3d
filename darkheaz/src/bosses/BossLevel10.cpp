//
// Created by eduardo on 17/04/22.
//

#include "BossLevel10.h"
#include "../../../include/ComponentsManager.h"
#include "../../../include/Brakeza3D.h"

BossLevel10::BossLevel10(Vertex3D position)
{
    this->projectileEmissor = nullptr;
    setEnabled(true);
    setLabel("boss_" + ComponentsManager::get()->getComponentRender()->getUniqueGameObjectLabel());
    setEnableLights(false);
    setPosition(position);
    setStencilBufferEnabled(true);
    loadBlinkShader();
    setScale(1);
    setSpeed(10);
    setStamina(1000);
    setStartStamina(10000);
    AssimpLoadGeometryFromFile(std::string(EngineSetup::get()->MODELS_FOLDER + "spaceships/boss_green_01.fbx"));
    makeGhostBody(ComponentsManager::get()->getComponentCollisions()->getDynamicsWorld(), this, EngineSetup::collisionGroups::Enemy, EngineSetup::collisionGroups::AllFilter);

    auto weapon = new Weapon("boss_weapon");
    weapon->getModel()->AssimpLoadGeometryFromFile(std::string(EngineSetup::get()->MODELS_FOLDER + "projectile_weapon_01.fbx"));
    weapon->getModelProjectile()->setFlatTextureColor(true);
    weapon->getModelProjectile()->setFlatColor(Color::fuchsia());
    weapon->getModelProjectile()->setEnableLights(false);
    weapon->getModelProjectile()->AssimpLoadGeometryFromFile(std::string(EngineSetup::get()->MODELS_FOLDER + "projectile_weapon_01.fbx"));
    weapon->getModelProjectile()->setLabel("projectile_enemy_template" + ComponentsManager::get()->getComponentRender()->getUniqueGameObjectLabel());
    weapon->getModelProjectile()->setScale(1);

    weapon->setAmmoAmount(5000);
    weapon->setStartAmmoAmount(5000);
    weapon->setSpeed(600);
    weapon->setDamage(10);
    weapon->setDispersion(100);
    weapon->setAccuracy(100);
    weapon->setCadenceTime(0.8);
    weapon->setType(WeaponTypes::WEAPON_PROJECTILE);
    weapon->setStop(false);
    weapon->setAvailable(true);

    setWeapon(weapon);

    setSoundChannel(-1);


    auto weaponForEmissor = new Weapon("boss_emissor_weapon");
    weaponForEmissor->getModel()->AssimpLoadGeometryFromFile(std::string(EngineSetup::get()->MODELS_FOLDER + "projectile_weapon_01.fbx"));
    weaponForEmissor->getModelProjectile()->setFlatTextureColor(true);
    weaponForEmissor->getModelProjectile()->setFlatColor(Color::fuchsia());
    weaponForEmissor->getModelProjectile()->setEnableLights(false);
    weaponForEmissor->getModelProjectile()->AssimpLoadGeometryFromFile(std::string(EngineSetup::get()->MODELS_FOLDER + "projectile_weapon_01.fbx"));
    weaponForEmissor->getModelProjectile()->setLabel("projectile_enemy_template" + ComponentsManager::get()->getComponentRender()->getUniqueGameObjectLabel());
    weaponForEmissor->getModelProjectile()->setScale(1);
    weaponForEmissor->setAmmoAmount(5000);
    weaponForEmissor->setStartAmmoAmount(5000);
    weaponForEmissor->setSpeed(500);
    weaponForEmissor->setDamage(10);
    weaponForEmissor->setDispersion(100);
    weaponForEmissor->setAccuracy(100);
    weaponForEmissor->setCadenceTime(0.8);
    weaponForEmissor->setType(WeaponTypes::WEAPON_PROJECTILE);
    weaponForEmissor->setStop(false);
    weaponForEmissor->setAvailable(true);

    projectileEmissor = new AmmoProjectileBodyEmissor(0.25, weaponForEmissor);
    projectileEmissor->setActive(true);
    projectileEmissor->setPosition(getPosition());
    projectileEmissor->setRotation(M3::getMatrixRotationForEulerAngles(90, 0, 0));
    projectileEmissor->setRotationFrameEnabled(true);
    projectileEmissor->setRotationFrame(Vertex3D(0, 1, 0));
    projectileEmissor->setStop(true);
    projectileEmissor->setStopDuration(1);
    projectileEmissor->setStopEvery(1);
    projectileEmissor->setLabel("boss_emissor_" + ComponentsManager::get()->getComponentRender()->getUniqueGameObjectLabel());

    Brakeza3D::get()->addObject3D(projectileEmissor, projectileEmissor->getLabel());
}

AmmoProjectileBodyEmissor *BossLevel10::getProjectileEmissor() const {
    return projectileEmissor;
}

void BossLevel10::setProjectileEmissor(AmmoProjectileBodyEmissor *emmissor) {
    BossLevel10::projectileEmissor = emmissor;
}
