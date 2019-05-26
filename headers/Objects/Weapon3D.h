
#ifndef BRAKEDA3D_WEAPON3D_H
#define BRAKEDA3D_WEAPON3D_H


#include "Sprite3D.h"
#include "Mesh3D.h"

class Weapon3D : public Mesh3D {
public:
    void setWeaponPosition(Camera3D *cam);
};


#endif //BRAKEDA3D_WEAPON3D_H
