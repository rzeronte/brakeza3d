
#ifndef SDL2_3D_ENGINE_RENDER_H
#define SDL2_3D_ENGINE_RENDER_H


#include "../Objects/Vertex3D.h"
#include "../Components/Camera3D.h"
#include "../Objects/Point2D.h"
#include "Texture.h"
#include "../Objects/LightPoint3D.h"

class Transforms {
public:

    static Vertex3D objectToLocal(Vertex3D &V, Object3D *o);

    static Vertex3D cameraToWorld(Vertex3D &V, Camera3D *cam);

    static Vertex3D Point2DToWorld(Point2D &p, Camera3D *cam);

    static void objectSpace(Vertex3D &dst, Vertex3D &src, Object3D *o);

    static Vertex3D PerspectiveNDCSpace(Vertex3D &V, Frustum *frustum);
    static Vertex3D OrthographicNDCSpace(Vertex3D &V, Frustum *frustum);

    static void cameraSpace(Vertex3D &dst, Vertex3D &src, Object3D *cam);

    static Vertex3D perspectiveDivision(Vertex3D &V, float nearDist);

    static void screenSpace(Point2D &P, Vertex3D &V);

};


#endif //SDL2_3D_ENGINE_RENDER_H
