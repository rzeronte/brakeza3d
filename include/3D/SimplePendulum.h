//
// Created by eduardo on 10/2/22.
//

#ifndef BRAKEDA3D_SIMPLEPENDULUM_H
#define BRAKEDA3D_SIMPLEPENDULUM_H


#include "Object3D.h"

class SimplePendulum: public Object3D {
public:
    SimplePendulum(float angleX, float angleZ, float length, float friction);

    bool activeFriction;
    float angleX;
    float angleZ;
    float friction;
    float length;
    float t;

    M3 pendulumRotation;
    void onUpdate() override;
};


#endif //BRAKEDA3D_SIMPLEPENDULUM_H
