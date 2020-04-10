//
// Created by darkhead on 2019-08-25.
//

#include "../../headers/Physics/Body.h"
#include "../../headers/Objects/Vertex3D.h"


Body::Body() {

}

void Body::integrate() {

}

void Body::setBodyEnabled(bool state)
{
    this->bodyEnabled = state;
}

void Body::setBoxShapeSize(Vertex3D size)
{
    this->boxShapeSize = size;
}

Vertex3D Body::getBoxShapeSize() {
    return this->boxShapeSize;
}

