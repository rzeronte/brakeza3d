/*
 * Brakeza3D - 3D/2D game engine
 * Author:  Eduardo Rodríguez Álvarez
 * GitHub:  https://github.com/rzeronte/brakeza3d
 * Web:     https://brakeza.com
 * Started: 2018-09-30
 * Copyright (C) 2018-2026 Eduardo Rodríguez Álvarez
 * License: GNU General Public License v3.0 (see LICENSE)
 */
#ifndef SDL2_3D_ENGINE_DRAWABLE_H
#define SDL2_3D_ENGINE_DRAWABLE_H

#include "Frustum.h"
#include "../Components/Camera3D.h"
#include "../3D/Cube3D.h"
#include "Octree.h"
#include "Grid3D.h"

class Drawable {
public:
    static void drawVertex(const Vertex3D &v, Color color);
    static void drawVector3D(const Vector3D &V, const Color &color);
    static void drawObject3DAxis(Object3D *object, bool drawUp, bool drawRight, bool drawForward);
    static void drawMainAxisOffset(const Vertex3D &offset);
    static void drawLightning(Vertex3D, Vertex3D, Color color);
    static void drawAABB(AABB3D *, const Color &);
    static void drawOctree(Octree *);
    static void drawOctreeNode(OctreeNode &);
    static void drawGrid3D(Grid3D *);
    static void drawObject3DGizmo(Object3D *o, const glm::mat4 &objectMatrix, glm::mat4 viewMatrix, glm::mat4 projectionMatrix);
    static void drawGrid3DMakeTravel(Grid3D *grid);
    static void WarningMessage(const char *msg);
    static void ImGuiLink(const char* label, const char* url);
};


#endif //SDL2_3D_ENGINE_DRAWABLE_H
