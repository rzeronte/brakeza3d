//
// Created by eduardo on 3/07/23.
//

#include "PlayerSatellite.h"
#include "../../include/Brakeza3D.h"

PlayerSatellite::PlayerSatellite(Mesh3D *parent)
:
    LivingObject(parent),
    damage(500)
{
    setParent(parent);

}

void PlayerSatellite::onStartSetup()
{
    /*makeSimpleGhostBody(
        Vertex3D(1, 1, 1),
        Brakeza3D::get()->getComponentsManager()->getComponentCollisions()->getDynamicsWorld(),
        EngineSetup::collisionGroups::Projectile,
        EngineSetup::collisionGroups::Enemy
    );*/

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

    updateShaderParticles();

    if (counterDamageBlink.isEnabled()) {
        counterDamageBlink.update();
        blink->update();
        if (counterDamageBlink.isFinished()) {
            blink->setEnabled(false);
        }
    }
}

void PlayerSatellite::postUpdate()
{
    Mesh3D::postUpdate();
}

void PlayerSatellite::onDrawHostBuffer() {
    Mesh3D::onDrawHostBuffer();
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

float PlayerSatellite::getDamage() const {
    return damage;
}


void PlayerSatellite::updateShaderParticles()
{

}
