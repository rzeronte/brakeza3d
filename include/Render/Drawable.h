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
};


#endif //SDL2_3D_ENGINE_DRAWABLE_H
