#include "../../include/Render/Transforms.h"
#include "../../include/Misc/Tools.h"
#include "../../include/EngineSetup.h"
#include "../../include/Render/M4.h"

void Transforms::objectSpace(Vertex3D &dst, Vertex3D &src, Object3D *o) {
    dst = src;
    dst.setScaled(o->scale);

    dst = o->getRotation() * dst;
    dst = dst + o->getPosition();

    dst = dst + o->drawOffset;

    dst.u = src.u;
    dst.v = src.v;
}

void Transforms::cameraSpace(Vertex3D &dst, Vertex3D &src, Object3D *cam) {
    dst = src;

    dst = dst - cam->getPosition();
    dst = cam->getRotation() * dst;

    dst.u = src.u;
    dst.v = src.v;
}

Vertex3D Transforms::PerspectiveNDCSpace(Vertex3D &V, Frustum *frustum) {
    const float f = frustum->farDist;
    const float n = frustum->nearDist;

    const float l = frustum->vNLs.x;
    const float r = frustum->vNRs.x;

    const float b = frustum->vNBs.y;
    const float t = frustum->vNTs.y;

    Vertex3D R(M4::getFrustumProjection(f, n, l, r, b, t) * V.createVertex4D());

    R.u = V.u;
    R.v = V.v;
    R.z = -V.z;

    return R.getInverse();
}

Vertex3D Transforms::OrthographicNDCSpace(Vertex3D &V, Frustum *frustum)
{
    const float f = frustum->farDist;
    const float n = frustum->nearDist;

    const float l = frustum->vNLs.x;
    const float r = frustum->vNRs.x;

    const float b = frustum->vNBs.y;
    const float t = frustum->vNTs.y;

    Vertex3D R(M4::getFrustumOrthographic(f, n, l, r, b, t) * V.createVertex4D());

    R.u = V.u;
    R.v = V.v;
    R.z = -V.z;

    return R.getInverse();
}

void Transforms::screenSpace(Point2D &P, Vertex3D &V) {
    P.x = (int) ((1 + V.x) * EngineSetup::get()->screenWidthHalf);
    P.y = (int) ((1 + V.y) * EngineSetup::get()->screenHeightHalf);
}

Vertex3D Transforms::perspectiveDivision(Vertex3D &V, float nearDist) {
    Vertex3D A = V;

    A.x = -(nearDist * V.x) / V.z;
    A.y = -(nearDist * V.y) / V.z;

    return A;
}

Vertex3D Transforms::objectToLocal(Vertex3D &V, Object3D *o) {
    Vertex3D T;

    T = V - o->drawOffset;

    T = T - o->getPosition();
    T = o->getRotation().getTranspose() * T;

    T.x /= o->scale;
    T.y /= o->scale;
    T.z /= o->scale;

    T.u = V.u;
    T.v = V.v;

    return T;
}

Vertex3D Transforms::cameraToWorld(Vertex3D &V, Camera3D *cam) {
    Vertex3D A;

    A = cam->getRotation().getTranspose() * V;
    A = A + cam->getPosition();

    A.u = V.u;
    A.v = V.v;

    return A;
}

Vertex3D Transforms::Point2DToWorld(Point2D &p, Camera3D *cam) {
    // 0 ... 1
    float xt = Tools::interpolate(p.x, 0, EngineSetup::get()->screenWidth);
    float yt = Tools::interpolate(p.y, 0, EngineSetup::get()->screenHeight);

    //*******

    Vertex3D vNLs = cam->frustum->vNLs;
    Vertex3D vNRs = cam->frustum->vNRs;
    Vertex3D vNTs = cam->frustum->vNTs;
    Vertex3D vNBs = cam->frustum->vNBs;

    //***********
    float vNLx = vNLs.x;
    float vNRx = vNRs.x;
    float tx1 = (xt) * (vNRx - vNLx);

    //***********
    float vNBy = vNBs.y;
    float vNTy = vNTs.y;
    float ty1 = (yt) * (vNTy - vNBy);

    Vertex3D v;

    v.z = cam->frustum->nearDist;
    v.x = vNRx + tx1;
    v.y = vNTy + ty1;

    v = Transforms::cameraToWorld(v, cam);

    return v;
}
