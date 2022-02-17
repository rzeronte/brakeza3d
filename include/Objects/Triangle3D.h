
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
#include "../OpenCL/OCLTriangle.h"
#include "../Misc/Color.h"

class Triangle {

public:
    int id;
    bool drawed = false;

    Vertex3D A, B, C;
    Vertex3D Ao, Bo, Co;
    Vertex3D Ac, Bc, Cc;
    Vertex3D An, Bn, Cn;
    Point2D As, Bs, Cs;

    Vertex3D Anormal, Bnormal, Cnormal;

    float fullArea;
    float reciprocalFullArea;

    bool flatTextureColor;
    bool enableLights;

    Texture *texture;
    Texture *lightmap;

    unsigned char typelight[4];

    // lightmap coordinates
    float light_u1, light_v1;
    float light_u2, light_v2;
    float light_u3, light_v3;

    // texture vertex coordinates
    float tex_u1, tex_v1;
    float tex_u2, tex_v2;
    float tex_u3, tex_v3;

    // texture & lightmaps pre-cached data
    float light_u1_Ac_z, light_u2_Bc_z, light_u3_Cc_z;
    float light_v1_Ac_z, light_v2_Bc_z, light_v3_Cc_z;
    float tex_u1_Ac_z, tex_u2_Bc_z, tex_u3_Cc_z;
    float tex_v1_Ac_z, tex_v2_Bc_z, tex_v3_Cc_z;
    float persp_correct_Az, persp_correct_Bz, persp_correct_Cz;

    // Screen coordinates bounds
    int maxX, minX, maxY, minY;

    Object3D *parent;

    Vertex3D normal;

    int order = 0;
    bool clipped = false;
    bool clipped_cl = false;

    bool isBSP = false;
    int surfaceBSPIndex = -1;

    int lod;

    int currentBSPTextureAnimatedFrame = 0;
    float timerTextureAnimatedFrameControl = 0;

    // Control lightmaps animations
    float lightmapIndexPattern = 0;
    float lightmapIndexPattern2 = 0;
    float lightmapIndexPattern3 = 0;
    float lightmapIndexPattern4 = 0;

    Triangle();

    Triangle(Vertex3D A, Vertex3D B, Vertex3D C, Object3D *parent);

    void updateFullVertexSpaces(Frustum *frustum);

    void updateObjectSpace();

    void updateCameraSpace(Object3D *cam);

    void updatePerspectiveNDCSpace(Frustum *frustum);
    void updateOrthographicNDCSpace(Frustum *frustum);

    void updateScreenSpace();

    void updateUVCache();

    void getLightmapCoordinatesFromUV(float &lu, float &lv, float tex_u, float tex_v) const;

    bool isBackFaceCulling(Vertex3D *position) const;

    Vertex3D getNormal() const;

    void updateNormal();

    Vertex3D getCenterOfMass() const;

    void drawNormal(Camera3D *cam, Color color) const;


    void processPixelTexture(Color &pixelColor, float tex_u, float tex_v, bool bilinear) const;

    void processPixelLightmap(Color &pixelColor, float light_u, float light_v, const Uint8 &red, const Uint8 &green, const Uint8 &blue, const Uint8 &alpha);

    Texture *getTexture() const;

    void setTexture(Texture *texture);

    Texture *getLightmap() const;

    void setLightmap(Texture *texture);

    void clipping(Frustum *frustum, Plane *planes, int startPlaneIndex, int endPlaneIndex, Object3D *newTrianglesParent,
                  std::vector<Triangle *> &triangles, bool isBSP);

    void setClipped(bool);

    bool isClipped() const;

    bool testForClipping(Plane *planes, int startPlaneIndex, int endPlaneIndex);

    bool isPointInside(Vertex3D) const;

    int processLOD() const;

    void updateFullArea();

    void updateBoundingBox();

    void updateLightmapFrame();

    int getId() const;

    void setId(unsigned int id);

    Plane plane() const;

    bool isFlatTextureColor() const;
    void setFlatTextureColor(bool isFlatTextureColor);

    bool isEnableLights() const;
    void setEnableLights(bool enableLights);
};

#endif //SDL2_3D_ENGINE_TRIANGLE_H
