
#ifndef BRAKEDA3D_WEAPON3D_H
#define BRAKEDA3D_WEAPON3D_H


#include "Sprite3D.h"
#include "Mesh3D.h"
#include "Camera3D.h";
class Weapon3D : public Mesh3D {
public:
    float head_vertical = 180;

    float farDist = 1;
    float XOffset = -0.5;
    float YOffset = 0.75;

    void HeadBob();
    void setup(float farDist, float XOffset, float YOffset);
    void updateWeaponPosition(Camera3D* camera);

};


#endif //BRAKEDA3D_WEAPON3D_H
