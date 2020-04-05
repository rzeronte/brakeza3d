//
// Created by darkhead on 3/4/20.
//

#include "../../headers/Objects/Mesh3DAnimatedCollection.h"
#include "../../headers/Brakeza3D.h"

void Mesh3DAnimatedCollection::addAnimation(std::string label, std::string modelFilename)
{
    auto* meshObject = new Mesh3DAnimated();
    meshObject->setLabel(label);

    if ( meshObject->AssimpLoad(EngineSetup::getInstance()->MODELS_FOLDER + modelFilename) ) {

        meshObject->setScale( 0.1);
        meshObject->setPosition( *this->getPosition() );
        meshObject->setRotation( M3::getMatrixRotationForEulerAngles(180, 0, 0) );

        meshObject->makeSimpleRigidBody(
                0,
                Vertex3D(1, 1, 1),
                Brakeza3D::get()->getComponentsManager()->getComponentCollisions()->getDynamicsWorld()
        );

        meshObject->setBodyEnabled(false);
    }

    mesh3Danimated.push_back( meshObject );
}

void Mesh3DAnimatedCollection::onUpdate()
{
    if (this->currentAnimation < 0) return;

    this->getCurrentMesh3DAnimated()->setPosition( *this->getPosition() );
    this->getCurrentMesh3DAnimated()->onUpdate();
}

void Mesh3DAnimatedCollection::setCurrentAnimation(int index)
{
    if (this->currentAnimation >= 0) {
        this->mesh3Danimated[currentAnimation]->setBodyEnabled(false );
    }

    this->currentAnimation = index;

    this->mesh3Danimated[currentAnimation]->setBodyEnabled(true );
}

Mesh3DAnimated *Mesh3DAnimatedCollection::getCurrentMesh3DAnimated() const {
    return this->mesh3Danimated[currentAnimation];
}
