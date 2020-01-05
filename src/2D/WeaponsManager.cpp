
#include <SDL_image.h>
#include "../../headers/2D/WeaponsManager.h"
#include "../../headers/Render/Logging.h"
#include "../../headers/Render/Maths.h"

WeaponsManager::WeaponsManager()
{
    this->currentWeaponIndex = EngineSetup::getInstance()->WeaponsTypes::PISTOL;
}

void WeaponsManager::addWeaponType(std::string label)
{
    this->weaponTypes.emplace_back( new WeaponType(label) );
}

WeaponType* WeaponsManager::getWeaponTypeByLabel(std::string label)
{
    for (int i = 0; i < this->weaponTypes.size() ; i++) {
        if (this->weaponTypes[i]->label == label) {
            return this->weaponTypes[i];
        }
    }

    return NULL;
}

WeaponType* WeaponsManager::getWeaponTypeByClassname(std::string classname)
{
    for (int i = 0; i < weaponTypes.size() ; i++) {
        if (this->weaponTypes[i]->classname == classname) {
            return this->weaponTypes[i];
        }
    }

    return nullptr;
}

WeaponType* WeaponsManager::getCurrentWeaponType()
{
    return this->weaponTypes[currentWeaponIndex];
}

AmmoType *WeaponsManager::getAmmoTypeByClassname(std::string classname)
{
    for (int i = 0; i < ammoTypes.size(); i++) {
        if (this->ammoTypes[i]->getClassname() == classname) {
            return this->ammoTypes[i];
        }
    }

    return nullptr;
}

void WeaponsManager::onUpdate(Camera3D *cam, SDL_Surface *dst)
{
    this->getCurrentWeaponType()->onUpdate();

    this->headBob(cam->velocity);

    if (getCurrentWeaponType()->isSniperEnabled()) {
        SDL_BlitSurface(getCurrentWeaponType()->sniperHUD, NULL, dst, NULL);
        return;
    }

    this->getCurrentWeaponType()->getCurrentWeaponAnimation()->draw(dst, (int)this->offsetX, (int)this->offsetY);
}

void WeaponsManager::headBob(Vector3D velocity)
{
    Vertex3D v = velocity.getComponent();

    // head bob
    if (abs(v.x) > 0.5 || abs(v.z) > 0.5 ) {
        this->offsetY = sin( Maths::degreesToRadians(this->headBobOffsetY) );
        this->offsetY = abs(this->offsetY) * 7;

        this->headBobOffsetY += 10;
        if (this->headBobOffsetY > 360) {
            this->headBobOffsetY = 0;
        }
    } else {
        this->headBobOffsetY = 0;
    }
}

bool WeaponsManager::isEmptyWeapon() {
    if (this->currentWeaponIndex != 0) {
        return false;
    }
}

