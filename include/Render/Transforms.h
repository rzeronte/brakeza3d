/*
 * Brakeza3D - 3D/2D game engine
 * Author:  Eduardo Rodríguez Álvarez
 * GitHub:  https://github.com/rzeronte/brakeza3d
 * Web:     https://brakeza.com
 * Started: 2018-09-30
 * Copyright (C) 2018-2026 Eduardo Rodríguez Álvarez
 * License: GNU General Public License v3.0 (see LICENSE)
 */

#ifndef SDL2_3D_ENGINE_RENDER_H
#define SDL2_3D_ENGINE_RENDER_H


#include "../3D/Vertex3D.h"
#include "../Components/Camera3D.h"
#include "Point2D.h"
#include "../3D/LightPoint.h"

class Transforms {
public:

    static Vertex3D objectToLocal(Vertex3D &V, Object3D *o);

    static Vertex3D Point2DToWorld(Point2D &p);

    static void objectSpace(Vertex3D &dst, Vertex3D &src, Object3D *o);

    static Point2D WorldToPoint(Vertex3D V);
};


#endif //SDL2_3D_ENGINE_RENDER_H
