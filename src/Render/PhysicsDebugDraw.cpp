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
    std::lock_guard<std::mutex> lock(mtx);

    auto p = Vertex3D::fromBullet(PointOnB);
    constexpr float s = 0.3f;

    // 3-axis cross at contact point
    Vertex3D xn(p.x - s, p.y, p.z), xp(p.x + s, p.y, p.z);
    Vertex3D yn(p.x, p.y - s, p.z), yp(p.x, p.y + s, p.z);
    Vertex3D zn(p.x, p.y, p.z - s), zp(p.x, p.y, p.z + s);
    Components::get()->Collisions()->AddVector3DIntoCache(Vector3D(xn, xp));
    Components::get()->Collisions()->AddVector3DIntoCache(Vector3D(yn, yp));
    Components::get()->Collisions()->AddVector3DIntoCache(Vector3D(zn, zp));

    // Normal direction line (2 units long)
    auto n = Vertex3D::fromBullet(normalOnB);
    Vertex3D np(p.x + n.x * 2.0f, p.y + n.y * 2.0f, p.z + n.z * 2.0f);
    Components::get()->Collisions()->AddVector3DIntoCache(Vector3D(p, np));
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
