#include "RayLight.h"
#include "../../include/LUAManager.h"
#include "../../include/Brakeza3D.h"
#include "../Items/ItemShieldGhost.h"

RayLight::RayLight(bool enabled, Object3D *parent, Vertex3D direction, Vertex3D startOffset, float speed, float damage, Color c, Color hit, int filterGroup, int filterMask)
:
    enabled(enabled),
    intensity(0),
    reach(0),
    speed(speed),
    damage(damage),
    color(c),
    parent(parent),
    direction(direction),
    startOffset(startOffset)
{
    rayCallback = new btCollisionWorld::ClosestRayResultCallback(
        parent->getPosition().toBullet(),
        parent->getPosition().toBullet()
    );

    rayCallback->m_collisionFilterGroup = filterGroup;
    rayCallback->m_collisionFilterMask = filterMask;

    light = LightPoint3D::base();
    light->setMultiScene(true);
    light->setRotation(180, 0, 0);
    light->setEnabled(false);
    Brakeza3D::get()->addObject3D(light, Brakeza3D::uniqueObjectLabel("rayLightPoint"));
}

RayLight::~RayLight()
{
    light->setRemoved(true);
}

void RayLight::update()
{
    if (!isEnabled()) return;

    light->setEnabled(false);

    auto game = ComponentsManager::get()->getComponentGame();

    Vertex3D start = parent->getPosition() + startOffset;
    Point2D screenPoint = Transforms::WorldToPoint(start);

    Vertex3D end = parent->getPosition() + startOffset + direction.getScaled(reach);
    Point2D middlePoint = Transforms::WorldToPoint(end);

    rayCallback->m_rayFromWorld = start.toBullet();
    rayCallback->m_rayToWorld = end.toBullet();

    ComponentsManager::get()->getComponentCollisions()->getDynamicsWorld()->rayTest(
        start.toBullet(),
        end.toBullet(),
        *rayCallback
    );

    bool increase = true;

    if (rayCallback->hasHit()) {
        auto *brkObjectA = (Collisionable *) rayCallback->m_collisionObject->getUserPointer();

        auto *object = dynamic_cast<Object3D*> (brkObjectA);
        auto *enemy = dynamic_cast<EnemyGhost*> (brkObjectA);
        auto *player = dynamic_cast<Player*> (brkObjectA);
        auto *bomb = dynamic_cast<ItemShieldGhost*> (brkObjectA);

        if (object != this->parent) {
            btVector3 rayHitPosition = rayCallback->m_hitPointWorld;
            auto hitPosition = Vertex3D(rayHitPosition.x(), rayHitPosition.y(), rayHitPosition.z());
            auto dt = Brakeza3D::get()->getDeltaTime() * 50;

            if (player != nullptr) {
                middlePoint = Transforms::WorldToPoint(hitPosition);

                player->takeDamage(damage * dt );
                increase = false;

                light->setEnabled(true);
                light->setPosition(hitPosition + Vertex3D(0, 0, -1));
                light->onUpdate();
            }

            if (bomb != nullptr) {
                middlePoint = Transforms::WorldToPoint(hitPosition);
                increase = false;

                light->setEnabled(true);
                light->setPosition(hitPosition + Vertex3D(0, 0, -1000));
                light->onUpdate();
            }

            if (enemy != nullptr) {
                middlePoint = Transforms::WorldToPoint(hitPosition);
                ComponentsManager::get()->getComponentGame()->getLevelLoader()->getStats()->increaseHit(WEAPON_RAYLIGHT);

                enemy->takeDamage(damage * dt);
                increase = false;

                light->setEnabled(true);
                light->setPosition(hitPosition + Vertex3D(0, 0, -1));
                light->onUpdate();
            }
        }
    }

    if (increase) {
        increaseReach();
    }

    game->getShaderLasers()->addLaser(
        glm::vec2(screenPoint.x, screenPoint.y),
        glm::vec2(middlePoint.x, middlePoint.y),
        color.toGLM(),
        intensity
    );

    if (EngineSetup::get()->BULLET_DEBUG_MODE) {
        Drawable::drawVector3D(Vector3D(start, end), Color::yellow());
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
    reach = reach + 0.001f * speed;
}

bool RayLight::isEnabled() const {
    return enabled;
}

void RayLight::setEnabled(bool enabled) {
    RayLight::enabled = enabled;
    light->setEnabled(false);
}

void RayLight::setColor(const Color &color) {
    RayLight::color = color;
}

const Vertex3D &RayLight::getDirection() const {
    return direction;
}

void RayLight::setDirection(Vertex3D direction) {
    RayLight::direction = direction;
}

void RayLight::updateDirection(Vertex3D direction, Vertex3D startsOffset)
{
    this->direction = direction;
    this->startOffset = startsOffset;
}

Object3D *RayLight::getParent() const {
    return parent;
}

void RayLight::setReach(float value) {
    reach = value;
}
