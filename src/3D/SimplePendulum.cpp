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

#include "../../include/3D/SimplePendulum.h"
#include "../../include/Config.h"
#include "../../include/Brakeza.h"

SimplePendulum::SimplePendulum(float angleX, float angleZ, float length, float friction)
{
    this->angleX = angleX;
    this->angleZ = angleZ;
    this->length = length;
    this->activeFriction = true;
    this->friction = friction;
    this->t = 0;
    this->pendulumRotation = M3::getMatrixIdentity();
}

void SimplePendulum::onUpdate() {
    Object3D::onUpdate();
    t += Brakeza::get()->getDeltaTime();
    float w = sqrtf(abs(Config::get()->gravity.y)/this->length);

    float alpha = this->angleX * cosf(w*this->t);
    float gamma = 0;
    float theta = this->angleZ * cosf(w*this->t);

    if (activeFriction) {
        alpha = alpha * expf(-this->friction * t);
        theta = theta * expf(-this->friction * t);
    }

    Vertex3D start(0, 0, 0);
    Vertex3D end(0, this->length, 0);

    pendulumRotation = getRotation() * M3::getMatrixRotationForEulerAngles(alpha, gamma, theta);

    start = getPosition() + (pendulumRotation * start);
    end = getPosition() + (pendulumRotation * end);

    //Vector3D V(start, end);
    //Drawable::drawVector3D(V, Color::fuchsia());
}

