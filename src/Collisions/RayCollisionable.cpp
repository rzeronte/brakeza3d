#include "../../include/Physics/RayCollisionable.h"
#include "../../include/Render/Transforms.h"
#include "../../include/ComponentsManager.h"

RayCollisionable::RayCollisionable(const Vertex3D &ray, int filterGroup, int filterMask) : ray(ray), filterGroup(filterGroup), filterMask(filterMask)
{
    Vertex3D start = getPosition();
}

void RayCollisionable::integrate()
{
    Collisionable::integrate();

    Vertex3D start = getPosition();
    Vertex3D end = start + this->ray;

    auto *rayCallback = new btCollisionWorld::ClosestRayResultCallback(
        btVector3(start.x, start.y, start.z),
        btVector3(end.x, end.y, end.z)
    );

    rayCallback->m_collisionFilterGroup = filterGroup;
    rayCallback->m_collisionFilterMask = filterMask;

    ComponentsManager::get()->getComponentCollisions()->getDynamicsWorld()->rayTest(
        btVector3(start.x, start.y, start.z),
        btVector3(end.x, end.y, end.z),
        *rayCallback
    );

    hasHit(rayCallback);
}

void RayCollisionable::hasHit(btCollisionWorld::ClosestRayResultCallback *rayCallback)
{
    if (rayCallback->hasHit()) {
        auto *collisionable = (Collisionable *) rayCallback->m_collisionObject->getUserPointer();

        btVector3 rayHitPosition = rayCallback->m_hitPointWorld;

        collisionable->resolveCollision(this);

        setHitPosition(Vertex3D(rayHitPosition.x(), rayHitPosition.y(), rayHitPosition.z()));

        this->resolveCollision(collisionable);
    }
}

void RayCollisionable::resolveCollision(Collisionable *collisionable)
{
    Logging::Log("Collision with ray!", "");
}

const Vertex3D &RayCollisionable::getHitPosition() const {
    return hitPosition;
}

void RayCollisionable::setHitPosition(const Vertex3D &value) {
    RayCollisionable::hitPosition = value;
}

void RayCollisionable::onUpdate()
{
    Object3D::onUpdate();

    if (EngineSetup::get()->BULLET_DEBUG_MODE) {
        auto end = getPosition() + ray;
        auto vector = Vector3D(getPosition(), end);
        Drawable::drawVector3D(vector, ComponentsManager::get()->getComponentCamera()->getCamera(), Color::red());
    }
}

const Vertex3D &RayCollisionable::getRay() const {
    return ray;
}

void RayCollisionable::setRay(const Vertex3D &ray) {
    RayCollisionable::ray = ray;
}



