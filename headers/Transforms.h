
#ifndef SDL2_3D_ENGINE_RENDER_H
#define SDL2_3D_ENGINE_RENDER_H


#include "Vertex.h"
#include "Camera.h"
#include "Point2D.h"
#include "Texture.h"
#include "LightPoint.h"

class Transforms {
public:

    static Vertex objectToLocal(Vertex V, Object3D *o);


    static Vertex objectSpace(Vertex A, Object3D *o);
    static Vertex homogeneousClipSpace(Vertex v, Camera *cam);
    static Vertex cameraSpace(Vertex V, Camera *cam);

    static Vertex screenSpacePerspective(Vertex v, Camera *cam);
    static Point2D screenSpace(Vertex, Camera *cam);



};


#endif //SDL2_3D_ENGINE_RENDER_H
