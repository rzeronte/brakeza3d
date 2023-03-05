#include "RayLight.h"
#include "../../../include/EngineBuffers.h"
#include "../../../include/Brakeza3D.h"

RayLight::RayLight(bool enabled, Object3D *parent, float speed, float damage, Color c, int filterGroup, int filterMask)
:
    enabled(enabled),
    intensity(0),
    reach(0),
    speed(speed),
    damage(damage),
    color(c),
    parent(parent)
{
    Vertex3D start = parent->getPosition();

    rayCallback = new btCollisionWorld::ClosestRayResultCallback(
        btVector3(start.x, start.y, start.z),
        btVector3(start.x, start.y, start.z)
    );

    rayCallback->m_collisionFilterGroup = filterGroup;
    rayCallback->m_collisionFilterMask = filterMask;
}

void RayLight::update()
{
    if (!isEnabled()) return;

    Vertex3D start = parent->getPosition() + parent->AxisDown().getScaled(1100);
    Point2D screenPoint = Transforms::WorldToPoint(start, ComponentsManager::get()->getComponentCamera()->getCamera());

    Vertex3D end = parent->getPosition() + parent->AxisDown().getScaled(reach);
    Point2D middlePoint = Transforms::WorldToPoint(end, ComponentsManager::get()->getComponentCamera()->getCamera());

    rayCallback->m_rayFromWorld = btVector3(start.x, start.y, start.z);
    rayCallback->m_rayToWorld = btVector3(end.x, end.y, end.z);

    ComponentsManager::get()->getComponentCollisions()->getDynamicsWorld()->rayTest(
        btVector3(start.x, start.y, start.z),
        btVector3(end.x, end.y, end.z),
        *rayCallback
    );

    bool increase = true;

    if (rayCallback->hasHit()) {
        auto *brkObjectA = (Collisionable *) rayCallback->m_collisionObject->getUserPointer();

        auto *object = dynamic_cast<Object3D*> (brkObjectA);
        auto *enemy = dynamic_cast<EnemyGhost*> (brkObjectA);
        auto *player = dynamic_cast<Player*> (brkObjectA);

        if (object != this->parent) {
            if (player != nullptr) {
                btVector3 rayHitPosition = rayCallback->m_hitPointWorld;
                auto hitPosition = Vertex3D(rayHitPosition.x(), rayHitPosition.y(), rayHitPosition.z());
                middlePoint = Transforms::WorldToPoint(hitPosition, ComponentsManager::get()->getComponentCamera()->getCamera());

                player->takeDamage(damage);
                increase = false;
            }

            if (enemy != nullptr) {
                btVector3 rayHitPosition = rayCallback->m_hitPointWorld;
                auto hitPosition = Vertex3D(rayHitPosition.x(), rayHitPosition.y(), rayHitPosition.z());
                middlePoint = Transforms::WorldToPoint(hitPosition, ComponentsManager::get()->getComponentCamera()->getCamera());

                enemy->takeDamage(damage);
                increase = false;
            }
        }
    }

    if (increase) {
        increaseReach();
    }

    ComponentsManager::get()->getComponentGame()->getShaderLasers()->addLaser(
        screenPoint.x, screenPoint.y,
        middlePoint.x, middlePoint.y,
        (int) color.r, (int) color.g, (int) color.b,
        intensity,
        true,
        true
    );

    if (EngineSetup::get()->BULLET_DEBUG_MODE) {
        Drawable::drawVector3D(Vector3D(start, end), ComponentsManager::get()->getComponentCamera()->getCamera(), Color::yellow());
    }

    rayCallback->m_closestHitFraction = btScalar(1.),
    rayCallback->m_collisionObject = nullptr;
}

void RayLight::setIntensity(float value) {
    RayLight::intensity = value;
}

void RayLight::resetReach() {
    this->reach = 0;
}

void RayLight::setDamage(float value) {
    RayLight::damage = value;
}

void RayLight::increaseReach() {
    reach = reach + 1 * speed;
}

bool RayLight::isEnabled() const {
    return enabled;
}

void RayLight::setEnabled(bool enabled) {
    RayLight::enabled = enabled;
}

void RayLight::setColor(const Color &color) {
    RayLight::color = color;
}
