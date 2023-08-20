#include "RayLight.h"
#include "../../include/EngineBuffers.h"
#include "../../include/Brakeza3D.h"
#include "../Items/ItemShieldGhost.h"

RayLight::RayLight(bool enabled, Object3D *parent, Vertex3D direction, Vertex3D startOffset, float speed, float damage, Color c, int filterGroup, int filterMask)
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
    Vertex3D start = parent->getPosition();

    rayCallback = new btCollisionWorld::ClosestRayResultCallback(
        btVector3(start.x, start.y, start.z),
        btVector3(start.x, start.y, start.z)
    );

    rayCallback->m_collisionFilterGroup = filterGroup;
    rayCallback->m_collisionFilterMask = filterMask;

    light = new LightPoint3D(20, 1, 0, 0, 0, PaletteColors::getStamina(), Color(15, 33, 92));
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

    auto game = ComponentsManager::get()->getComponentGame();
    auto camera = ComponentsManager::get()->getComponentCamera()->getCamera();

    Vertex3D start = parent->getPosition() + startOffset;
    Point2D screenPoint = Transforms::WorldToPoint(start, camera);

    Vertex3D end = parent->getPosition() + direction.getScaled(reach);
    Point2D middlePoint = Transforms::WorldToPoint(end, camera);

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
        auto *bomb = dynamic_cast<ItemShieldGhost*> (brkObjectA);

        if (object != this->parent) {
            btVector3 rayHitPosition = rayCallback->m_hitPointWorld;
            auto hitPosition = Vertex3D(rayHitPosition.x(), rayHitPosition.y(), rayHitPosition.z());
            auto dt = Brakeza3D::get()->getDeltaTime() * 50;

            if (player != nullptr) {
                middlePoint = Transforms::WorldToPoint(hitPosition, camera);
                Tools::makeExplosion(player, hitPosition, 0.5, OCParticlesContext::forRayLight(), PaletteColors::getExplosionEnemyFrom(), PaletteColors::getExplosionEnemyTo());

                player->takeDamage(damage * dt );
                increase = false;
            }

            if (bomb != nullptr) {
                middlePoint = Transforms::WorldToPoint(hitPosition, camera);
                increase = false;
            }

            if (enemy != nullptr) {
                middlePoint = Transforms::WorldToPoint(hitPosition, camera);
                ComponentsManager::get()->getComponentGame()->getLevelLoader()->getStats()->increaseHit(WEAPON_RAYLIGHT);

                enemy->takeDamage(damage * dt);
                increase = false;

                Tools::makeExplosion(parent, hitPosition, 0.5, OCParticlesContext::forRayLight(), PaletteColors::getExplosionEnemyFrom(), PaletteColors::getExplosionEnemyTo());

                light->setEnabled(true);
                light->setPosition(hitPosition + Vertex3D(0, 0, -1000));
                light->onUpdate();

            }
        }
    }

    if (increase) {
        increaseReach();
    }

    game->getShaderLasers()->addLaser(
        screenPoint.x, screenPoint.y,
        middlePoint.x, middlePoint.y,
        color,
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
    if (!enabled) {
        light->setEnabled(false);
    }
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

void RayLight::setReach(int value) {
    reach = value;
}
