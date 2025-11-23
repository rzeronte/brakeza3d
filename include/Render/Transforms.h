
#ifndef SDL2_3D_ENGINE_RENDER_H
#define SDL2_3D_ENGINE_RENDER_H


#include "../Objects/Vertex3D.h"
#include "../Components/Camera3D.h"
#include "../Objects/Point2D.h"
#include "../Objects/LightPoint.h"

class Transforms {
public:

    static Vertex3D objectToLocal(Vertex3D &V, Object3D *o);

    static Vertex3D Point2DToWorld(Point2D &p);

    static void objectSpace(Vertex3D &dst, Vertex3D &src, Object3D *o);

    static Point2D WorldToPoint(Vertex3D V);
};


#endif //SDL2_3D_ENGINE_RENDER_H
