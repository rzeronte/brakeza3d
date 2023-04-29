
#ifndef SDL2_3D_ENGINE_TRIANGLE_H
#define SDL2_3D_ENGINE_TRIANGLE_H

#include "Vertex3D.h"
#include "Vector3D.h"
#include "../Components/Camera3D.h"
#include "Point2D.h"
#include "Object3D.h"
#include "../Render/Texture.h"
#include <list>
#include <vector>
#include "../Misc/Color.h"
struct OCPoint2D {
    OCPoint2D(int x, int y) : x(x), y(y) {}

    int x;
    int y;
};

struct OCVertex3D {
    OCVertex3D() {}

    OCVertex3D(float x, float y, float z, float u, float v) : x(x), y(y), z(z), u(u), v(v) {}
    OCVertex3D(float x, float y, float z) : x(x), y(y), z(z), u(0), v(0) {}

    float x = 0;
    float y = 0;
    float z = 0;
    float u = 0;
    float v;
};

struct OCTriangle
{
    OCTriangle(
        OCVertex3D A, OCVertex3D B, OCVertex3D C,
        OCVertex3D Ao, OCVertex3D Bo, OCVertex3D Co,
        OCVertex3D Ac, OCVertex3D Bc, OCVertex3D Cc,
        OCVertex3D An, OCVertex3D Bn, OCVertex3D Cn,
        OCPoint2D As, OCPoint2D Bs, OCPoint2D Cs,
        float fullArea, float reciprocalFullArea,
        OCVertex3D normal,
        float tex_u1_Ac_z, float tex_u2_Bc_z, float tex_u3_Cc_z,
        float tex_v1_Ac_z, float tex_v2_Bc_z, float tex_v3_Cc_z,
        float persp_correct_Az, float persp_correct_Bz, float persp_correct_Cz,
        int maxX, int minX, int maxY, int minY,
        bool clipped
    )
    :
    A(A), B(B), C(C),
    Ao(Ao), Bo(Bo), Co(Co),
    Ac(Ac), Bc(Bc), Cc(Cc),
    An(An), Bn(Bn), Cn(Cn),
    As(As), Bs(Bs), Cs(Cs),
    fullArea(fullArea), reciprocalFullArea(reciprocalFullArea),
    normal(normal),
    tex_u1_Ac_z(tex_u1_Ac_z), tex_u2_Bc_z(tex_u2_Bc_z), tex_u3_Cc_z(tex_u3_Cc_z),
    tex_v1_Ac_z(tex_v1_Ac_z), tex_v2_Bc_z(tex_v2_Bc_z), tex_v3_Cc_z(tex_v3_Cc_z),
    persp_correct_Az(persp_correct_Az), persp_correct_Bz(persp_correct_Bz), persp_correct_Cz(persp_correct_Cz),
    maxX(maxX), minX(minX), maxY(maxY), minY(minY),
    clipped(clipped)
    {}
    OCVertex3D A, B, C;
    OCVertex3D Ao, Bo, Co;
    OCVertex3D Ac, Bc, Cc;
    OCVertex3D An, Bn, Cn;
    OCPoint2D As, Bs, Cs;
    float fullArea, reciprocalFullArea = 0;
    OCVertex3D normal;
    float tex_u1_Ac_z = 0, tex_u2_Bc_z = 0, tex_u3_Cc_z = 0;
    float tex_v1_Ac_z = 0, tex_v2_Bc_z = 0, tex_v3_Cc_z = 0;
    float persp_correct_Az = 0, persp_correct_Bz = 0, persp_correct_Cz = 0;
    int maxX, minX = 0, maxY = 0, minY = 0;
    bool clipped = false;
};


class Triangle {

public:
    Vertex3D A, B, C;
    Vertex3D Ao, Bo, Co;
    Vertex3D Ac, Bc, Cc;
    Vertex3D An, Bn, Cn;
    Point2D As, Bs, Cs;

    float fullArea;
    float reciprocalFullArea;

    bool flatTextureColor;
    bool enableLights;

    Color flatColor;
    Object3D *parent;
    Vertex3D normal;
    Texture *texture;

    // texture vertex coordinates
    float tex_u1, tex_v1;
    float tex_u2, tex_v2;
    float tex_u3, tex_v3;

    // texture  pre-cached data
    float tex_u1_Ac_z, tex_u2_Bc_z, tex_u3_Cc_z;
    float tex_v1_Ac_z, tex_v2_Bc_z, tex_v3_Cc_z;
    float persp_correct_Az, persp_correct_Bz, persp_correct_Cz;

    // Screen coordinates bounds
    int maxX, minX, maxY, minY;

    bool clipped;

    Triangle();

    Triangle(Vertex3D A, Vertex3D B, Vertex3D C, Object3D *parent);

    void updateFullVertexSpaces(Frustum *frustum);

    void updateObjectSpace();

    void updateCameraSpace(Object3D *cam);

    void updatePerspectiveNDCSpace(Frustum *frustum);

    void updateOrthographicNDCSpace(Frustum *frustum);

    void updateScreenSpace();

    void updateUVCache();

    bool isBackFaceCulling(Vertex3D *position) const;

    void updateNormal();

    void processPixelTexture(Color &pixelColor, float tex_u, float tex_v, bool bilinear) const;

    void setTexture(Texture *texture);

    void clipping(Frustum *frustum, Plane *planes, int startPlaneIndex, int endPlaneIndex, Object3D *newTrianglesParent, std::vector<Triangle *> &triangles) const;

    void setClipped(bool);

    bool testForClipping(Plane *planes, int startPlaneIndex, int endPlaneIndex);

    void updateFullArea();

    void updateBoundingBox();

    void setFlatTextureColor(bool isFlatTextureColor);

    void setEnableLights(bool enableLights);

    void setFlatColor(const Color &flatColor);

    [[nodiscard]] bool isFlatTextureColor() const;

    [[nodiscard]] bool isPointInside(Vertex3D) const;

    [[nodiscard]] bool isClipped() const;

    [[nodiscard]] Texture *getTexture() const;

    [[nodiscard]] Vertex3D getCenterOfMass() const;

    [[nodiscard]] Vertex3D getNormal() const;

    [[nodiscard]] bool isEnableLights() const;
};

#endif //SDL2_3D_ENGINE_TRIANGLE_H
