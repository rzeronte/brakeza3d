#include "../../include/Render/Frustum.h"
#include "../../include/EngineSetup.h"
#include "../../include/Render/Maths.h"
#include "../../include/Render/Transforms.h"
#include "../../include/Render/Logging.h"

Frustum::Frustum()
{
};

void Frustum::setup(Vertex3D position, Vertex3D direction, Vertex3D up, Vertex3D right, float horizontalFOV, float aspectRatio, float farDistance) {
    // Establecemos posicion, el FOV horizontal, el FOV vertical va en función del ratio y la nearDistance
    this->setPosition( position);
    this->horizontal_fov = horizontalFOV;
    this->aspectRatio = aspectRatio;
    this->farDist = farDistance;

    this->direction = direction;
    this->up = up;
    this->right = right;

    this->nearDist = getNearDistance();

    this->Hnear = calcCanvasNearHeight();
    this->Wnear = calcCanvasNearWidth();

    this->Hfar = calcCanvasFarHeight();
    this->Wfar = calcCanvasFarWidth();
}
void Frustum::updateFrustum()
{
    updateCenters();
    updatePoints();
    updatePlanes();

    // Cacheamos los espacios de coordenadas de las 4 esquinas para reutilizarlos en la transformación NDC
    Transforms::cameraSpace(vNLs, near_left.vertex1, this);
    Transforms::cameraSpace(vNRs, near_right.vertex1, this);
    Transforms::cameraSpace(vNTs, near_top.vertex1, this);
    Transforms::cameraSpace(vNBs, near_bottom.vertex1, this);
}

void Frustum::updateCenters() {
    Vertex3D p = this->getPosition();

    // far center
    fc.x = p.x + direction.x * farDist;
    fc.y = p.y + direction.y * farDist;
    fc.z = p.z + direction.z * farDist;

    // near center
    nc.x = p.x + direction.x * nearDist;
    nc.y = p.y + direction.y * nearDist;
    nc.z = p.z + direction.z * nearDist;
}

void Frustum::updatePoints() {
    ntl.x = nc.x + (up.x * Hnear / 2) - (right.x * Wnear / 2);
    ntl.y = nc.y + (up.y * Hnear / 2) - (right.y * Wnear / 2);
    ntl.z = nc.z + (up.z * Hnear / 2) - (right.z * Wnear / 2);

    ntr.x = nc.x + (up.x * Hnear / 2) + (right.x * Wnear / 2);
    ntr.y = nc.y + (up.y * Hnear / 2) + (right.y * Wnear / 2);
    ntr.z = nc.z + (up.z * Hnear / 2) + (right.z * Wnear / 2);

    nbl.x = nc.x - (up.x * Hnear / 2) - (right.x * Wnear / 2);
    nbl.y = nc.y - (up.y * Hnear / 2) - (right.y * Wnear / 2);
    nbl.z = nc.z - (up.z * Hnear / 2) - (right.z * Wnear / 2);

    nbr.x = nc.x - (up.x * Hnear / 2) + (right.x * Wnear / 2);
    nbr.y = nc.y - (up.y * Hnear / 2) + (right.y * Wnear / 2);
    nbr.z = nc.z - (up.z * Hnear / 2) + (right.z * Wnear / 2);

    ftl.x = fc.x + (up.x * Hfar / 2) - (right.x * Wfar / 2);
    ftl.y = fc.y + (up.y * Hfar / 2) - (right.y * Wfar / 2);
    ftl.z = fc.z + (up.z * Hfar / 2) - (right.z * Wfar / 2);

    ftr.x = fc.x + (up.x * Hfar / 2) + (right.x * Wfar / 2);
    ftr.y = fc.y + (up.y * Hfar / 2) + (right.y * Wfar / 2);
    ftr.z = fc.z + (up.z * Hfar / 2) + (right.z * Wfar / 2);

    fbl.x = fc.x - (up.x * Hfar / 2) - (right.x * Wfar / 2);
    fbl.y = fc.y - (up.y * Hfar / 2) - (right.y * Wfar / 2);
    fbl.z = fc.z - (up.z * Hfar / 2) - (right.z * Wfar / 2);

    fbr.x = fc.x - (up.x * Hfar / 2) + (right.x * Wfar / 2);
    fbr.y = fc.y - (up.y * Hfar / 2) + (right.y * Wfar / 2);
    fbr.z = fc.z - (up.z * Hfar / 2) + (right.z * Wfar / 2);
}

// http://www.lighthouse3d.com/tutorials/view-frustum-culling/geometric-approach-extracting-the-planes/
void Frustum::updatePlanes() {
    // near/far plane
    planes[EngineSetup::get()->NEAR_PLANE] = Plane(ntl, ntr, nbl);   // near
    planes[EngineSetup::get()->FAR_PLANE]  = Plane(ftr, ftl, fbl);   // far

    // view frustum
    planes[EngineSetup::get()->LEFT_PLANE]   = Plane(getPosition(), fbl, ftl);   // left
    planes[EngineSetup::get()->RIGHT_PLANE]  = Plane(getPosition(), ftr, fbr);   // right
    planes[EngineSetup::get()->TOP_PLANE]    = Plane(getPosition(), ftl, ftr);   // top
    planes[EngineSetup::get()->BOTTOM_PLANE] = Plane(getPosition(), fbr, fbl);   // bottom

    // Near Plane Vertices
    Vertex3D topLn(ntl.x, ntl.y, ntl.z); //top left
    Vertex3D topRn(ntr.x, ntr.y, ntr.z); // top right
    Vertex3D bottomLn(nbl.x, nbl.y, nbl.z); // bottom left
    Vertex3D bottomRn(nbr.x, nbr.y, nbr.z); // bottom right
    Vertex3D leftTn(ntl.x, ntl.y, ntl.z); // left top
    Vertex3D leftBn(nbl.x, nbl.y, nbl.z); // left bottom
    Vertex3D rightTn(ntr.x, ntr.y, ntr.z); // right top
    Vertex3D rightBn(nbr.x, nbr.y, nbr.z); // right bottom

    // Near Planes Vectors
    this->near_top = Vector3D(topLn, topRn);
    this->near_bottom = Vector3D(bottomLn, bottomRn);
    this->near_left = Vector3D(leftTn, leftBn);
    this->near_right = Vector3D(rightTn, rightBn);

    // Far Plane Vertices
    Vertex3D topLf(ftl.x, ftl.y, ftl.z); // top left
    Vertex3D topRf(ftr.x, ftr.y, ftr.z); // top right
    Vertex3D bottomLf(fbl.x, fbl.y, fbl.z); // bottom left
    Vertex3D bottomRf(fbr.x, fbr.y, fbr.z); // bottom right
    Vertex3D leftTf(ftl.x, ftl.y, ftl.z); // left top
    Vertex3D leftBf(fbl.x, fbl.y, fbl.z); // left bottom
    Vertex3D rightTf(ftr.x, ftr.y, ftr.z); // right top
    Vertex3D rightBf(fbr.x, fbr.y, fbr.z); // right bottom

    // Far Planes Vectors
    this->far_top = Vector3D(topLf, topRf);
    this->far_bottom = Vector3D(bottomLf, bottomRf);
    this->far_left = Vector3D(leftTf, leftBf);
    this->far_right = Vector3D(rightTf, rightBf);
}

bool Frustum::isVertexInside(Vertex3D &v) {
    EngineSetup *setup = EngineSetup::get();

    for (int i = setup->FAR_PLANE; i <= setup->BOTTOM_PLANE; i++) {
        if (planes[i].distance(v) >= EngineSetup::get()->FRUSTUM_CLIPPING_DISTANCE) {
            return false;
        }
    }

    return true;
}


void Frustum::consoleInfo() const {

    //Logging::get()->Log("Frustum for " + parent->getLabel());
    this->nc.consoleInfo("fnc", false);
    printf("\r\n");
    Logging::Log("Aspect ratio:" + std::to_string(aspectRatio), "CAMERA");
    Logging::Log("Horizontal FOV:" + std::to_string(horizontal_fov), "CAMERA");
    Logging::Log("Vertical FOV:" + std::to_string(getVerticalFOV()), "CAMERA");
    Logging::Log("Near distance:" + std::to_string(getNearDistance()), "CAMERA");
    Logging::Log("Canvas width:" + std::to_string(calcCanvasNearWidth()), "CAMERA");
    Logging::Log("Canvas height:" + std::to_string(calcCanvasNearHeight()), "CAMERA");
}

bool Frustum::isAABBInFrustum(AABB3D *aabb) {
    for(auto & vertice : aabb->vertices) {
        if (!this->isVertexInside(vertice)) {
            return false;
        }
    }
    return true;
}

float Frustum::getNearDistance() const {
    return (1 / tanf(Maths::degreesToRadians(this->horizontal_fov / 2)));
}

float Frustum::getVerticalFOV() const {
    float vfov = 2 * atanf(getScreenAspectRatio() / getNearDistance());

    return Maths::radiansToDegrees(vfov);
}

float Frustum::calcCanvasNearWidth() const {
    float width = (2 * tanf(Maths::degreesToRadians(horizontal_fov / 2)) * getNearDistance());

    return width;
}

float Frustum::calcCanvasNearHeight() const {
    float height = (2 * tanf(Maths::degreesToRadians(getVerticalFOV() / 2)) * getNearDistance()) * getScreenAspectRatio();

    return height;
}

float Frustum::calcCanvasFarWidth() const {
    float width = (2 * tanf(Maths::degreesToRadians(horizontal_fov / 2)) * farDist);

    return width;
}

float Frustum::calcCanvasFarHeight() const {
    float height = (2 * tanf(Maths::degreesToRadians(getVerticalFOV() / 2)) * farDist) * getScreenAspectRatio();

    return height;
}

float Frustum::getScreenAspectRatio() const {
    return this->aspectRatio;
}

