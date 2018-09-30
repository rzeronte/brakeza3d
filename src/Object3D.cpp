
#include <iostream>
#include "../headers/Object3D.h"
#include "../headers/Point2D.h"
#include "../headers/Tools.h"
#include "../headers/Line2D.h"
#include "../headers/Color.h"
#include "../headers/Render.h"
#include "../headers/M3.h"
#include "../headers/Billboard.h"


Object3D::Object3D()
{
    this->enabled = true;
    this->position = Vertex(1, 1, 1);
    this->rotation = Vertex(0, 0, 0);

    this->right   = Vector3D( Vertex(0, 0, 0), Vertex(1, 0, 0) );
    this->up      = Vector3D( Vertex(0, 0, 0), Vertex(0, 1, 0) );
    this->forward = Vector3D( Vertex(0, 0, 0), Vertex(0, 0, 1) );

    this->scale = 1;

    this->handleKeyboard = false;

    //this->billboard = new Billboard();
    //this->billboard->loadTexture("../pak0/icons/icona_rocket.tga");
}

Vertex* Object3D::getPosition() {
    return &position;
}

Vertex* Object3D::getRotation() {
    return &rotation;
}

void Object3D::setPosition(Vertex p) {
    position = p;
}

void Object3D::setRotation(Vertex r) {
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

    Vertex r = (MRZ * MRY * MRX) * Vertex(1, 0, 0);
    Vertex u = (MRZ * MRY * MRX) * Vertex(0, 1, 0);
    Vertex f = (MRZ * MRY * MRX) * Vertex(0, 0, 1);

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
