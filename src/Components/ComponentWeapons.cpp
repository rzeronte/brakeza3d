
#include <SDL_image.h>
#include "../../headers/Components/ComponentWeapons.h"
#include "../../headers/Render/Maths.h"
#include "../../headers/ComponentsManager.h"
#include "../../headers/Brakeza3D.h"

ComponentWeapons::ComponentWeapons() {
    this->currentWeaponIndex = SETUP->WeaponsTypes::EMPTY;
}

void ComponentWeapons::onStart() {
}

void ComponentWeapons::preUpdate() {

}

void ComponentWeapons::onUpdate() {
    if (this->currentWeaponIndex != EngineSetup::WeaponsTypes::EMPTY) {
        this->getCurrentWeaponType()->onUpdate();
    }
}

void ComponentWeapons::postUpdate() {

}

void ComponentWeapons::onEnd() {

}

void ComponentWeapons::onSDLPollEvent(SDL_Event *e, bool &finish) {
}

void ComponentWeapons::addWeaponType(std::string label) {
    this->weaponTypes.emplace_back(new WeaponType(label));
}

WeaponType *ComponentWeapons::getWeaponTypeByLabel(std::string label) {
    for (int i = 0; i < this->weaponTypes.size(); i++) {
        if (this->weaponTypes[i]->label == label) {
            return this->weaponTypes[i];
        }
    }

    return NULL;
}

WeaponType *ComponentWeapons::getWeaponTypeByClassname(std::string classname) {
    for (int i = 0; i < weaponTypes.size(); i++) {
        if (this->weaponTypes[i]->classname == classname) {
            return this->weaponTypes[i];
        }
    }

    return nullptr;
}

WeaponType *ComponentWeapons::getCurrentWeaponType() {
    return this->weaponTypes[currentWeaponIndex];
}

AmmoType *ComponentWeapons::getAmmoTypeByClassname(std::string classname) {
    for (int i = 0; i < ammoTypes.size(); i++) {
        if (this->ammoTypes[i]->getClassname() == classname) {
            return this->ammoTypes[i];
        }
    }

    return nullptr;
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

bool ComponentWeapons::isEmptyWeapon() {
    if (this->currentWeaponIndex >= 0) {
        return false;
    }

    return true;
}

void ComponentWeapons::setCurrentWeaponIndex(int currentWeaponIndex) {
    if (currentWeaponIndex != EngineSetup::WeaponsTypes::EMPTY) {
        // Si ya teníamos un arma, la deshabilitamos
        if (this->currentWeaponIndex != EngineSetup::WeaponsTypes::EMPTY) {
            this->getCurrentWeaponType()->getWeaponAnimations()->setEnabled(false);
        }

        ComponentWeapons::currentWeaponIndex = currentWeaponIndex;

        // Habilitamos la nueva selección
        this->getCurrentWeaponType()->getWeaponAnimations()->setEnabled(true);

        return;
    }

    ComponentWeapons::currentWeaponIndex = currentWeaponIndex;

}

void ComponentWeapons::shoot() {
    if (isEmptyWeapon()) {
        return;
    }

    Logging::getInstance()->Log("ComponentWeapons shoot!");

    if (getCurrentWeaponType()->getAmmoType()->getAmount() > 0) {
        this->getCurrentWeaponType()->shoot();
    } else {
        std::string soundLabel = getCurrentWeaponType()->getSoundEmptyLabel();
        Tools::playMixedSound(
                EngineBuffers::getInstance()->soundPackage->getSoundByLabel(soundLabel),
                EngineSetup::SoundChannels::SND_WEAPON,
                0
        );
    }
}

void ComponentWeapons::reload() {
    Logging::getInstance()->Log("ComponentWeapons reload!");

    if (getCurrentWeaponType()->getAmmoType()->getReloads() > 0) {
        this->getCurrentWeaponType()->reload();
    }
}

