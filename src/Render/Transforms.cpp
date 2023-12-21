#include "../../include/Render/Transforms.h"
#include "../../include/Misc/Tools.h"
#include "../../include/EngineSetup.h"
#include "../../include/Render/M4.h"
#include "../../include/ComponentsManager.h"

void Transforms::objectSpace(Vertex3D &dst, Vertex3D &src, Object3D *o)
{
    dst = src;
    dst.setScaled(o->getScale());

    dst = o->getRotation() * dst;
    dst = dst + o->getPosition();

    dst = dst + o->getDrawOffset();

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

void Transforms::screenSpace(Point2D &P, Vertex3D &V)
{
    P.x = (int) ((1 + V.x) * EngineSetup::get()->screenWidthHalf);
    P.y = (int) ((1 + V.y) * EngineSetup::get()->screenHeightHalf);
}

Vertex3D Transforms::objectToLocal(Vertex3D &V, Object3D *o)
{
    Vertex3D T;

    T = V - o->getDrawOffset();

    T = T - o->getPosition();
    T = o->getRotation().getTranspose() * T;

    T.x /= o->getScale();
    T.y /= o->getScale();
    T.z /= o->getScale();

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
    // Mapear las coordenadas de (0, 0) a (100, 100) a la esquina superior izquierda e inferior derecha del plano cercano
    float mappedX = 2.0f * p.x / EngineSetup::get()->screenWidth - 1;
    float mappedY = 1.0f - 2.0f * p.y / EngineSetup::get()->screenHeight;

    // Obtener la matriz de proyección y la matriz de vista
    glm::mat4 projectionMatrix = Camera3D::getProjectionMatrix();
    glm::mat4 viewMatrix = cam->getViewMatrix();

    // Invertir la matriz de proyección y la matriz de vista
    glm::mat4 inverseProjectionMatrix = glm::inverse(projectionMatrix);
    glm::mat4 inverseViewMatrix = glm::inverse(viewMatrix);

    // Crear el vector de clip
    glm::vec4 clipCoords(mappedX, mappedY, -1.0f, 1.0f);

    // Aplicar la inversa de la matriz de proyección y la inversa de la matriz de vista
    glm::vec4 eyeCoords = inverseProjectionMatrix * clipCoords;
    eyeCoords.z = -1.0f;
    eyeCoords.w = 1.0f;

    glm::vec4 worldCoords = inverseViewMatrix * eyeCoords;

    // Normalizar las coordenadas w para obtener coordenadas del mundo
    worldCoords /= worldCoords.w;

    Vertex3D v(worldCoords.x, worldCoords.y, worldCoords.z);

    return v;
}

Point2D Transforms::WorldToPoint(Vertex3D v)
{
    glm::mat4 ViewMatrix = ComponentsManager::get()->getComponentCamera()->getCamera()->getViewMatrix();
    glm::mat4 ProjectionMatrix = Camera3D::getProjectionMatrix();

    glm::vec4 position1 = ProjectionMatrix * ViewMatrix * glm::vec4(v.x, v.y, v.z, 1.0);
    position1 /= position1.w;

    const auto windowWidth = EngineSetup::get()->screenWidth;
    const auto windowHeight = EngineSetup::get()->screenHeight;

    Point2D screenPoint((int)((position1.x + 1.0) * 0.5 * windowWidth), (int)((1.0 - position1.y) * 0.5 * windowHeight));
    return screenPoint;
}
