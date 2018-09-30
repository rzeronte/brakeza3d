//
// Created by darkhead on 12/7/18.
//

#ifndef SDL2_3D_ENGINE_DRAWABLE_H
#define SDL2_3D_ENGINE_DRAWABLE_H


#include <SDL_surface.h>
#include "Frustum.h"
#include "Camera.h"
#include "Billboard.h"
#include "GUI/GUI.h"

class Drawable {
public:
    static void drawBox2D(SDL_Surface *surface, SDL_Rect r);

    static void drawPlaneNormalVector(SDL_Surface *screen, Plane *object, Camera *cam, Uint32 col);
    static void drawVertex(SDL_Surface *screen, Vertex v, Camera *cam, Uint32 color);

    static void drawVector3D(SDL_Surface *screen, Vector3D V, Camera *cam, Uint32 color);

    static void drawObject3DAxis(SDL_Surface *screen, Object3D *object, Camera *cam, bool drawUp, bool drawRight, bool drawForward);

    static void drawFrustum(SDL_Surface *surface, Frustum *f, Camera *cam, bool drawNP, bool drawFP, bool drawSides);

    static void drawPlane(SDL_Surface *screen, Plane P, Camera *cam, Uint32 color);

    static void drawMainAxis(SDL_Surface *screen, Camera *cam);

    static void drawBillboard(SDL_Surface *screen, Billboard *B, Camera *cam);
    };


#endif //SDL2_3D_ENGINE_DRAWABLE_H
