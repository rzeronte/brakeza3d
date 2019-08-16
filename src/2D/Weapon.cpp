
#include <SDL_image.h>
#include "../../headers/2D/Weapon.h"
#include "../../headers/Render/EngineSetup.h"
#include "../../headers/Render/Logging.h"
#include "../../headers/Render/Maths.h"

Weapon::Weapon()
{
    this->currentWeapon = EngineSetup::getInstance()->WeaponsTypes::WEAPON_TYPE_MELEE;
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
        this->getCurrentWeaponType()->currentAnimationIndex = EngineSetup::getInstance()->WeaponsActions::WEAPON_ACTION_WALK;
    }

    if (isFiring) {
        this->getCurrentWeaponType()->currentAnimationIndex = EngineSetup::getInstance()->WeaponsActions::WEAPON_ACTION_FIRE;
    }
}

void Weapon::onUpdate(Camera3D *cam, bool isFiring, SDL_Surface *dst, Vector3D velocity)
{
    this->getCurrentWeaponType()->updateCadenceTimer();
    this->getCurrentWeaponType()->getCurrentWeaponAnimation()->updateFrame();
    this->setAction(cam, isFiring);
    this->headBob(velocity);
    this->getCurrentWeaponType()->getCurrentWeaponAnimation()->draw(dst, (int)this->offsetX, (int)this->offsetY);
}

void Weapon::headBob(Vector3D velocity)
{
    Vertex3D v = velocity.getComponent();

    // head bob
    if (abs(v.x) > 0.5 || abs(v.z) > 0.5 ) {
        this->offsetY = sin( Maths::degreesToRadians(this->headBobOffsetY) );
        this->offsetY = abs(this->offsetY) * 7;

        this->headBobOffsetY += 20;
        if (this->headBobOffsetY > 360) {
            this->headBobOffsetY = 0;
        }
    } else {
        this->headBobOffsetY = 0;
    }
}