#include "../../include/Physics/RayCollisionable.h"
#include "../../include/Render/Transforms.h"
#include "../../include/ComponentsManager.h"

RayCollisionable::RayCollisionable(const Vertex3D &ray) : ray(ray)
{
    Vertex3D start = getPosition();
    Vertex3D end = start + this->ray;

    rayCallback = new btCollisionWorld::ClosestRayResultCallback(
        btVector3(start.x, start.y, start.z),
        btVector3(end.x, end.y, end.z)
    );

    rayCallback->m_collisionFilterGroup = EngineSetup::collisionGroups::Player;
    rayCallback->m_collisionFilterMask = EngineSetup::collisionGroups::Enemy;
}

void RayCollisionable::integrate()
{
    Collisionable::integrate();

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

    //this->setRemoved(true);
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
        Drawable::drawVector3D(vector, ComponentsManager::get()->getComponentCamera()->getCamera(), Color::green());
    }
}

btCollisionWorld::ClosestRayResultCallback *RayCollisionable::getRayCallback() const {
    return rayCallback;
}

const Vertex3D &RayCollisionable::getRay() const {
    return ray;
}



