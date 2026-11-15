/*
 * Brakeza3D - 3D/2D game engine
 * Author:  Eduardo Rodríguez Álvarez
 * GitHub:  https://github.com/rzeronte/brakeza3d
 * Web:     https://brakeza.com
 * Started: 2018-09-30
 * Copyright (C) 2018-2026 Eduardo Rodríguez Álvarez
 * License: GNU General Public License v3.0 (see LICENSE)
 */
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
