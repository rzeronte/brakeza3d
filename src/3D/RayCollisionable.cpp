#include "../../include/3D/RayCollisionable.h"
#include "../../include/Render/Transforms.h"
#include "../../include/Components/ComponentsManager.h"
#include "../../include/Misc/Logging.h"
#include "../../include/Render/Drawable.h"

RayCollisionable::RayCollisionable(const Vertex3D &ray, int filterGroup, int filterMask)
:
    ray(ray)
{
    Vertex3D start = getPosition();
    Vertex3D end = start + this->ray;

    rayCallback = new btCollisionWorld::ClosestRayResultCallback(start.toBullet(), end.toBullet());

    rayCallback->m_collisionFilterGroup = filterGroup;
    rayCallback->m_collisionFilterMask = filterMask;
    rayCallback->m_closestHitFraction = 1.1;
}

void RayCollisionable::Integrate()
{
    Vertex3D start = getPosition();
    Vertex3D end = start + this->ray;

    rayCallback->m_rayFromWorld = btVector3(start.x, start.y, start.z);
    rayCallback->m_rayToWorld = btVector3(end.x, end.y, end.z);

    ComponentsManager::get()->getComponentCollisions()->getDynamicsWorld()->rayTest(
        btVector3(start.x, start.y, start.z),
        btVector3(end.x, end.y, end.z),
        *rayCallback
    );

    hasHit();
}

void RayCollisionable::hasHit()
{
    /*if (rayCallback->hasHit()) {
        auto *objectWithCollision = (Collider *) rayCallback->m_collisionObject->getUserPointer();

        btVector3 rayHitPosition = rayCallback->m_hitPointWorld;

        objectWithCollision->resolveCollision(this);

        setHitPosition(Vertex3D(rayHitPosition.x(), rayHitPosition.y(), rayHitPosition.z()));

        this->resolveCollision(objectWithCollision);
    }*/

    rayCallback->m_closestHitFraction = static_cast<btScalar>(1.),
    rayCallback->m_collisionObject = nullptr;
}

void RayCollisionable::ResolveCollision(CollisionInfo objectWithCollision)
{
    Logging::Message("Collision with ray!");
}

const Vertex3D &RayCollisionable::getHitPosition() const
{
    return hitPosition;
}

void RayCollisionable::setHitPosition(const Vertex3D &value)
{
    hitPosition = value;
}

void RayCollisionable::onUpdate()
{
    Object3D::onUpdate();

    if (!isEnabled()) return;

    if (BrakezaSetup::get()->BULLET_DEBUG_MODE) {
        auto end = getPosition() + ray;
        auto vector = Vector3D(getPosition(), end);
        Drawable::drawVector3D(vector, Color::yellow());
    }
}

const Vertex3D &RayCollisionable::getRay() const
{
    return ray;
}

void RayCollisionable::setRay(const Vertex3D &ray)
{
    RayCollisionable::ray = ray;
}

void RayCollisionable::setTarget(Object3D *target)
{
    RayCollisionable::target = target;
    if (target != nullptr) {
        hadTarget = true;
    }
}

Object3D *RayCollisionable::getTarget() const {
    return target;
}

bool RayCollisionable::isHadTarget() const {
    return hadTarget;
}
