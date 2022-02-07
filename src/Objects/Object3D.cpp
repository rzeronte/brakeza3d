
#include <iostream>
#include "../../include/Objects/Object3D.h"
#include "../../include/Misc/Tools.h"
#include "../../include/EngineSetup.h"
#include "../../include/ComponentsManager.h"

Object3D::Object3D() : enabled(true), removed(false), position(Vertex3D(1, 1, 1)), scale(1), decal(false) {
    followCamera = false;
    this->rotX = 0;
    this->rotY = 0;
    this->rotZ = 0;

}

Vertex3D &Object3D::getPosition() {
    return position;
}

M3 Object3D::getRotation() {
    return rotation;
}

void Object3D::setPosition(Vertex3D p) {
    position = p;
}

void Object3D::setRotation(M3 r) {
    this->rotation = r;
}

std::string Object3D::getLabel() const {
    return label;
}

void Object3D::setLabel(const std::string& label) {
    Object3D::label = label;
}


bool Object3D::isEnabled() const {
    return enabled;
}

void Object3D::setEnabled(bool enabled) {
    Object3D::enabled = enabled;
}

Vertex3D Object3D::AxisUp() {
    Vertex3D v = getRotation() * EngineSetup::get()->up;
    return v.getNormalize();
}

Vertex3D Object3D::AxisDown() {
    Vertex3D v = getRotation() * EngineSetup::get()->down;
    return v.getNormalize();
}

Vertex3D Object3D::AxisForward() {
    Vertex3D v = getRotation() * EngineSetup::get()->forward;
    return v.getNormalize();
}

Vertex3D Object3D::AxisBackwards() {
    Vertex3D v = getRotation() * EngineSetup::get()->backward;

    return v.getNormalize();

}

Vertex3D Object3D::AxisRight() {
    Vertex3D v = getRotation() * EngineSetup::get()->right;

    return v.getNormalize();

}

Vertex3D Object3D::AxisLeft() {
    Vertex3D v = getRotation() * EngineSetup::get()->left;

    return v.getNormalize();
}

float Object3D::getScale() const {
    return scale;
}

void Object3D::setScale(float scale) {
    Object3D::scale = scale;
}

bool Object3D::isRemoved() const {
    return removed;
}

void Object3D::setRemoved(bool removed) {
    Object3D::removed = removed;
}

bool Object3D::isDecal() const {
    return decal;
}

void Object3D::setDecal(bool decal) {
    Object3D::decal = decal;
}

void Object3D::setDrawOffset(Vertex3D offset) {
    this->drawOffset = offset;
}

Vertex3D Object3D::getDrawOffset() const {
    return this->drawOffset;
}

Object3D *Object3D::getParent() const {
    return parent;
}

void Object3D::setParent(Object3D *parent) {
    Object3D::parent = parent;
}

bool Object3D::isFollowCamera() const {
    return this->followCamera;
}

void Object3D::setFollowCamera(bool followCamera) {
    Object3D::followCamera = followCamera;
}

void Object3D::onUpdate() {
    if (this->isFollowCamera()) {
        this->setPosition(ComponentsManager::get()->getComponentCamera()->getCamera()->getPosition());
        this->setRotation(ComponentsManager::get()->getComponentCamera()->getCamera()->getRotation().getTranspose());
    }
}
