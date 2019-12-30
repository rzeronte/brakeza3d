
#include <SDL_image.h>
#include "../../headers/2D/WeaponsManager.h"
#include "../../headers/Render/Logging.h"
#include "../../headers/Render/Maths.h"

WeaponsManager::WeaponsManager()
{
    this->currentWeapon = EngineSetup::getInstance()->WeaponsTypes::PISTOL;
}

void WeaponsManager::addWeaponType(std::string label)
{
    this->weaponsType[numWeaponsType] = new WeaponType(label);
    numWeaponsType++;
}

WeaponType* WeaponsManager::getWeaponTypeByLabel(std::string label)
{
    for (int i = 0; i < numWeaponsType; i++) {
        if (this->weaponsType[i]->label == label) {
            return this->weaponsType[i];
        }
    }

    return NULL;
}

WeaponType* WeaponsManager::getWeaponTypeByClassname(std::string classname)
{
    for (int i = 0; i < numWeaponsType; i++) {
        if (this->weaponsType[i]->classname == classname) {
            return this->weaponsType[i];
        }
    }

    return NULL;
}

WeaponType* WeaponsManager::getCurrentWeaponType()
{
    return this->weaponsType[currentWeapon];
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

