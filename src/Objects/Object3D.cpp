
#include <iostream>
#include "../../headers/Objects/Object3D.h"
#include "../../headers/Objects/Point2D.h"
#include "../../headers/Render/Tools.h"
#include "../../headers/Objects/Line2D.h"
#include "../../headers/Render/Color.h"
#include "../../headers/Render/Transforms.h"
#include "../../headers/Render/M3.h"
#include "../../headers/Render/Billboard.h"


Object3D::Object3D()
{
    this->enabled = true;
    this->position = Vertex3D(1, 1, 1);
    this->rotation = Vertex3D(0, 0, 0);

    this->right   = Vector3D( Vertex3D(0, 0, 0), Vertex3D(1, 0, 0) );
    this->up      = Vector3D( Vertex3D(0, 0, 0), Vertex3D(0, 1, 0) );
    this->forward = Vector3D( Vertex3D(0, 0, 0), Vertex3D(0, 0, 1) );

    this->scale = 1;

    this->handleKeyboard = false;

    //this->billboard = new Billboard();
    //this->billboard->loadTexture("../pak0/icons/icona_rocket.tga");
}

Vertex3D* Object3D::getPosition() {
    return &position;
}

Vertex3D* Object3D::getRotation() {
    return &rotation;
}

void Object3D::setPosition(Vertex3D p) {
    position = p;
}

void Object3D::setRotation(Vertex3D r) {
    this->rotation = r;
}

std::string Object3D::getLabel() const {
    return label;
}

void Object3D::setLabel(const std::string label) {
    Object3D::label = label;
}

void Object3D::updateAxis()
{
    M3 MRX = M3::RX(getRotation()->x);
    M3 MRY = M3::RY(getRotation()->y);
    M3 MRZ = M3::RZ(getRotation()->z);

    Vertex3D r = (MRZ * MRY * MRX) * Vertex3D(1, 0, 0);
    Vertex3D u = (MRZ * MRY * MRX) * Vertex3D(0, 1, 0);
    Vertex3D f = (MRZ * MRY * MRX) * Vertex3D(0, 0, 1);

    r.addVertex(*getPosition());
    u.addVertex(*getPosition());
    f.addVertex(*getPosition());

    Vector3D axis_x = Vector3D( *getPosition(), r );
    Vector3D axis_y = Vector3D( *getPosition(), u );
    Vector3D axis_z = Vector3D( *getPosition(), f );

    this->right   = axis_x;
    this->up      = axis_y;
    this->forward = axis_z;
}

bool Object3D::isHandleKeyboard() const {
    return handleKeyboard;
}

void Object3D::setHandleKeyboard(bool handleKeyboard) {
    Object3D::handleKeyboard = handleKeyboard;
}

bool Object3D::isEnabled() const {
    return enabled;
}

void Object3D::setEnabled(bool enabled) {
    Object3D::enabled = enabled;
}
