#include "../../headers/Transforms.h"
#include "../../headers/Tools.h"
#include "../../headers/Triangle.h"
#include "../../headers/Drawable.h"
#include "../../headers/EngineBuffers.h"
#include "../../headers/M3.h"
#include "../../headers/Logging.h"
#include "../../headers/Maths.h"

Vertex Transforms::objectSpace(Vertex A, Object3D *o)
{
    Vertex v = A;

    v = Maths::rotateVertex(v, *o->getRotation());
    v.addVertex(*o->getPosition());

    v.x*=o->scale;
    v.y*=o->scale;
    v.z*=o->scale;

    return v;
}

Vertex Transforms::cameraSpace(Vertex V, Camera *cam)
{
    Vertex A = V;

    A.subVertex(*cam->getPosition());
    A = Maths::rotateVertex(A, *cam->getRotation());

    A.u = V.u; A.v = V.v;

    return A;
}

Vertex Transforms::homogeneousClipSpace(Vertex v, Camera *cam)
{
    Vertex vNL = cam->frustum->near_left.vertex1;
    Vertex vNR = cam->frustum->near_right.vertex1;
    if (vNL.x < vNR.x) {
        Vertex tmp = vNR;
        vNR = vNL;
        vNL = tmp;
    }

    Vertex vNT = cam->frustum->near_top.vertex1;
    Vertex vNB = cam->frustum->near_bottom.vertex1;
    if (vNT.y < vNB.y) {
        Vertex tmp = vNT;
        vNT = vNB;
        vNB = tmp;
    }

    vNL = Transforms::cameraSpace(vNL, cam);
    vNR = Transforms::cameraSpace(vNR, cam);
    vNT = Transforms::cameraSpace(vNT, cam);
    vNB = Transforms::cameraSpace(vNB, cam);

    // get 2d coordinates
    vNL = Transforms::screenSpacePerspective(vNL, cam);
    vNR = Transforms::screenSpacePerspective(vNR, cam);
    vNT = Transforms::screenSpacePerspective(vNT, cam);
    vNB = Transforms::screenSpacePerspective(vNB, cam);

    // Perspective projection ( w = 1)
    Vertex A = Transforms::screenSpacePerspective(v, cam);

    // HomogeneousClipSpace
    float Ax = A.x;
    float vNLx = vNL.x;
    float vNRx = vNR.x;
    float tx0 = (Ax - vNLx);
    float tx1 =  2 / ( vNRx - vNLx);
    float xt =  (tx0 * tx1)  - 1;

    float Ay = A.y;
    float vNBy = vNB.y;
    float vNTy = vNT.y;
    float ty0 = (Ay - vNBy);
    float ty1 =  2 / (vNTy - vNBy);
    float yt =  (ty0 * ty1)  - 1;

    A.x = xt;
    A.y = yt;

    A.z = v.z;      // Almaceno z (deberia ser w)

    return A;
}

Point2D Transforms::screenSpace(Vertex V, Camera *cam)
{
    Point2D A;
    A.x = V.x * cam->pixelPerUnit;
    A.y = V.y * cam->pixelPerUnit;

    A.x = (1 + V.x) * ((float) EngineSetup::getInstance()->SCREEN_WIDTH/2);
    A.y = (1 + V.y) * ((float) EngineSetup::getInstance()->SCREEN_HEIGHT/2);

    A.x = (int) A.x;
    A.y = (int) A.y;

    return A;
}

Vertex Transforms::screenSpacePerspective(Vertex v, Camera *cam)
{
    Vertex A = v;
    if (v.z != 0 || v.z != -0) {
        A.x =  - ( ( cam->frustum->nearDist * v.x) / v.z ) ;
        A.y =  - ( ( cam->frustum->nearDist * v.y) / v.z );
    }

    return A;
}


Vertex Transforms::objectToLocal(Vertex V, Object3D *o)
{
    Vertex T = V;

    T.x*=o->scale;
    T.y*=o->scale;
    T.z*=o->scale;

    T.subVertex(*o->getPosition());

    M3 MRX = M3::RX(-o->getRotation()->x);
    M3 MRY = M3::RY(-o->getRotation()->y);
    M3 MRZ = M3::RZ(-o->getRotation()->z);

    T = (MRZ * MRY * MRX ) * T;

    return T;
}
