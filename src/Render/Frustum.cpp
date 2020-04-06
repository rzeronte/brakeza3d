//
// Created by darkhead on 29/5/18.
//

#include <iostream>
#include "../../headers/Render/Frustum.h"
#include "../../headers/Misc/Color.h"
#include "../../headers/Render/Transforms.h"
#include "../../headers/EngineSetup.h"

Frustum::Frustum() {}

Frustum::Frustum(Vertex3D position, Vertex3D direction, Vertex3D up, Vertex3D right, float nearDist, float Hnear, float Wnear, float farDist, float Hfar, float Wfar)
{
    setup(position, direction, up, right, nearDist, Hnear, Wnear, farDist, Hfar, Wfar);
}

void Frustum::setup(Vertex3D position, Vertex3D direction, Vertex3D up, Vertex3D right, float nearDist, float Hnear, float Wnear, float farDist, float Hfar, float Wfar)
{
    this->position = position;

    this->direction = direction;
    this->up = up;
    this->right = right;

    this->nearDist = nearDist;
    this->farDist = farDist;

    this->Hnear = Hnear;
    this->Wnear = Wnear;

    this->Hfar = Hfar;
    this->Wfar = Wfar;
}

void Frustum::updateCenters() {
    // far center
    fc.x = this->position.x + direction.x * farDist;
    fc.y = this->position.y + direction.y * farDist;
    fc.z = this->position.z + direction.z * farDist;

    // near center
    nc.x = this->position.x + direction.x * nearDist;
    nc.y = this->position.y + direction.y * nearDist;
    nc.z = this->position.z + direction.z * nearDist;
}

void Frustum::updatePoints() {
    ntl.x = nc.x + (up.x * Hnear/2) - (right.x * Wnear/2);
    ntl.y = nc.y + (up.y * Hnear/2) - (right.y * Wnear/2);
    ntl.z = nc.z + (up.z * Hnear/2) - (right.z * Wnear/2);

    ntr.x = nc.x + (up.x * Hnear/2) + (right.x * Wnear/2);
    ntr.y = nc.y + (up.y * Hnear/2) + (right.y * Wnear/2);
    ntr.z = nc.z + (up.z * Hnear/2) + (right.z * Wnear/2);

    nbl.x = nc.x - (up.x * Hnear/2) - (right.x * Wnear/2);
    nbl.y = nc.y - (up.y * Hnear/2) - (right.y * Wnear/2);
    nbl.z = nc.z - (up.z * Hnear/2) - (right.z * Wnear/2);

    nbr.x = nc.x - (up.x * Hnear/2) + (right.x * Wnear/2);
    nbr.y = nc.y - (up.y * Hnear/2) + (right.y * Wnear/2);
    nbr.z = nc.z - (up.z * Hnear/2) + (right.z * Wnear/2);

    ftl.x = fc.x + (up.x * Hfar/2) - (right.x * Wfar/2);
    ftl.y = fc.y + (up.y * Hfar/2) - (right.y * Wfar/2);
    ftl.z = fc.z + (up.z * Hfar/2) - (right.z * Wfar/2);

    ftr.x = fc.x + (up.x * Hfar/2) + (right.x * Wfar/2);
    ftr.y = fc.y + (up.y * Hfar/2) + (right.y * Wfar/2);
    ftr.z = fc.z + (up.z * Hfar/2) + (right.z * Wfar/2);

    fbl.x = fc.x - (up.x * Hfar/2) - (right.x * Wfar/2);
    fbl.y = fc.y - (up.y * Hfar/2) - (right.y * Wfar/2);
    fbl.z = fc.z - (up.z * Hfar/2) - (right.z * Wfar/2);

    fbr.x = fc.x - (up.x * Hfar/2) + (right.x * Wfar/2);
    fbr.y = fc.y - (up.y * Hfar/2) + (right.y * Wfar/2);
    fbr.z = fc.z - (up.z * Hfar/2) + (right.z * Wfar/2);
}

// http://www.lighthouse3d.com/tutorials/view-frustum-culling/geometric-approach-extracting-the-planes/
void Frustum::updatePlanes() {

    // near/far plane
    planes[EngineSetup::getInstance()->NEAR_PLANE]   = Plane(ntl, ntr, nbl);   // near
    planes[EngineSetup::getInstance()->FAR_PLANE]    = Plane(ftr, ftl, fbl);   // far

    // view frustum
    planes[EngineSetup::getInstance()->LEFT_PLANE]   = Plane(position, fbl, ftl  );   // left
    planes[EngineSetup::getInstance()->RIGHT_PLANE]  = Plane(position, ftr, fbr );   // right
    planes[EngineSetup::getInstance()->TOP_PLANE]    = Plane(position, ftl, ftr );   // top
    planes[EngineSetup::getInstance()->BOTTOM_PLANE] = Plane(position, fbr, fbl );   // bottom

    // Near edges
    //top
    Vertex3D top1(ntl.x, ntl.y, ntl.z);
    Vertex3D top2(ntr.x, ntr.y, ntr.z);
    this->near_top = Vector3D(top1, top2);

    // bottom
    Vertex3D bottom1(nbl.x, nbl.y, nbl.z);
    Vertex3D bottom2(nbr.x, nbr.y, nbr.z);
    this->near_bottom = Vector3D(bottom1, bottom2);

    // left
    Vertex3D left1(ntl.x, ntl.y, ntl.z);
    Vertex3D left2(nbl.x, nbl.y, nbl.z);
    this->near_left = Vector3D(left1, left2);

    // right
    Vertex3D right1(ntr.x, ntr.y, ntr.z);
    Vertex3D right2(nbr.x, nbr.y, nbr.z);
    this->near_right = Vector3D(right1, right2);

    // Far edges
    // top
    Vertex3D top1f(ftl.x, ftl.y, ftl.z);
    Vertex3D top2f(ftr.x, ftr.y, ftr.z);
    this->far_top = Vector3D(top1f, top2f);

    // bottom
    Vertex3D bottom1f(fbl.x, fbl.y, fbl.z);
    Vertex3D bottom2f(fbr.x, fbr.y, fbr.z);
    this->far_bottom = Vector3D(bottom1f, bottom2f);

    // left
    Vertex3D left1f(ftl.x, ftl.y, ftl.z);
    Vertex3D left2f(fbl.x, fbl.y, fbl.z);
    this->far_left = Vector3D(left1f, left2f);

    // right
    Vertex3D right1f(ftr.x, ftr.y, ftr.z);
    Vertex3D right2f(fbr.x, fbr.y, fbr.z);
    this->far_right = Vector3D(right1f, right2f);
}

bool Frustum::isPointInFrustum(Vertex3D &v)
{
    EngineSetup *setup = EngineSetup::getInstance();

    for(int i = setup->FAR_PLANE; i <= setup->BOTTOM_PLANE; i++) {
        if (planes[i].distance(v) >= EngineSetup::getInstance()->FRUSTUM_CLIPPING_DISTANCE) {
            return false;
        }
    }

    return true;
}


void Frustum::consoleInfo() {

    printf("Frustum: ");
    this->position.consoleInfo("pos", false);
    this->nc.consoleInfo("fnc", false);

    printf("\r\n");
}

bool Frustum::isAABBInFrustum(AABB3D *aabb)
{
    if (
    !this->isPointInFrustum( aabb->vertices[0] ) &&
    !this->isPointInFrustum( aabb->vertices[1] ) &&
    !this->isPointInFrustum( aabb->vertices[2] ) &&
    !this->isPointInFrustum( aabb->vertices[3] ) &&
    !this->isPointInFrustum( aabb->vertices[4] ) &&
    !this->isPointInFrustum( aabb->vertices[5] ) &&
    !this->isPointInFrustum( aabb->vertices[6] ) &&
    !this->isPointInFrustum( aabb->vertices[7] )
    ) {
        return false;
    }

    return true;
}
