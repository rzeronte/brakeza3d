
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
#include "../Misc/PathFinder.h"

class Drawable {
public:
    static void drawVertex(Vertex3D v, Camera3D *cam, Color color);

    static void drawVector3D(Vector3D V, Color color);

    static void drawLightingVector3D(Vector3D V, Camera3D *cam, Color color);

    static void drawLineLighting(Line2D L, Color color);

    static void drawObject3DAxis(Object3D *object, Camera3D *cam, bool drawUp, bool drawRight, bool drawForward);

    static void drawMainAxis(Camera3D *cam);

    static void drawMainAxisOffset(Camera3D *cam, Vertex3D offset);

    static void drawLightning(Vertex3D, Vertex3D, Color color);

    static void drawAABB(AABB3D *, Color);

    static void drawOctree(Octree *, bool onlyWithTriangles);

    static void drawOctreeNode(OctreeNode *, bool onlyWithTriangles);

    static void drawGrid3D(Grid3D *);

    static void drawPathInGrid(Grid3D *, std::stack<PairData>);

    static void drawPathDebugForDevelopment(Grid3D *grid, PathFinder *pathfinder);

};


#endif //SDL2_3D_ENGINE_DRAWABLE_H
