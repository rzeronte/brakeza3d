
#ifndef SDL2_3D_ENGINE_TRIANGLE_H
#define SDL2_3D_ENGINE_TRIANGLE_H

#include "Vertex3D.h"
#include "Vector3D.h"
#include "../Components/Camera3D.h"
#include "Point2D.h"
#include "Object3D.h"
#include <list>
#include <vector>
#include "../Misc/Color.h"
#include "../Misc/Image.h"


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
    Image *texture;

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

    void setTexture(Image *texture);

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

    [[nodiscard]] Image *getTexture() const;

    [[nodiscard]] Vertex3D getCenterOfMass() const;

    [[nodiscard]] Vertex3D getNormal() const;

    [[nodiscard]] bool isEnableLights() const;
};

#endif //SDL2_3D_ENGINE_TRIANGLE_H
