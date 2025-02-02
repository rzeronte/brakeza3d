#include "../../include/Render/PhysicsDebugDraw.h"
#include "../../include/Render/Drawable.h"
#include "../../include/ComponentsManager.h"

PhysicsDebugDraw::PhysicsDebugDraw() = default;

void PhysicsDebugDraw::drawLine(const btVector3 &from, const btVector3 &to, const btVector3 &color)
{
    auto window = ComponentsManager::get()->getComponentWindow();

    window->getShaderOGLLine3D()->render(
        Vertex3D::fromBullet(from),
        Vertex3D::fromBullet(to),
        window->getForegroundFramebuffer(),
        Color::fuchsia()
    );
}

void PhysicsDebugDraw::drawContactPoint(
    const btVector3 &PointOnB,
    const btVector3 &normalOnB,
    btScalar distance,
    int lifeTime,
    const btVector3 &color)
{
}

void PhysicsDebugDraw::reportErrorWarning(const char *warningString)
{
}

void PhysicsDebugDraw::draw3dText(const btVector3 &location, const char *textString)
{
}

void PhysicsDebugDraw::setDebugMode(int debugMode)
{
}

int PhysicsDebugDraw::getDebugMode() const
{
    return btIDebugDraw::DBG_DrawWireframe;
}
