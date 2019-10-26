
#include <iostream>
#include "../../headers/Objects/Object3D.h"
#include "../../headers/Objects/Point2D.h"
#include "../../headers/Render/Tools.h"
#include "../../headers/Objects/Line2D.h"
#include "../../headers/Render/Color.h"
#include "../../headers/Render/Transforms.h"
#include "../../headers/Render/M3.h"
#include "../../headers/Render/Billboard.h"


Object3D::Object3D() : enabled(true), removed(false), position(Vertex3D(1, 1, 1)), scale(1), decal(false) {
}

Vertex3D *Object3D::getPosition() {
    return &position;
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

void Object3D::setLabel(const std::string label) {
    Object3D::label = label;
}


bool Object3D::isEnabled() const {
    return enabled;
}

void Object3D::setEnabled(bool enabled) {
    Object3D::enabled = enabled;
}

Vertex3D Object3D::AxisUp() {
    Vertex3D v = getRotation() * EngineSetup::getInstance()->up;
    return v.getNormalize();
}

Vertex3D Object3D::AxisDown() {
    Vertex3D v = getRotation() * EngineSetup::getInstance()->down;
    return v.getNormalize();
}

Vertex3D Object3D::AxisForward() {
    Vertex3D v = getRotation() * EngineSetup::getInstance()->forward;
    return v.getNormalize();
}

Vertex3D Object3D::AxisBackwards() {
    Vertex3D v = getRotation() * EngineSetup::getInstance()->backward;

    return v.getNormalize();

}

Vertex3D Object3D::AxisRight() {
    Vertex3D v = getRotation() * EngineSetup::getInstance()->right;

    return v.getNormalize();

}

Vertex3D Object3D::AxisLeft() {
    Vertex3D v = getRotation() * EngineSetup::getInstance()->left;

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
