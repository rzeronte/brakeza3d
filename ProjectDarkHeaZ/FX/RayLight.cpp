#include "RayLight.h"
#include "../../include/LUAManager.h"
#include "../../include/Brakeza3D.h"
#include "../Items/ItemShieldGhost.h"

RayLight::RayLight(
    bool enabled,
    Object3D *parent,
    Vertex3D direction,
    Vertex3D startOffset,
    float speed,
    float damage,
    Color c,
    Color hit,
    int filterGroup,
    int filterMask
)
:
    intensity(0),
    reach(0),
    speed(speed),
    damage(damage),
    color(c),
    hit(hit),
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
}

RayLight::~RayLight()
{
}

void RayLight::update(bool increase)
{
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

    result.wasHit = false;
    result.position = end;

    if (rayCallback->hasHit()) {
        result.wasHit = true;

        auto *brkObjectA = (Collisionable *) rayCallback->m_collisionObject->getUserPointer();

        auto *object = dynamic_cast<Object3D*> (brkObjectA);
        auto *enemy = dynamic_cast<EnemyGhost*> (brkObjectA);
        auto *player = dynamic_cast<Player*> (brkObjectA);
        auto *bomb = dynamic_cast<ItemShieldGhost*> (brkObjectA);

        btVector3 rayHitPosition = rayCallback->m_hitPointWorld;
        auto hitPosition = Vertex3D(rayHitPosition.x(), rayHitPosition.y(), rayHitPosition.z());
        result.position = hitPosition;

        if (object != this->parent) {
            auto dt = Brakeza3D::get()->getDeltaTime() * 50;

            if (player != nullptr) {
                middlePoint = Transforms::WorldToPoint(hitPosition);

                player->takeDamage(damage * dt );
                increase = false;
                game->getShaderLasers()->addLaser(
                    glm::vec2(middlePoint.x, middlePoint.y),
                    glm::vec2(middlePoint.x, middlePoint.y),
                    hit.toGLM(),
                    12.0f,
                    intensity,
                    1.25f
                );
            }

            if (bomb != nullptr) {
                middlePoint = Transforms::WorldToPoint(hitPosition);
                increase = false;
            }

            if (enemy != nullptr) {
                middlePoint = Transforms::WorldToPoint(hitPosition);
                ComponentsManager::get()->getComponentGame()->getLevelLoader()->getStats()->increaseHit(WEAPON_RAYLIGHT);

                enemy->takeDamage(damage * dt);
                increase = false;

                game->getShaderLasers()->addLaser(
                    glm::vec2(middlePoint.x, middlePoint.y),
                    glm::vec2(middlePoint.x, middlePoint.y),
                    hit.toGLM(),
                    12.0f,
                    intensity,
                    5.0f
                );
            }
        }
        end = hitPosition;
    }

    if (increase) {
        increaseReach();
    }

    game->getShaderLasers()->addLaser(
        glm::vec2(screenPoint.x, screenPoint.y),
        glm::vec2(middlePoint.x, middlePoint.y),
        color.toGLM(),
        10.0f,
        intensity,
        0.25f
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

void RayLight::setDamage(float value) {
    RayLight::damage = value;
}

void RayLight::increaseReach() {
    reach = reach + 0.001f * speed;
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

void RayLight::setReach(float value) {
    reach = value;
}

float RayLight::getReach() const {
    return reach;
}

void RayLight::drawImGuiProperties()
{
}

void RayLight::setCollisionMask(int filterGroup, int filterMask)
{
    rayCallback->m_collisionFilterGroup = filterGroup;
    rayCallback->m_collisionFilterMask = filterMask;
}

ResultRay RayLight::getResult() {
    return result;
}