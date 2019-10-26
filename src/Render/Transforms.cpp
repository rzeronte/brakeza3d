#include "../../headers/Render/Transforms.h"
#include "../../headers/Render/Tools.h"

Vertex3D Transforms::objectSpace(Vertex3D A, Object3D *o)
{
    Vertex3D V = A;
    V.setScaled(o->scale);

    V = o->getRotation() * V;
    V = V + *o->getPosition();

    V.u = A.u;
    V.v = A.v;

    return V;
}

Vertex3D Transforms::cameraSpace(Vertex3D V, Camera3D *cam)
{
    Vertex3D A = V;

    A = A - *cam->getPosition();
    A = cam->getRotation() * A;

    A.u = V.u;
    A.v = V.v;

    return A;
}

Vertex3D Transforms::R3ToEllipsoidSpace(Vertex3D V, Vertex3D eRadius)
{
    Vertex3D A = V;

    A.x = V.x / eRadius.x;
    A.y = V.y / eRadius.y;
    A.z = V.z / eRadius.z;

    return A;
}

Vertex3D Transforms::EllipsoidSpaceToR3(Vertex3D V, Vertex3D eRadius)
{
    Vertex3D A = V;

    A.x = V.x * eRadius.x;
    A.y = V.y * eRadius.y;
    A.z = V.z * eRadius.z;

    return A;
}

Vertex3D Transforms::NDCSpace(Vertex3D v, Camera3D *cam)
{
    Vertex3D vNL = cam->frustum->vNLpers;
    Vertex3D vNR = cam->frustum->vNRpers;

    Vertex3D vNT = cam->frustum->vNTpers;
    Vertex3D vNB = cam->frustum->vNBpers;

    // Perspective projection ( w = 1)
    Vertex3D A = Transforms::perspectiveDivision(v, cam);

    // NDC
    float Ax = A.x;                     // componente X de nuestro vértice en PANTALLA2D
    float vNLx = vNL.x;                 // Límite Izquierdo de PANTALLA2D
    float vNRx = vNR.x;                 // Límite Derecho de PANTALLA2D
    float tx0 = (Ax - vNLx);            // Distancia entre el límite Izquierdo y nuestro vértice
    float tx1 = 2 / ( vNRx - vNLx);    // Multiplicador (para 2 unidades, rango [0,2])
    float xt = (tx0 * tx1)  - 1;       // Calculamos el valor entre el rango [0,2], finalmente resta uno, tenemos [-1, 1]

    float Ay = A.y;
    float vNBy = vNB.y;
    float vNTy = vNT.y;
    float ty0 = (Ay - vNBy);
    float ty1 = 2 / (vNTy - vNBy);
    float yt = (ty0 * ty1)  - 1;

    A.x = xt;
    A.y = yt;

    A.z = v.z; // Almaceno z (deberia ser w)

    return A;
}

Point2D Transforms::screenSpace(Vertex3D V, Camera3D *cam)
{
    Point2D A;

    A.x = (1 + V.x) * ((float) EngineSetup::getInstance()->screenWidth / 2);
    A.y = (1 + V.y) * ((float) EngineSetup::getInstance()->screenHeight / 2);

    A.x = (int) A.x;
    A.y = (int) A.y;

    return A;
}

Vertex3D Transforms::perspectiveDivision(Vertex3D v, Camera3D *cam)
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
    Vertex3D T;

    T = V - *o->getPosition();
    T = o->getRotation().getTranspose() * T;

    T.x/=o->scale;
    T.y/=o->scale;
    T.z/=o->scale;


    T.u = V.u;
    T.v = V.v;

    return T;
}

Vertex3D Transforms::cameraToWorld(Vertex3D V,  Camera3D *cam)
{
    Vertex3D A;

    A = cam->getRotation().getTranspose() * V;
    A = A + *cam->getPosition();

    A.u = V.u;
    A.v = V.v;

    return A;
}

Vertex3D Transforms::Point2DToWorld(Point2D p, Camera3D *cam)
{
    // 0 ... 1
    float xt = Tools::interpolate(p.x, 0, EngineSetup::getInstance()->screenWidth);
    float yt = Tools::interpolate(p.y, 0, EngineSetup::getInstance()->screenHeight);

    //*******

    Vertex3D vNLs = cam->frustum->vNLs;
    Vertex3D vNRs = cam->frustum->vNRs;
    Vertex3D vNTs = cam->frustum->vNTs;
    Vertex3D vNBs = cam->frustum->vNBs;

    //***********
    float vNLx = vNLs.x;
    float vNRx = vNRs.x;
    float tx1 =  (xt) * (vNRx - vNLx);

    //***********
    float vNBy = vNBs.y;
    float vNTy = vNTs.y;
    float ty1 =  (yt) * (vNTy - vNBy);

    Vertex3D v;

    v.z = cam->frustum->nearDist;
    v.x = vNRx + tx1;
    v.y = vNTy + ty1;

    v = Transforms::cameraToWorld(v, cam);

    return v;
}
