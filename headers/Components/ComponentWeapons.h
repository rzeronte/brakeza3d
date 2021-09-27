
#ifndef BRAKEDA3D_COMPONENTWEAPONS_H
#define BRAKEDA3D_COMPONENTWEAPONS_H

#include <string>
#include <SDL2/SDL.h>
#include "../Misc/Timer.h"
#include "../2D/WeaponAnimation.h"
#include "../Game/WeaponType.h"
#include "Camera3D.h"
#include "../Game/AmmoType.h"
#include "Component.h"
#include "ComponentCamera.h"

class ComponentWeapons : public Component {
public:
    ComponentWeapons();

    void onStart();

    void preUpdate();

    void onUpdate();

    void postUpdate();

    void onEnd();

    void onSDLPollEvent(SDL_Event *event, bool &finish);

    void shoot();

    void reload();

    int currentWeaponIndex = -1;

    float headBobOffsetY = 0;

    std::vector<WeaponType *> weaponTypes;
    std::vector<AmmoType *> ammoTypes;

    // Global Offset When drawing
    float offsetX = 0;
    float offsetY = 0;

    void addWeaponType(std::string);

    WeaponType *getWeaponTypeByLabel(const std::string& label);

    void setCurrentWeaponIndex(int newCurrentWeaponIndex);

    WeaponType *getWeaponTypeByClassname(const std::string& label);

    WeaponType *getCurrentWeaponType();

    AmmoType *getAmmoTypeByClassname(const std::string& label);

    bool isEmptyWeapon() const;

    void headBob(Vector3D velocity);
};


#endif //BRAKEDA3D_COMPONENTWEAPONS_H
