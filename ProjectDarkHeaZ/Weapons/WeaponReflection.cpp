//
// Created by edu on 19/01/24.
//

#include "WeaponReflection.h"
#include "../Player.h"
#include "../../include/Brakeza3D.h"

WeaponReflection::WeaponReflection(const WeaponAttributes &attributes) : Weapon(attributes)
{

}

void WeaponReflection::shootHologram(Object3D *parent, Vertex3D position)
{
    if (getAmmoAmount() <= 0) return;

    if (isStop() && counterStopDuration.isEnabled()) {
        return;
    }

    if (counterCadence->isFinished()) {
        counterCadence->setEnabled(true);
        setStatus(WeaponStatus::PRESSED);

        Logging::Log("Weapon shootHologram from %s", parent->getLabel().c_str());

        auto *reflection = new PlayerReflection(5);
        reflection->setStencilBufferEnabled(true);
        reflection->setParent(parent);
        reflection->setLabel(Brakeza3D::uniqueObjectLabel("playerReflection"));
        reflection->clone(getModelProjectile());
        reflection->setPosition(position);
        reflection->setEnableLights(false);
        reflection->setEnabled(true);
        reflection->setRotationFrame(Tools::randomVertex().getScaled(0.25f));
        reflection->setRotationFrameEnabled(true);
        reflection->setFlatTextureColor(false);

        setAmmoAmount(ammoAmount - 1);

        ComponentsManager::get()->getComponentSound()->sound("gravitationalShield", EngineSetup::SoundChannels::SND_GLOBAL, 0);

        Brakeza3D::get()->addObject3D(reflection, reflection->getLabel());
        Tools::makeFadeInSprite(position, ComponentsManager::get()->getComponentGame()->getFadeInSpriteGreen()->getAnimation());
    }
}

bool WeaponReflection::shoot(WeaponShootAttributes attributes)
{
    shootHologram(parent, parent->getPosition());
    return true;
}
