//
// Created by eduardo on 10/2/22.
//

#ifndef BRAKEDA3D_SIMPLEPENDULUM_H
#define BRAKEDA3D_SIMPLEPENDULUM_H


#include "../Objects/Object3D.h"
#include "../Misc/Timer.h"

class SimplePendulum: public Object3D {
public:
    SimplePendulum(float angleX, float angleZ, float length, float friction);

    float angleX;
    float angleZ;

    M3 pendulumRotation;

    float friction;
    bool activeFriction;

    float length;
    float t;
    void onUpdate() override;
};


#endif //BRAKEDA3D_SIMPLEPENDULUM_H
