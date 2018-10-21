#include "../../headers/Render/Transforms.h"
#include "../../headers/Render/Tools.h"
#include "../../headers/Objects/Triangle3D.h"
#include "../../headers/Render/Drawable.h"
#include "../../headers/Render/EngineBuffers.h"
#include "../../headers/Render/M3.h"
#include "../../headers/Render/Logging.h"
#include "../../headers/Render/Maths.h"

Vertex3D Transforms::objectSpace(Vertex3D A, Object3D *o)
{
    Vertex3D v = A;

    v = Maths::rotateVertex(v, *o->getRotation());
    v.addVertex(*o->getPosition());

    v.x*=o->scale;
    v.y*=o->scale;
    v.z*=o->scale;

    return v;
}

Vertex3D Transforms::cameraSpace(Vertex3D V, Camera3D *cam)
{
    Vertex3D A = V;

    A.subVertex(*cam->getPosition());
    A = Maths::rotateVertex(A, *cam->getRotation());

    A.u = V.u; A.v = V.v;

    return A;
}

Vertex3D Transforms::homogeneousClipSpace(Vertex3D v, Camera3D *cam)
{
    Vertex3D vNL = cam->frustum->near_left.vertex1;
    Vertex3D vNR = cam->frustum->near_right.vertex1;
    if (vNL.x < vNR.x) {
        Vertex3D tmp = vNR;
        vNR = vNL;
        vNL = tmp;
    }

    Vertex3D vNT = cam->frustum->near_top.vertex1;
    Vertex3D vNB = cam->frustum->near_bottom.vertex1;
    if (vNT.y < vNB.y) {
        Vertex3D tmp = vNT;
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
    Vertex3D A = Transforms::screenSpacePerspective(v, cam);

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

Point2D Transforms::screenSpace(Vertex3D V, Camera3D *cam)
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

Vertex3D Transforms::screenSpacePerspective(Vertex3D v, Camera3D *cam)
{
    Vertex3D A = v;
    if (v.z != 0 || v.z != -0) {
        A.x =  - ( ( cam->frustum->nearDist * v.x) / v.z ) ;
        A.y =  - ( ( cam->frustum->nearDist * v.y) / v.z );
    }

    return A;
}


Vertex3D Transforms::objectToLocal(Vertex3D V, Object3D *o)
{
    Vertex3D T = V;

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
