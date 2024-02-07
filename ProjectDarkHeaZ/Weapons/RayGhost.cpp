//
// Created by edu on 6/02/24.
//

#include "RayGhost.h"
#include "../../include/ComponentsManager.h"

RayGhost::RayGhost(float damage, Color c, Object3D *targetOne, Object3D *targetTwo, int filterGroup, int filterMask)
:
    color(c),
    damage(damage),
    targetOne(targetOne),
    targetTwo(targetTwo),
    filterGroup(filterGroup),
    filterMask(filterMask)
{
    setPosition(Vertex3D(0, 0, 0));
    updateShape();
}

void RayGhost::onUpdate() {
    Object3D::onUpdate();

    if (targetOne->isRemoved() || targetTwo->isRemoved()) {
        setRemoved(true);
    }

    updateShape();
    updateBulletFromMesh3D();
}

void RayGhost::updateShape()
{
    removeCollisionObject();

    setTypeShape(GhostTypeShape::GHOST_SIMPLE_SHAPE);

    btTransform transformation;
    transformation.setIdentity();
    transformation.setOrigin(btVector3(0, 0, 0));

    ghostObject->setCollisionShape(CustomShape(targetOne->getPosition().toBullet(), targetTwo->getPosition().toBullet(), 0.5, 2));
    ghostObject->setWorldTransform(transformation);
    ghostObject->setUserPointer(dynamic_cast<Ghost *> (this));

    ghostObject->setCollisionFlags(btCollisionObject::CF_NO_CONTACT_RESPONSE);

    ComponentsManager::get()->getComponentCollisions()->getDynamicsWorld()->addCollisionObject(
        ghostObject,
        filterGroup,
        filterMask
    );
}

void RayGhost::resolveCollision(Collisionable *collisionable) {
    auto projectile = dynamic_cast<AmmoProjectileBody*> (collisionable);
    if (projectile != nullptr) {
    }
}

void RayGhost::updateBulletFromMesh3D()
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

btCollisionShape* RayGhost::CustomShape(const btVector3& startPoint, const btVector3& endPoint, float width, float height)
{
    btVector3 direction = (endPoint - startPoint).normalized();
    btVector3 perpendicular = btVector3(0, 0, 1).cross(direction).normalized();

    btVector3 widthVec = perpendicular * width / 2.0f;

    btVector3 upVec = direction.cross(perpendicular).normalized();
    btVector3 heightVec = upVec * height / 2.0f;

    btVector3 originVertices[4] = {
        startPoint + widthVec + heightVec, // Esquina superior derecha
        startPoint - widthVec + heightVec, // Esquina superior izquierda
        startPoint - widthVec - heightVec, // Esquina inferior izquierda
        startPoint + widthVec - heightVec, // Esquina inferior derecha
    };
    btVector3 endVertices[4] = {
            endPoint + widthVec + heightVec,
            endPoint - widthVec + heightVec,
            endPoint - widthVec - heightVec,
            endPoint + widthVec - heightVec,
    };

    auto mesh = new btTriangleMesh();

    // Caras laterales
    for (int i = 0; i < 4; ++i) {
        mesh->addTriangle(originVertices[i], originVertices[(i + 1) % 4], endVertices[i]);
        mesh->addTriangle(endVertices[i], originVertices[(i + 1) % 4], endVertices[(i + 1) % 4]);
    }

    // Cara superior
    mesh->addTriangle(originVertices[0], originVertices[1], originVertices[2]);
    mesh->addTriangle(originVertices[0], originVertices[2], originVertices[3]);

    // Cara inferior
    mesh->addTriangle(endVertices[0], endVertices[2], endVertices[1]);
    mesh->addTriangle(endVertices[0], endVertices[3], endVertices[2]);

    return new btConvexTriangleMeshShape(mesh);
}

void RayGhost::drawImGuiProperties()
{
    Object3D::drawImGuiProperties();

    if (ImGui::TreeNode("RayGhost")) {

        ImGui::TreePop();
    }
}

const Vertex3D &RayGhost::getTargetOne() const {
    return targetOne->getPosition();
}

const Vertex3D &RayGhost::getTargetTwo() const {
    return targetTwo->getPosition();
}

float RayGhost::getDamage() const {
    return damage;
}

const Color &RayGhost::getColor() const {
    return color;
}
