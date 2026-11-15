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

#include "../../include/Render/CollisionInfo.h"

CollisionInfo::CollisionInfo(
    void *with,
    int source,
    int boneIndexMapping
)
:
    with(with),
    source(source),
    boneIndexMapping(boneIndexMapping)
{

}

int CollisionInfo::getSource() const {
    return source;
}

int CollisionInfo::getBoneIndexMapping() const {
    return boneIndexMapping;
}

Object3D *CollisionInfo::getObject() const {
    return (Object3D*) with;
}
