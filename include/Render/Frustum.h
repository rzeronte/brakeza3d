
#ifndef SDL2_3D_ENGINE_FRUSTUM_H
#define SDL2_3D_ENGINE_FRUSTUM_H

#include "../Objects/Vertex3D.h"
#include "../Objects/Object3D.h"
#include "../Objects/Vector3D.h"
#include "Plane.h"
#include "AABB3D.h"

// http://www.lighthouse3d.com/tutorials/view-frustum-culling/geometric-approach-extracting-the-planes/
class Frustum : public Object3D {
public:
    Vertex3D direction;   // Normalized
    Vertex3D up;
    Vertex3D right;

    float horizontal_fov;
    float aspectRatio;

    float nearDist;     // the distance from the camera to the near plane
    float Hnear;        // the “height” of the near plane
    float Wnear;        // the “width” of the near plane
    float farDist;      // the distance from the camera to the far plane
    float Hfar;         // the “height” of the far plane
    float Wfar;         // the “width” of the far plane

    // far plane points
    Vertex3D fc;   // center
    Vertex3D ftl;  // top left
    Vertex3D ftr;  // top right
    Vertex3D fbl;  // bottom left
    Vertex3D fbr;  // bottom right

    // near plane points
    Vertex3D nc;   // center
    Vertex3D ntl;  // top left
    Vertex3D ntr;  // top right
    Vertex3D nbl;  // bottom left
    Vertex3D nbr;  // bottom right

    Plane planes[6];

    // Near Plane Edges
    Vector3D near_top;
    Vector3D near_bottom;
    Vector3D near_left;
    Vector3D near_right;

    // Near Left / Right / Top / Bottom in CameraSpace
    Vertex3D vNLs, vNRs;
    Vertex3D vNTs, vNBs;

    // Far Plane Edges
    Vector3D far_top = Vector3D();
    Vector3D far_bottom = Vector3D();
    Vector3D far_left = Vector3D();
    Vector3D far_right = Vector3D();

    AABB3D bounds;

public:
    Frustum();

    void setup(Vertex3D position, Vertex3D direction, Vertex3D up, Vertex3D right, float horizontalFOV, float aspectRatio, float farDistance);

    float calcCanvasNearWidth() const;

    float calcCanvasNearHeight() const;

    float getScreenAspectRatio() const;

    float calcCanvasFarWidth() const;

    float calcCanvasFarHeight() const;

    float getNearDistance() const;

    float getVerticalFOV() const;

    void updateFrustum();

    void updateCenters();

    void updatePlanes();

    void updatePoints();

    bool isVertexInside(Vertex3D &v);

    bool isAABBInFrustum(AABB3D *aabb);

    void consoleInfo() const;

    void fillDeepMapping();
};


#endif //SDL2_3D_ENGINE_FRUSTUM_H
