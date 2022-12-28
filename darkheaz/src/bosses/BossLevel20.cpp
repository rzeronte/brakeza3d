//
// Created by eduardo on 17/04/22.
//

#include "BossLevel20.h"
#include "../../../include/ComponentsManager.h"


BossLevel20::BossLevel20(Vertex3D position)
{
    setEnabled(true);
    setLabel("boss_" + ComponentsManager::get()->getComponentRender()->getUniqueGameObjectLabel());
    setEnableLights(false);
    setPosition(position);
    setStencilBufferEnabled(true);
    loadBlinkShader();
    setScale(1);
    setStamina(10000);
    setStartStamina(10000);
    AssimpLoadAnimation(std::string(EngineSetup::get()->MODELS_FOLDER + "tentacle.fbx"));
    makeSimpleGhostBody(
            Vertex3D(1500, 1500, 1500),
            ComponentsManager::get()->getComponentCollisions()->getDynamicsWorld(),
            EngineSetup::collisionGroups::Enemy,
            EngineSetup::collisionGroups::AllFilter
    );

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
    weapon->setType(WeaponTypes::WEAPON_SMART_PROJECTILE);
    weapon->setStop(false);
    weapon->setAvailable(true);

    setWeapon(weapon);

    setSoundChannel(-1);
}

AmmoProjectileBodyEmissor *BossLevel20::getProjectileEmissorLeft() const {
    return projectileEmissorLeft;
}

void BossLevel20::setProjectileEmissorLeft(AmmoProjectileBodyEmissor *projectileEmissorLeft) {
    BossLevel20::projectileEmissorLeft = projectileEmissorLeft;
}

AmmoProjectileBodyEmissor *BossLevel20::getProjectileEmissorRight() const {
    return projectileEmissorRight;
}

void BossLevel20::setProjectileEmissorRight(AmmoProjectileBodyEmissor *projectileEmissorRight) {
    BossLevel20::projectileEmissorRight = projectileEmissorRight;
}