
#ifndef SDL2_3D_ENGINE_FRUSTUM_H
#define SDL2_3D_ENGINE_FRUSTUM_H

#include "Vertex.h"
#include "Plane.h"
#include "Object3D.h"
#include "Vector3D.h"

// http://www.lighthouse3d.com/tutorials/view-frustum-culling/geometric-approach-extracting-the-planes/
class Frustum {
public:

    Vertex position;

    Vertex direction;   // Normalized
    Vertex up;
    Vertex right;

    float nearDist;     // the distance from the camera to the near plane
    float Hnear;        // the “height” of the near plane
    float Wnear;        // the “width” of the near plane
    float farDist;      // the distance from the camera to the far plane
    float Hfar;         // the “height” of the far plane
    float Wfar;         // the “width” of the far plane


    // far plane points
    Vertex fc;   // center
    Vertex ftl;  // top left
    Vertex ftr;  // top right
    Vertex fbl;  // bottom left
    Vertex fbr;  // bottom right

    // near plane points
    Vertex nc;   // center
    Vertex ntl;  // top left
    Vertex ntr;  // top right
    Vertex nbl;  // bottom left
    Vertex nbr;  // bottom right

    Plane planes[6];

    // Near Plane Edges
    Vector3D near_top = Vector3D(Vertex(0, 0, 0), Vertex(0, 0, 0));
    Vector3D near_bottom = Vector3D(Vertex(0, 0, 0), Vertex(0, 0, 0));
    Vector3D near_left = Vector3D(Vertex(0, 0, 0), Vertex(0, 0, 0));
    Vector3D near_right = Vector3D(Vertex(0, 0, 0), Vertex(0, 0, 0));

    // Near Plane Edges
    Vector3D far_top = Vector3D(Vertex(0, 0, 0), Vertex(0, 0, 0));
    Vector3D far_bottom = Vector3D(Vertex(0, 0, 0), Vertex(0, 0, 0));
    Vector3D far_left = Vector3D(Vertex(0, 0, 0), Vertex(0, 0, 0));
    Vector3D far_right = Vector3D(Vertex(0, 0, 0), Vertex(0, 0, 0));

public:
    Frustum();
    Frustum(Vertex position, Vertex direction, Vertex up, Vertex right, float nearDist, float Hnear, float Wnear, float farDist, float Hfar, float Wfar);
    void setup(Vertex position, Vertex direction, Vertex up, Vertex right, float nearDist, float Hnear, float Wnear, float farDist, float Hfar, float Wfar);

    void updateCenters();
    void updatePlanes();
    void updatePoints();

    bool isPointInFrustum(Vertex v);
    void consoleInfo();
};


#endif //SDL2_3D_ENGINE_FRUSTUM_H
