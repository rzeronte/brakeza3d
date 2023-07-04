//
// Created by eduardo on 3/07/23.
//

#include "PlayerSatellite.h"
#include "../../../include/Brakeza3D.h"

PlayerSatellite::PlayerSatellite(Mesh3D *parent)
:
    LivingObject(parent),
    damage(100)
{
    setParent(parent);

    shaderParticles = new ShaderParticles(true, Color::yellow(), Color::green(), OCParticlesContext(
        0.0f,
        0.0025f,
        1.5f,
        45.0f,
        0.0f,
        50.0f,
        50.0f,
        255.0f,
        2.0f,
        0.8f,
        0.98f
    ));
}

void PlayerSatellite::onStartSetup()
{
    makeSimpleGhostBody(
        Vertex3D(200, 200, 200),
        Brakeza3D::get()->getComponentsManager()->getComponentCollisions()->getDynamicsWorld(),
        EngineSetup::collisionGroups::Projectile,
        EngineSetup::collisionGroups::Enemy
    );

    blink = new ShaderBlink(false, this, 0.05, Color::red());
}

void PlayerSatellite::onUpdate()
{
    Mesh3D::onUpdate();
    Mesh3DGhost::integrate();


    setPosition(ComponentsManager::get()->getComponentGame()->getPlayer()->getPosition());

    auto l = Vertex3D(1800, 0, 0);
    auto r = getRotation() * M3::getMatrixRotationForEulerAngles(1, 0, 0);

    addToPosition(r * l);

}

void PlayerSatellite::postUpdate()
{
    Mesh3D::postUpdate();
}

void PlayerSatellite::onDraw() {
    Mesh3D::onDraw();
}

void PlayerSatellite::resolveCollision(Collisionable *collisionable)
{
    Mesh3DGhost::resolveCollision(collisionable);

    auto *enemy = dynamic_cast<EnemyGhost*> (collisionable);
    if (enemy != nullptr) {
        blink->setEnabled(true);
        counterDamageBlink.setEnabled(true);
    }
}

void PlayerSatellite::drawCall()
{
    Object3D::drawCall();

    if (counterDamageBlink.isEnabled()) {
        counterDamageBlink.update();
        blink->update();
        if (counterDamageBlink.isFinished()) {
            blink->setEnabled(false);
        }
    }

    updateShaderParticles();
}

float PlayerSatellite::getDamage() const {
    return damage;
}


void PlayerSatellite::updateShaderParticles()
{
    shaderParticles->update(
        Transforms::WorldToPoint(
            getPosition(),
            ComponentsManager::get()->getComponentCamera()->getCamera()
        ),
        AxisUp(),
        1.0f
    );
}
