#include "../../include/Render/PhysicsDebugDraw.h"
#include "../../include/Render/Drawable.h"
#include "../../include/Components/Components.h"

PhysicsDebugDraw::PhysicsDebugDraw() = default;

void PhysicsDebugDraw::drawLine(const btVector3 &from, const btVector3 &to, const btVector3 &color)
{
    std::lock_guard<std::mutex> lock(mtx);

    auto vf = Vertex3D::fromBullet(from);
    auto vt = Vertex3D::fromBullet(to);
    Components::get()->Collisions()->AddVector3DIntoCache(Vector3D(vf, vt));
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
    m_debugMode = debugMode;

}

int PhysicsDebugDraw::getDebugMode() const
{
    return m_debugMode;
}
