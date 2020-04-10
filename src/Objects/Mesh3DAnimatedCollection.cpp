//
// Created by darkhead on 3/4/20.
//

#include "../../headers/Objects/Mesh3DAnimatedCollection.h"

void Mesh3DAnimatedCollection::addAnimation(std::string label, std::string modelFilename, float scale, bool remove_at_end)
{
    auto* meshObject = new Mesh3DAnimated();
    meshObject->setLabel(label);
    meshObject->setParent( this );

    if ( meshObject->AssimpLoad(EngineSetup::getInstance()->MODELS_FOLDER + modelFilename) ) {
        meshObject->setScale( scale );
        meshObject->setDrawOffset( this-> drawOffset );
        meshObject->setPosition( *this->getPosition() );
        meshObject->setRotation( this->rotationFixed );
        meshObject->setRemoveAtEndAnimation( remove_at_end );
    }

    mesh3Danimated.push_back( meshObject );
}

void Mesh3DAnimatedCollection::onUpdate()
{
    if (this->currentAnimation < 0) return;

    this->getCurrentMesh3DAnimated()->setPosition( *this->getPosition() );
    this->getCurrentMesh3DAnimated()->setRotation( this->getRotation() * rotationFixed);
    this->getCurrentMesh3DAnimated()->onUpdate();
}

void Mesh3DAnimatedCollection::setAnimation(int index)
{
    this->currentAnimation = index;
}

Mesh3DAnimated *Mesh3DAnimatedCollection::getCurrentMesh3DAnimated() const {
    return this->mesh3Danimated[currentAnimation];
}

std::vector<Mesh3DAnimated *> Mesh3DAnimatedCollection::copyFrom(Mesh3DAnimatedCollection *from)
{
    for (int i = 0; i < from->mesh3Danimated.size(); i++) {
        Mesh3DAnimated *copy = new Mesh3DAnimated();
        copy->scene = from->mesh3Danimated[i]->scene;
        copy->Init();
        this->mesh3Danimated.push_back( copy );
    }
}
