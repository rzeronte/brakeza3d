
#include <SDL_image.h>
#include "../../headers/2D/Weapon.h"
#include "../../headers/Render/EngineSetup.h"
#include "../../headers/Render/Logging.h"

Weapon::Weapon()
{

}

void Weapon::addWeaponType(std::string label)
{
    this->weaponsType[numWeaponsType] = new WeaponType(label);
    numWeaponsType++;
}

WeaponType* Weapon::getWeaponTypeByLabel(std::string label)
{
    for (int i = 0; i < numWeaponsType; i++) {
        if (this->weaponsType[i]->label == label) {
            return this->weaponsType[i];
        }
    }

    return NULL;
}

WeaponType* Weapon::getCurrentWeaponType()
{
    return this->weaponsType[currentWeapon];
}

void Weapon::setAction(Camera3D *cam, bool isFiring)
{
    if (cam->kinematicController->onGround()) {
        this->getCurrentWeaponType()->current_animation = EngineSetup::getInstance()->WeaponsActions::WEAPON_ACTION_WALK;
    }

    if (isFiring) {
        this->getCurrentWeaponType()->current_animation = EngineSetup::getInstance()->WeaponsActions::WEAPON_ACTION_FIRE;
    }
}