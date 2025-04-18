#include "../../include/Render/Transforms.h"
#include "../../include/Misc/Tools.h"
#include "../../include/ComponentsManager.h"

void Transforms::objectSpace(Vertex3D &dst, Vertex3D &src, Object3D *o)
{
    dst = src;
    dst.setScaled(o->getScale());

    dst = o->getRotation() * dst;
    dst = dst + o->getPosition();

    dst.u = src.u;
    dst.v = src.v;
}

Vertex3D Transforms::objectToLocal(Vertex3D &V, Object3D *o)
{
    Vertex3D T;

    T = V;

    T = T - o->getPosition();
    T = o->getRotation().getTranspose() * T;

    T.x /= o->getScale();
    T.y /= o->getScale();
    T.z /= o->getScale();

    T.u = V.u;
    T.v = V.v;

    return T;
}

Vertex3D Transforms::Point2DToWorld(Point2D &p)
{
    // Mapear las coordenadas de (0, 0) a (100, 100) a la esquina superior izquierda e inferior derecha del plano cercano
    float mappedX = 2.0f * p.x / EngineSetup::get()->screenWidth - 1;
    float mappedY = 1.0f - 2.0f * p.y / EngineSetup::get()->screenHeight;

    // Obtener la matriz de proyección y la matriz de vista
    auto camera = ComponentsManager::get()->getComponentCamera();
    glm::mat4 projectionMatrix = camera->getGLMMat4ProjectionMatrix();
    glm::mat4 viewMatrix = camera->getGLMMat4ViewMatrix();

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
    auto camera = ComponentsManager::get()->getComponentCamera();
    glm::mat4 ViewMatrix = camera->getGLMMat4ViewMatrix();
    glm::mat4 ProjectionMatrix = camera->getGLMMat4ProjectionMatrix();

    glm::vec4 position1 = ProjectionMatrix * ViewMatrix * glm::vec4(v.x, v.y, v.z, 1.0);
    position1 /= position1.w;

    const auto windowWidth = EngineSetup::get()->screenWidth;
    const auto windowHeight = EngineSetup::get()->screenHeight;

    Point2D screenPoint((int)((position1.x + 1.0) * 0.5 * windowWidth), (int)((1.0 - position1.y) * 0.5 * windowHeight));
    return screenPoint;
}
