#include "ItemHumanGhost.h"
#include "../../include/ComponentsManager.h"

ItemHumanGhost::ItemHumanGhost()
:
    LivingObject(this)
{
    counterDamageBlink.setEnabled(true);
    setStamina(50);
}

void ItemHumanGhost::onStart()
{
    blink = new ShaderBlink(true, this, 0.05, Color::red());
}

void ItemHumanGhost::resolveCollision(Collisionable *withObject)
{
    Mesh3DGhost::resolveCollision(withObject);

    auto *projectile = dynamic_cast<AmmoProjectile*> (withObject);
    if (projectile != nullptr) {

        blink->setEnabled(true);
        counterDamageBlink.setEnabled(true);

        this->takeDamage((float) projectile->getDamage());
    }
}

void ItemHumanGhost::onUpdate()
{
    Mesh3D::onUpdate();

    if (!isEnabled() || isRemoved()) return;

    if (ComponentsManager::get()->getComponentGame()->getGameState() == EngineSetup::GAMING) {
        this->magnetizableTo(ComponentsManager::get()->getComponentGame()->getPlayer());
    }

    if (counterDamageBlink.isEnabled()) {
        counterDamageBlink.update();
        blink->update();
        if (counterDamageBlink.isFinished()) {
            blink->setEnabled(false);
            counterDamageBlink.setEnabled(false);
        }
    }
}

void ItemHumanGhost::takeDamage(float damage)
{
    this->stamina -= damage;
    if (this->stamina <= 0) {
        blink->setEnabled(false);
        ComponentsManager::get()->getComponentSound()->sound("humanDamage", EngineSetup::SoundChannels::SND_GLOBAL, 0);
        Tools::makeExplosion(this, getPosition(), 5, OCParticlesContext::forExplosion(), Color::red(), Color::white());
        removeCollisionObject();
        setRemoved(true);
        setRender(false);
    }
}