
#include <SDL2/SDL_surface.h>
#include <list>
#include "../../include/Objects/Triangle3D.h"
#include "../../include/Misc/Tools.h"
#include "../../include/Render/Transforms.h"
#include "../../include/Render/Drawable.h"
#include "../../include/Render/Maths.h"
#include "../../include/ComponentsManager.h"
#include "../../include/Brakeza3D.h"
#include "../../include/Objects/Decal.h"

Triangle::Triangle()
:
    fullArea(0),
    reciprocalFullArea(0),
    flatTextureColor(false),
    enableLights(false),
    parent(nullptr), texture(nullptr),
    tex_u1(0), tex_v1(0),
    tex_u2(0), tex_v2(0),
    tex_u3(0), tex_v3(0), tex_u1_Ac_z(0),
    tex_u2_Bc_z(0), tex_u3_Cc_z(0), tex_v1_Ac_z(0),
    tex_v2_Bc_z(0), tex_v3_Cc_z(0), persp_correct_Az(0),
    persp_correct_Bz(0), persp_correct_Cz(0), maxX(0), minX(0),
    maxY(0),
    minY(0),
    clipped(false)
{
}

Triangle::Triangle(Vertex3D A, Vertex3D B, Vertex3D C, Object3D *parent)
:
    flatTextureColor(false),
    parent(nullptr),
    texture(nullptr),
    clipped(false)
{
    this->A = A;
    this->B = B;
    this->C = C;

    this->parent = parent;
    texture = nullptr;
}

void Triangle::updateObjectSpace()
{
    Transforms::objectSpace(Ao, A, parent);
    Transforms::objectSpace(Bo, B, parent);
    Transforms::objectSpace(Co, C, parent);
}

void Triangle::updateCameraSpace(Object3D *cam) {
    Transforms::cameraSpace(Ac, Ao, cam);
    Transforms::cameraSpace(Bc, Bo, cam);
    Transforms::cameraSpace(Cc, Co, cam);
}

void Triangle::updatePerspectiveNDCSpace(Frustum *frustum) {
    An = Transforms::PerspectiveNDCSpace(Ac, frustum);
    Bn = Transforms::PerspectiveNDCSpace(Bc, frustum);
    Cn = Transforms::PerspectiveNDCSpace(Cc, frustum);
}

void Triangle::updateOrthographicNDCSpace(Frustum *frustum)
{
    An = Transforms::OrthographicNDCSpace(Ac, frustum);
    Bn = Transforms::OrthographicNDCSpace(Bc, frustum);
    Cn = Transforms::OrthographicNDCSpace(Cc, frustum);
}

void Triangle::updateScreenSpace() {
    Transforms::screenSpace(As, An);
    Transforms::screenSpace(Bs, Bn);
    Transforms::screenSpace(Cs, Cn);
}

void Triangle::updateFullVertexSpaces(Frustum *frustum) {
    this->updateObjectSpace();
    this->updateCameraSpace(frustum);
    this->updatePerspectiveNDCSpace(frustum);
    this->updateScreenSpace();
    this->updateNormal();
}

void Triangle::updateUVCache()
{
    if (this->getTexture() == nullptr) return;

    tex_u1 = A.u;
    tex_v1 = A.v;

    tex_u2 = B.u;
    tex_v2 = B.v;

    tex_u3 = C.u;
    tex_v3 = C.v;

    if (parent->isDecal()) {
        auto *decalParent = dynamic_cast<Decal *> (parent);

        float decal_As = decalParent->getSCoord(Ao);
        float decal_At = decalParent->getTCoord(Ao);
        float decal_Bs = decalParent->getSCoord(Bo);
        float decal_Bt = decalParent->getTCoord(Bo);
        float decal_Cs = decalParent->getSCoord(Co);
        float decal_Ct = decalParent->getTCoord(Co);

        this->texture = decalParent->getSprite()->getCurrentTextureAnimation()->getCurrentFrame();

        tex_u1 = decal_As;
        tex_v1 = decal_At;

        tex_u2 = decal_Bs;
        tex_v2 = decal_Bt;

        tex_u3 = decal_Cs;
        tex_v3 = decal_Ct;
    }

    tex_u1_Ac_z = tex_u1 / Ac.z;
    tex_u2_Bc_z = tex_u2 / Bc.z;
    tex_u3_Cc_z = tex_u3 / Cc.z;

    tex_v1_Ac_z = tex_v1 / Ac.z;
    tex_v2_Bc_z = tex_v2 / Bc.z;
    tex_v3_Cc_z = tex_v3 / Cc.z;

    persp_correct_Az = 1 / Ac.z;
    persp_correct_Bz = 1 / Bc.z;
    persp_correct_Cz = 1 / Cc.z;
}

void Triangle::updateNormal()
{
    this->normal = Vector3D(this->Ao, this->Bo).getComponent() % Vector3D(this->Ao, this->Co).getComponent();
}

Vertex3D Triangle::getNormal() const
{
    return this->normal;
}

void Triangle::clipping(
    Frustum *frustum,
    Plane *planes,
    int startPlaneIndex,
    int endPlaneIndex,
    Object3D *newTrianglesParent,
   std::vector<Triangle *> &triangles
) const {
    Vertex3D output_vertices[10];
    int num_outvertices = 0;
    Vertex3D input_vertices[10];
    int num_inputvertices = 0;

    input_vertices[0] = this->Ao;
    num_inputvertices++;
    input_vertices[1] = this->Bo;
    num_inputvertices++;
    input_vertices[2] = this->Co;
    num_inputvertices++;

    // clip against planes
    for (int i = startPlaneIndex; i <= endPlaneIndex; i++) {
        Maths::ClippingPolygon(input_vertices, num_inputvertices, output_vertices, num_outvertices, i, planes);

        memcpy(&input_vertices, &output_vertices, sizeof(output_vertices));
        /*for (int j = 0; j < num_outvertices; j++) { input_vertices[j] = output_vertices[j]; }*/

        num_inputvertices = num_outvertices;
        num_outvertices = 0;
    }

    if (num_inputvertices != 0) {
        int oldNumTriangles = (int) triangles.size();

        Maths::TriangulatePolygon(
            num_inputvertices,
            input_vertices,
            this->getNormal(),
            triangles,
            newTrianglesParent,
            this->getTexture(),
            true,
            this->isFlatTextureColor(),
            this->flatColor,
            this->isEnableLights()
        );

        // update cache for clipped triangles (they are out from hide removal surface updating)
        for (int i = oldNumTriangles; i < (int) triangles.size(); i++) {
            triangles[i]->updateFullVertexSpaces(frustum);
            triangles[i]->updateUVCache();
            triangles[i]->updateBoundingBox();
            triangles[i]->updateFullArea();
        }
    }
}

void Triangle::updateBoundingBox()
{
    maxX = std::max(As.x, std::max(Bs.x, Cs.x));
    minX = std::min(As.x, std::min(Bs.x, Cs.x));
    maxY = std::max(As.y, std::max(Bs.y, Cs.y));
    minY = std::min(As.y, std::min(Bs.y, Cs.y));
}

void Triangle::updateFullArea()
{
    this->fullArea = (float) Maths::orient2d(Bs, Cs, Point2D((int) As.x, (int) As.y));
    this->reciprocalFullArea = 1 / this->fullArea;
}

// (v0 - P) . N
bool Triangle::isBackFaceCulling(Vertex3D *position) const
{
    // Camera-triangle vector
    Vertex3D v = this->Ao - *position;

    return (v * this->getNormal()) >= 0;
}

Vertex3D Triangle::getCenterOfMass() const
{
    return Vertex3D(
        (Ao.x + Bo.x + Co.x) / 3,
        (Ao.y + Bo.y + Co.y) / 3,
        (Ao.z + Bo.z + Co.z) / 3
    );
}

void Triangle::processPixelTexture(Color &pixelColor, float tex_u, float tex_v, bool bilinear) const
{
    float ignorablePartInt;

    // Check for inversion U
    if (!std::signbit(tex_u)) {
        tex_u = modf(tex_u, &ignorablePartInt);
    } else {
        tex_u = 1 - modf(abs(tex_u), &ignorablePartInt);
    }

    // Check for inversion V
    if (!std::signbit(tex_v)) {
        tex_v = modf(tex_v, &ignorablePartInt);
    } else {
        tex_v = 1 - modf(abs(tex_v), &ignorablePartInt);
    }

    if (bilinear) {
        pixelColor = Tools::readSurfacePixelFromBilinearUV(getTexture()->getSurface(), tex_u, tex_v);
        return;
    }

    pixelColor = Tools::readSurfacePixelFromUV(getTexture()->getSurface(), tex_u, tex_v);
}

Image *Triangle::getTexture() const {
    return texture;
}

void Triangle::setTexture(Image *t) {
    texture = t;
}

void Triangle::setClipped(bool value) {
    this->clipped = value;
}

bool Triangle::isClipped() const {
    return this->clipped;
}

bool Triangle::isPointInside(Vertex3D v) const {
    return Maths::PointInTriangle(v, Ao, Bo, Co);
}

bool Triangle::testForClipping(Plane *planes, int startPlaneIndex, int endPlaneIndex)
{
    // clip against planes
    Vector3D edges[3];
    edges[0] = Vector3D(Ao, Bo);
    edges[1] = Vector3D(Bo, Co);
    edges[2] = Vector3D(Co, Ao);

    for (int i = startPlaneIndex; i <= endPlaneIndex; i++) {
        for (auto & edge : edges) {
            if (Maths::isVector3DClippingPlane(planes[i], edge) > 1) {
                return true;
            }
        }
    }

    return false;
}

bool Triangle::isFlatTextureColor() const {
    return flatTextureColor;
}

void Triangle::setFlatTextureColor(bool isFlatTextureColor) {
    this->flatTextureColor = isFlatTextureColor;
}

bool Triangle::isEnableLights() const {
    return this->enableLights;
}

void Triangle::setEnableLights(bool enableLights) {
    this->enableLights = enableLights;
}

void Triangle::setFlatColor(const Color &flatColor) {
    Triangle::flatColor = flatColor;
}

