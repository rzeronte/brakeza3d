
#ifndef SDL2_3D_ENGINE_DRAWABLE_H
#define SDL2_3D_ENGINE_DRAWABLE_H


#include <SDL_surface.h>
#include "Frustum.h"
#include "Camera.h"
#include "Billboard.h"
#include "GUI/GUI.h"

class Drawable {
public:
    static void drawBox2D(SDL_Rect r);
    static void drawPlaneNormalVector(Plane *object, Camera *cam, Uint32 col);
    static void drawVertex(Vertex v, Camera *cam, Uint32 color);
    static void drawVector3D(Vector3D V, Camera *cam, Uint32 color);
    static void drawObject3DAxis(Object3D *object, Camera *cam, bool drawUp, bool drawRight, bool drawForward);
    static void drawFrustum(Frustum *f, Camera *cam, bool drawNP, bool drawFP, bool drawSides);
    static void drawPlane(Plane P, Camera *cam, Uint32 color);
    static void drawMainAxis(Camera *cam);
    static void drawBillboard(Billboard *B, Camera *cam);
};


#endif //SDL2_3D_ENGINE_DRAWABLE_H
