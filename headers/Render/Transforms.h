
#ifndef SDL2_3D_ENGINE_RENDER_H
#define SDL2_3D_ENGINE_RENDER_H


#include "../Objects/Vertex3D.h"
#include "../Objects/Camera3D.h"
#include "../Objects/Point2D.h"
#include "Texture.h"
#include "../Objects/LightPoint3D.h"

class Transforms {
public:

    static Vertex3D objectToLocal(Vertex3D V, Object3D *o);
    static Vertex3D cameraToWorld(Vertex3D V,  Camera3D *cam);
    static Vertex3D Point2DToWorld(Point2D p, Camera3D *cam);

    static Vertex3D objectSpace(Vertex3D A, Object3D *o);
    static Vertex3D NDCSpace(Vertex3D v, Camera3D *cam);
    static Vertex3D cameraSpace(Vertex3D V, Camera3D *cam);

    static Vertex3D R3ToEllipsoidSpace(Vertex3D V, Vertex3D eRadius);
    static Vertex3D EllipsoidSpaceToR3(Vertex3D V, Vertex3D eRadius);

    static Vertex3D perspectiveDivision(Vertex3D v, Camera3D *cam);
    static Point2D screenSpace(Vertex3D, Camera3D *cam);

};


#endif //SDL2_3D_ENGINE_RENDER_H
