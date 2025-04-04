
#ifndef SDL2_3D_ENGINE_DRAWABLE_H
#define SDL2_3D_ENGINE_DRAWABLE_H


#include <SDL2/SDL_surface.h>
#include "Frustum.h"
#include "../Components/Camera3D.h"
#include "../Objects/Line2D.h"
#include "../Objects/Cube3D.h"
#include "Billboard.h"
#include "../Misc/Octree.h"
#include "../Misc/Grid3D.h"
#include "ImGuizmo.h"

class Drawable {
public:
    static void drawVertex(Vertex3D v, Color color);

    static void drawVector3D(Vector3D V, Color color);

    static void drawObject3DAxis(Object3D *object, bool drawUp, bool drawRight, bool drawForward);

    static void drawMainAxis();

    static void drawMainAxisOffset(Vertex3D offset);

    static void drawLightning(Vertex3D, Vertex3D, Color color);

    static void drawAABB(AABB3D *, Color);

    static void drawOctree(Octree *);

    static void drawOctreeNode(OctreeNode &);

    static void drawGrid3D(Grid3D *);

    static void drawObject3DGizmo(Object3D *o, glm::mat4 objectMatrix, glm::mat4 viewMatrix, glm::mat4 projectionMatrix);

    static void drawGrid3DMakeTravel(Grid3D *grid);
};


#endif //SDL2_3D_ENGINE_DRAWABLE_H
