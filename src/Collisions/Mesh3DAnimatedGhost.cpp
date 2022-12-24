#include "../../include/Physics/Mesh3DAnimatedGhost.h"


Mesh3DAnimatedGhost::Mesh3DAnimatedGhost()
{

}


void Mesh3DAnimatedGhost::integrate()
{
    updateBulletFromMesh3D();
}


void Mesh3DAnimatedGhost::onUpdate()
{
    Mesh3DAnimated::onUpdate();
}

void Mesh3DAnimatedGhost::resolveCollision(Collisionable *with)
{
    if (EngineSetup::get()->LOG_COLLISION_OBJECTS) {
        auto *object = dynamic_cast<Object3D*> (with);
        Logging::getInstance()->Log("Mesh3DAnimatedGhost: Collision "  + getLabel() + " with " + object->getLabel());
    }
}

void Mesh3DAnimatedGhost::updateBulletFromMesh3D()
{
    btTransform trans;
    trans.setIdentity();
    trans.setOrigin(btVector3(getPosition().x, getPosition().y, getPosition().z));

    btMatrix3x3 matrixRotation = Tools::M3ToBulletM3(getRotation());
    btQuaternion rotation;
    matrixRotation.getRotation(rotation);

    trans.setRotation(rotation);
    getGhostObject()->setWorldTransform(trans);
}

void Mesh3DAnimatedGhost::remove()
{
    this->setRemoved(true);
}
