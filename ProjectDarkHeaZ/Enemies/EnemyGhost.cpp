
#include "EnemyGhost.h"
#include "../../include/Brakeza3D.h"
#include "../Common/ShockWave.h"

EnemyGhost::EnemyGhost()
:
    LivingObject(this),
    RotatableToTarget(ComponentsManager::get()->getComponentGame()->getPlayer(), this, 1.0f),
    counterStuck(Counter(5)),
    particleEmitter (new ParticleEmitter(
        ParticleEmitterState::DEFAULT,
        nullptr,
        getPosition(),
        0,
        PaletteColors::getParticlesPlayerFrom(),
        PaletteColors::getParticlesPlayerTo(),
        OCParticlesContext::forPlayerEngine(),
        ComponentsManager::get()->getComponentGame()->getImages()->getTextureByLabel("particle01")
    )),
    projectileStartOffsetPosition(1.4f),
    particlesEngineOffset(Vertex3D(0, 0.25f, 0))
{
    counterDamageBlink.setEnabled(false);
    counterStuck.setEnabled(false);
    particleEmitter->setEnabled(true);
    attachObject(light);
    setStuck(false);
}

void EnemyGhost::onStart()
{
    blink = new FXBlink(true, this, 0.05f, PaletteColors::getEnemyBlink());
}

void EnemyGhost::onUpdate()
{
    Mesh3DAnimatedGhost::onUpdate();

    if (isRemoved()) return;

    if (!rotationFrameEnabled) {
        rotateToTarget();
    }

    if (getState() == EnemyState::ENEMY_STATE_DIE) {
        handleDie();
    }

    if (counterDamageBlink.isEnabled()) {
        counterDamageBlink.update();
        blink->update();
        if (counterDamageBlink.isFinished()) {
            blink->setEnabled(false);
            counterDamageBlink.setEnabled(false);
        }
    }

    auto componentGame = ComponentsManager::get()->getComponentGame();

    auto playerState = componentGame->getPlayer()->getState();

    if (getWeapon() != nullptr) {
        getWeapon()->onUpdate();
    }

    if (
        state != EnemyState::ENEMY_STATE_DIE &&
        playerState == PlayerState::LIVE &&
        componentGame->getGameState() == EngineSetup::GAMING
    ) {
        shoot(getTarget());
    }

    updateEmitters();

    updateLasers();

    updateLight();

    for ( auto dialog: dialogs) {
        const float staminaPercentage = (getStamina() * 100) / getStartStamina();
        if (staminaPercentage < dialog->staminaPercentage && !dialog->isShowed()) {
            dialog->setShowed(true);
            componentGame->setCurrentEnemyDialog(dialog);
        }
    }

    if (getBehavior() != nullptr) {
        particleEmitter->onUpdate();
        particleEmitter->setPosition(getPosition() + particlesEngineOffset - AxisUp().getScaled(-1));
        particleEmitter->setRotation(getRotation());
    }
}

void EnemyGhost::postUpdate()
{
    Mesh3D::postUpdate();

    if (!isEnabled()) {
        return;
    }

    counterLight.update();
    if (counterLight.isFinished()) {
        light->setEnabled(false);
        counterLight.setEnabled(false);
    }

    /*if (projectileEmitter != nullptr) {
        projectileEmitter->postUpdate();
    }*/

    if (isStuck()) {
        counterStuck.update();
        if (counterStuck.isFinished()) {
            unstuck();
        }
    }
}

void EnemyGhost::onDrawHostBuffer()
{
    Mesh3D::onDrawHostBuffer();

}

void EnemyGhost::handleDie()
{
    Brakeza3D::get()->addObject3D(new ShockWave(getPosition(), 0.50, 1, ShockWaveParams::standard(), true), Brakeza3D::uniqueObjectLabel("shockWave"));

    makeReward();

    Tools::makeFadeInSprite(getPosition(), ComponentsManager::get()->getComponentGame()->getRandomExplosionSprite()->getAnimation());
    ComponentsManager::get()->getComponentSound()->sound("enemyExplosion", EngineSetup::SoundChannels::SND_GLOBAL, 0);

    unstuck();
    remove();
}

void EnemyGhost::updateLasers()
{
    for (auto ray : fixedLasers) {
        ray->setRay(getRotation() * ray->getDirection());
        ray->setPosition(getPosition());
    }
}

void EnemyGhost::makeReward()
{
    if (!isRewards()) return;

    auto playerWeapons = ComponentsManager::get()->getComponentGame()->getPlayer()->getWeapons();

    int typePresent = Tools::random(0, 2);
    switch(typePresent) {
        case 0: {
            auto *healthItem = new ItemHealthGhost();
            healthItem->setLabel("itemHealth");
            healthItem->setEnableLights(true);
            healthItem->setPosition(getPosition());
            healthItem->setRotationFrameEnabled(true);
            healthItem->setRotationFrame(Tools::randomVertex().getScaled(0.5));
            healthItem->setStencilBufferEnabled(true);
            healthItem->setScale(1);
            healthItem->AssimpLoadGeometryFromFile(std::string(EngineSetup::get()->MODELS_FOLDER + "red_pill.fbx"));
            healthItem->makeGhostBody(
                ComponentsManager::get()->getComponentCollisions()->getDynamicsWorld(),
                healthItem,
                EngineSetup::Health,
                EngineSetup::Player
            );
            Brakeza3D::get()->addObject3D(healthItem, healthItem->getLabel());
            break;
        }
        case 1: {
            auto *healthItem = new ItemEnergyGhost();
            healthItem->setLabel("itemEnergy");
            healthItem->setEnableLights(true);
            healthItem->setPosition(getPosition());
            healthItem->setRotationFrameEnabled(true);
            healthItem->setRotationFrame(Tools::randomVertex().getScaled(0.5));
            healthItem->setStencilBufferEnabled(true);
            healthItem->AssimpLoadGeometryFromFile(std::string(EngineSetup::get()->MODELS_FOLDER + "pill.fbx"));
            healthItem->makeGhostBody(
                ComponentsManager::get()->getComponentCollisions()->getDynamicsWorld(),
                healthItem,
                EngineSetup::Health,
                EngineSetup::Player
            );
            Brakeza3D::get()->addObject3D(healthItem, healthItem->getLabel());
            break;
        }
        case 2: {
            int randomWeapon = Tools::random(0, 5);
            auto *weaponItem = new ItemWeaponGhost(playerWeapons[randomWeapon]);
            weaponItem->setLabel("itemWeapon");
            weaponItem->setEnableLights(false);
            weaponItem->setPosition(getPosition());
            weaponItem->setRotation(0, 0, 180);
            weaponItem->setRotationFrameEnabled(true);
            weaponItem->setRotationFrame(Tools::randomVertex().getScaled(0.5));
            weaponItem->setStencilBufferEnabled(true);
            weaponItem->clone(playerWeapons[randomWeapon]->getModel());
            weaponItem->makeSimpleGhostBody(
                    Vertex3D(0.5, 0.5, 0.5),
                    ComponentsManager::get()->getComponentCollisions()->getDynamicsWorld(),
                    EngineSetup::collisionGroups::Weapon,
                    EngineSetup::collisionGroups::Player
            );
            Brakeza3D::get()->addObject3D(weaponItem, weaponItem->getLabel());
            break;
        }
    }
}

void EnemyGhost::rotateToTarget()
{
    setRotationTarget(getTarget());
    makeRotationToTarget();
}

void EnemyGhost::integrate()
{
    Mesh3DAnimatedGhost::integrate();
}

void EnemyGhost::resolveCollision(Collisionable *withObject)
{
    Mesh3DAnimatedGhost::resolveCollision(withObject);

    auto *projectile = dynamic_cast<AmmoProjectile*> (withObject);
    if (projectile != nullptr) {
        if (projectile->getOwner() != this) {
            ComponentsManager::get()->getComponentSound()->sound("enemyDamage", EngineSetup::SoundChannels::SND_GLOBAL, 0);

            blink->setEnabled(true);
            counterDamageBlink.setEnabled(true);

            auto *body = dynamic_cast<AmmoProjectileBody*> (withObject);
            auto *ray = dynamic_cast<ProjectileRay*> (withObject);

            if (body != nullptr) {
                ComponentsManager::get()->getComponentGame()->getLevelLoader()->getStats()->increaseHit(WEAPON_PROJECTILE);
            }

            if (ray != nullptr) {
                ComponentsManager::get()->getComponentGame()->getLevelLoader()->getStats()->increaseHit(WEAPON_LASER);
            }

            this->takeDamage((float) projectile->getDamage());
        }
    }

    auto *satellite = dynamic_cast<PlayerSatellite*> (withObject);
    if (satellite != nullptr) {
        blink->setEnabled(true);
        counterDamageBlink.setEnabled(true);

        this->takeDamage(satellite->getDamage() * Brakeza3D::get()->getDeltaTime());
    }
    auto *player = dynamic_cast<Player*> (withObject);
    if (player != nullptr) {
        if (isExplode()) {
            takeDamage(getStamina());
            player->takeDamage(getExplodeDamage());
        }
    }
}

void EnemyGhost::remove()
{
    if (ComponentsManager::get()->getComponentRender()->getSelectedObject() == this) {
        ComponentsManager::get()->getComponentRender()->setSelectedObject(nullptr);
    }

    Mesh3DAnimatedGhost::remove();
}

void EnemyGhost::shoot(Object3D *target)
{
    if (getWeapon() == nullptr || !getWeapon()->isAvailable()) return;

    Vector3D direction(getPosition(), target->getPosition());

    bool wasShoot = weapon->shoot({
        direction.getComponent().getNormalize(),
        projectileStartOffsetPosition,
        EngineSetup::collisionGroups::ProjectileEnemy,
        EngineSetup::collisionGroups::Player | EngineSetup::collisionGroups::Enemy,
        false
    });

    if (wasShoot) {
        getLight()->setColor(PaletteColors::getPlayerProjectileLight());
        initLight();
    }
}

EnemyGhost::~EnemyGhost()
{
    delete blink;

    for (auto ray : fixedLasers) {
        ray->setRemoved(true);
    }
}

void EnemyGhost::stuck(float time)
{
    counterStuck.setStep(time);
    setStuck(true);
    Tools::makeLoopSprite(getPosition(), ComponentsManager::get()->getComponentGame()->getSpriteStuck()->getAnimation(), 5);

    counterStuck.setEnabled(true);
    if (getBehavior() != nullptr) {
        this->getBehavior()->setEnabled(false);
    }

    ComponentsManager::get()->getComponentSound()->sound("enemyExplosion", EngineSetup::SoundChannels::SND_GLOBAL, 0);
}

void EnemyGhost::unstuck()
{
    setStuck(false);
    counterStuck.setEnabled(false);

    if (getBehavior() != nullptr) {
        this->getBehavior()->setEnabled(true);
    }
}

Object3D *EnemyGhost::getTarget()
{
    auto game = ComponentsManager::get()->getComponentGame();
    auto target = game->getClosestReflection(getPosition());

    if (target != nullptr) {
        return target;
    }

    return game->getPlayer();
}

void EnemyGhost::addFixedLaser(ProjectileRay *ray)
{
    fixedLasers.push_back(ray);
    Brakeza3D::get()->addObject3D(ray, Brakeza3D::uniqueObjectLabel("fixedRay"));
}

void EnemyGhost::takeDamage(float damageTaken)
{
    this->stamina -= damageTaken;
    if (this->stamina <= 0) {
        die();
    }
}

void EnemyGhost::die()
{
    blink->setEnabled(false);
    ComponentsManager::get()->getComponentGame()->getPlayer()->increaseCoins(100);
    setState(ENEMY_STATE_DIE);
}

void EnemyGhost::initLight()
{
    counterLight.setEnabled(true);
    if (light != nullptr) {
        light->setEnabled(true);
    }
}

void EnemyGhost::updateLight()
{
    light->setPosition(getPosition() + lightPositionOffset + AxisUp().getScaled(-0.1));
}

std::vector<EnemyDialog *> &EnemyGhost::getDialogs() {
    return dialogs;
}

void EnemyGhost::drawImGuiProperties()
{
    Mesh3D::drawImGuiProperties();

    ImGui::Separator();

    if (ImGui::TreeNode("Enemy##")) {
        ImGui::Checkbox("Explode", &explode);

        if (ImGui::TreeNode("Particles Offset##")) {
            const float range_color_sensibility = 0.01f;
            const float range_col_min = -100.0f;
            const float range_col_max = 100.0f;

            ImGui::DragScalar("x", ImGuiDataType_Float, &particlesEngineOffset.x, range_color_sensibility,&range_col_min, &range_col_max, "%f", 1.0f);
            ImGui::DragScalar("y", ImGuiDataType_Float, &particlesEngineOffset.y, range_color_sensibility,&range_col_min, &range_col_max, "%f", 1.0f);
            ImGui::DragScalar("z", ImGuiDataType_Float, &particlesEngineOffset.z, range_color_sensibility,&range_col_min, &range_col_max, "%f", 1.0f);
            ImGui::TreePop();
        }

        if (weapon != nullptr) {
            if (ImGui::TreeNode("Weapon")) {
                getWeapon()->drawImGuiProperties();
                ImGui::TreePop();
            }
        }
    }
}

void EnemyGhost::updateEmitters()
{
    Enemy::updateEmitters();

    for (auto e: projectileEmitters) {
        e->setPosition(getPosition() + e->getOffsetPosition());
    }
}