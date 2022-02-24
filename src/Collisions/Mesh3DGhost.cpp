#include "../../include/Physics/Mesh3DGhost.h"
#include "../../include/Render/Logging.h"

Mesh3DGhost::Mesh3DGhost() {
    BSPEntityIndex = -1;
}

void Mesh3DGhost::integrate() {
    updateBulletFromMesh();
}

void Mesh3DGhost::updateBulletFromMesh() {
    btTransform trans;
    trans.setIdentity();
    trans.setOrigin(btVector3(getPosition().x, getPosition().y, getPosition().z));

    btMatrix3x3 matrixRotation = Tools::M3ToBulletM3(getRotation());
    btQuaternion rotation;
    matrixRotation.getRotation(rotation);

    trans.setRotation(rotation);
    getGhostObject()->setWorldTransform(trans);
}

void Mesh3DGhost::resolveCollision(Collisionable *with) {
    if (EngineSetup::get()->LOG_COLLISION_OBJECTS) {
        auto *object = dynamic_cast<Object3D*> (with);
        Logging::getInstance()->Log("Mesh3DGhost: Collision "  + getLabel() + " with " + object->getLabel());
    }
}
