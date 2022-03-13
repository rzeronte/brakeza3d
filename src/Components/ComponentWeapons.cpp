
#include <SDL2/SDL_image.h>
#include "../../include/Components/ComponentWeapons.h"
#include "../../include/Render/Maths.h"
#include "../../include/ComponentsManager.h"

ComponentWeapons::ComponentWeapons() {
    this->currentWeaponIndex = WeaponsTypes::EMPTY;
}

void ComponentWeapons::onStart() {
    Logging::Log("ComponentWeapons onStart", "ComponentWeapons");
}

void ComponentWeapons::preUpdate() {

}

void ComponentWeapons::onUpdate() {
    if (this->currentWeaponIndex != WeaponsTypes::EMPTY) {
        this->getCurrentWeaponType()->onUpdate();
    }
}

void ComponentWeapons::postUpdate() {

}

void ComponentWeapons::onEnd() {

}

void ComponentWeapons::onSDLPollEvent(SDL_Event *e, bool &finish) {
}

void ComponentWeapons::addWeaponType(const std::string& label) {
    this->weaponTypes.emplace_back(new WeaponType(label));
}

WeaponType *ComponentWeapons::getWeaponTypeByLabel(const std::string& label) {
    for (auto & weaponType : this->weaponTypes) {
        if (weaponType->label == label) {
            return weaponType;
        }
    }

    return nullptr;
}

WeaponType *ComponentWeapons::getCurrentWeaponType() {
    return this->weaponTypes[currentWeaponIndex];
}

void ComponentWeapons::headBob(Vector3D velocity) {
    Vertex3D v = velocity.getComponent();

    // head bob
    if (abs(v.x) > 0.5 || abs(v.z) > 0.5) {
        this->offsetY = sin(Maths::degreesToRadians(this->headBobOffsetY));
        this->offsetY = abs(this->offsetY) * 7;

        this->headBobOffsetY += 10;
        if (this->headBobOffsetY > 360) {
            this->headBobOffsetY = 0;
        }
    } else {
        this->headBobOffsetY = 0;
    }
}

bool ComponentWeapons::isEmptyWeapon() const {
    if (this->currentWeaponIndex >= 0) {
        return false;
    }

    return true;
}

void ComponentWeapons::setCurrentWeaponIndex(int newCurrentWeaponIndex) {
    ComponentWeapons::currentWeaponIndex = newCurrentWeaponIndex;
}

void ComponentWeapons::playerShoot() {
    if (isEmptyWeapon()) {
        return;
    }
    auto game = ComponentsManager::get()->getComponentGame();
    Logging::Log("ComponentWeapons playerShoot!", "ComponentWeapons");
    if (getCurrentWeaponType()->getAmmoType()->getAmount() > 0) {
        this->getCurrentWeaponType()->shoot(
            game->getPlayer(),
            game->getPlayer()->getPosition() - game->getPlayer()->AxisUp().getScaled(1000),
            ComponentsManager::get()->getComponentGame()->getPlayer()->AxisUp().getInverse()
        );
    } else {
        Logging::Log("Empty Ammo!", "ComponentWeapons");
        std::string soundLabel = getCurrentWeaponType()->getSoundEmptyLabel();
        Tools::playMixedSound(
                EngineBuffers::getInstance()->soundPackage->getSoundByLabel(soundLabel),
                EngineSetup::SoundChannels::SND_WEAPON,
                0
        );
    }
}

void ComponentWeapons::reload() {
    Logging::Log("ComponentWeapons reload!", "Weapons");

    if (getCurrentWeaponType()->getAmmoType()->getReloads() > 0) {
        this->getCurrentWeaponType()->reload();
    }
}

