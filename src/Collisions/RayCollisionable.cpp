#include "../../include/Physics/RayCollisionable.h"
#include "../../include/Render/Transforms.h"
#include "../../include/ComponentsManager.h"

RayCollisionable::RayCollisionable(const Vertex3D &ray) : ray(ray)
{

}

void RayCollisionable::integrate()
{
    Collisionable::integrate();

    Vertex3D start = getPosition();
    Vertex3D end = start + this->ray;

    btCollisionWorld::ClosestRayResultCallback rayCallback(
        btVector3(start.x, start.y, start.z),
        btVector3(end.x, end.y, end.z)
    );

    ComponentsManager::get()->getComponentCollisions()->getDynamicsWorld()->rayTest(
        btVector3(start.x, start.y, start.z),
        btVector3(end.x, end.y, end.z),
        rayCallback
    );

    if (rayCallback.hasHit()) {
        auto *collisionable = (Collisionable *) rayCallback.m_collisionObject->getUserPointer();

        btVector3 rayHitPosition = rayCallback.m_hitPointWorld;

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

void RayCollisionable::setHitPosition(const Vertex3D &hitPosition) {
    RayCollisionable::hitPosition = hitPosition;
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



