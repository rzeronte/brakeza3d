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
// Created by Eduardo on 06/04/2025.
//

#ifndef BRAKEZA3D_COLLISIONINFO_H
#define BRAKEZA3D_COLLISIONINFO_H


#include "../Config.h"
class Object3D;

class CollisionInfo {

public:
    CollisionInfo(void *with, int source, int boneIndexMapping);

    void *with;
    int source;
    int boneIndexMapping;

    int getSource() const;

    int getBoneIndexMapping() const;

    Object3D* getObject() const; // solo forward declaration es suficiente

};


#endif //BRAKEZA3D_COLLISIONINFO_H
