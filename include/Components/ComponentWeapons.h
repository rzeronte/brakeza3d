
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

enum WeaponsActions {
    NONE = -1,
    IDLE = 0,
    FIRE = 1,
    RELOAD = 2,
};

enum WeaponsTypes {
    EMPTY = -1,
    DEFAULT = 0,
    REPEATER = 1,
    STATIC_RIFLE = 2,
    HAR = 3,
    CHAINGUN = 4,
    GAUSS_CANNON = 5,
    RAILGUN = 6,
    ROCKETLAUNCHER = 7,
};

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

    void addWeaponType(const std::string&);

    WeaponType *getWeaponTypeByLabel(const std::string& label);

    void setCurrentWeaponIndex(int newCurrentWeaponIndex);

    WeaponType *getCurrentWeaponType();

    bool isEmptyWeapon() const;

    void headBob(Vector3D velocity);
};


#endif //BRAKEDA3D_COMPONENTWEAPONS_H
