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
// Created by Eduardo on 25/11/2025.
//

#ifndef BRAKEZA3D_SCENEOBJECTTYPES_H
#define BRAKEZA3D_SCENEOBJECTTYPES_H

enum class ObjectType {
    Object3D = 1,
    Mesh3D = 2,
    Mesh3DAnimation = 3,
    LightPoint = 4,
    LightSpot = 5,
    ParticleEmitter = 6,
    Image3DAnimation = 7,
    Image3DAnimation360 = 8,
    Image2DAnimation = 9,
    Image3D = 10,
    Image2D = 11,
    Swarm = 12,
    Sound3D = 13
};

#endif //BRAKEZA3D_SCENEOBJECTTYPES_H