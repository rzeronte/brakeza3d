//
// Created by darkhead on 3/4/20.
//

#include "../../headers/Objects/Mesh3DAnimatedCollection.h"
#include "../../headers/Render/Drawable.h"
#include "../../headers/ComponentsManager.h"

void Mesh3DAnimatedCollection::addAnimation(std::string label, std::string modelFilename, float scale, bool remove_at_end)
{
    auto* meshObject = new Mesh3DAnimated();
    meshObject->setLabel(label);
    meshObject->setParent( this );

    if (meshObject->AssimpLoadAnimation(EngineSetup::getInstance()->MODELS_FOLDER + modelFilename) ) {
        meshObject->setScale( scale );
        meshObject->setDrawOffset( this-> drawOffset );
        meshObject->setPosition( this->getPosition() );
        meshObject->setRotation( this->rotationFixed );
        meshObject->setRemoveAtEndAnimation( remove_at_end );
    }

    mesh3Danimated.push_back( meshObject );
}

void Mesh3DAnimatedCollection::onUpdate()
{
    Object3D::onUpdate();
    
    this->getCurrentMesh3DAnimated()->updateBoundingBox();

    if (EngineSetup::getInstance()->DRAW_MESH3D_AABB) {
        Drawable::drawAABB( &this->getCurrentMesh3DAnimated()->aabb, this );
    }

    if (!ComponentsManager::get()->getComponentCamera()->getCamera()->frustum->isAABBInFrustum( &this->getCurrentMesh3DAnimated()->aabb )) {
        return;
    }

    this->onUpdateCurrentMesh3D();
    this->getCurrentMesh3DAnimated()->draw( &ComponentsManager::get()->getComponentRender()->getFrameTriangles() );
}

void Mesh3DAnimatedCollection::onUpdateCurrentMesh3D()
{
    if (this->currentAnimation < 0) return;

    this->getCurrentMesh3DAnimated()->setPosition( this->getPosition() );
    this->getCurrentMesh3DAnimated()->setRotation( this->getRotation() * rotationFixed);
    this->getCurrentMesh3DAnimated()->updateFrameTransformations();
}

void Mesh3DAnimatedCollection::setAnimation(int index)
{
    this->currentAnimation = index;
}

Mesh3DAnimated *Mesh3DAnimatedCollection::getCurrentMesh3DAnimated() const {
    return this->mesh3Danimated[currentAnimation];
}

const M3 &Mesh3DAnimatedCollection::getRotationFixed() const {
    return rotationFixed;
}

void Mesh3DAnimatedCollection::setRotationFixed(const M3 &rotationFixed) {
    Mesh3DAnimatedCollection::rotationFixed = rotationFixed;
}
