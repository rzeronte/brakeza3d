//
// Created by darkhead on 2019-08-25.
//

#include "../../headers/Physics/Body.h"
#include "../../headers/EngineSetup.h"
#include "../../headers/Misc/Tools.h"

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

void Body::applyImpulse(Vertex3D impulse)
{

    this->m_body->clearForces();

    if (!Tools::isValidVector(impulse) ) {
        return;
    }

    btVector3 i( impulse.x, impulse.y, impulse.z );

    this->m_body->activate(true);
    this->m_body->applyCentralImpulse( i );
}

