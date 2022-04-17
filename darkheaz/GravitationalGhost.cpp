//
// Created by eduardo on 3/04/22.
//

#include "include/GravitationalGhost.h"
#include "../include/Brakeza3D.h"

GravitationalGhost::GravitationalGhost(float force, float factor) : force(force), factor(factor)
{
}

float GravitationalGhost::getForce() const {
    return force;
}

void GravitationalGhost::setForce(float force) {
    GravitationalGhost::force = force;
}

void GravitationalGhost::onUpdate() {
    Mesh3D::onUpdate();
    auto objects = Brakeza3D::get()->getSceneObjects();

    for (auto object : objects) {
        auto body = dynamic_cast<Body*> (object);
        if (body != nullptr) {
            Vector3D direction(object->getPosition(), getPosition());
            const float distance = direction.getComponent().getModule() / force;
            btVector3 btDirection;
            direction.getComponent().getNormalize().getScaled(force/distance * factor).saveToBtVector3(&btDirection);
            body->getRigidBody()->applyCentralImpulse(btDirection);
        }
    }
}

float GravitationalGhost::getFactor() const {
    return factor;
}

void GravitationalGhost::setFactor(float factor) {
    GravitationalGhost::factor = factor;
}
