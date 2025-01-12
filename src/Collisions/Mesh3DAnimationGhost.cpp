#include "../../include/Physics/Mesh3DAnimationGhost.h"


Mesh3DAnimationGhost::Mesh3DAnimationGhost()
{

}


void Mesh3DAnimationGhost::integrate()
{
    updateBulletFromMesh3D();
}

void Mesh3DAnimationGhost::onUpdate()
{
    Mesh3DAnimation::onUpdate();
}

void Mesh3DAnimationGhost::resolveCollision(Collisionable *with)
{
    if (EngineSetup::get()->LOG_COLLISION_OBJECTS) {
        auto *object = dynamic_cast<Object3D*> (with);
        Logging::Log("Mesh3DAnimatedGhost: Collision %s with %s", getLabel().c_str(), object->getLabel().c_str());
    }
}

void Mesh3DAnimationGhost::updateBulletFromMesh3D()
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

void Mesh3DAnimationGhost::remove()
{
    this->setRemoved(true);
}
