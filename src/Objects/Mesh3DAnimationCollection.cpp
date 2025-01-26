#include <utility>

#include "../../include/Objects/Mesh3DAnimationCollection.h"
#include "../../include/ComponentsManager.h"

void
Mesh3DAnimationCollection::addAnimation(const std::string& label, const std::string& modelFilename, float scale, bool remove_at_end) {
    auto *meshObject = new Mesh3DAnimation();
    meshObject->setLabel(label);
    meshObject->setParent(this);

    if (meshObject->AssimpLoadAnimation(EngineSetup::get()->MODELS_FOLDER + modelFilename)) {
        meshObject->setScale(scale);
        meshObject->setDrawOffset(this->drawOffset);
        meshObject->setPosition(this->getPosition());
        meshObject->setRotation(this->rotationFixed);
        meshObject->setRemoveAtEndAnimation(remove_at_end);
    }

    mesh3Danimated.push_back(meshObject);
}

void Mesh3DAnimationCollection::onUpdate() {
    Object3D::onUpdate();

    this->getCurrentMesh3DAnimated()->updateBoundingBox();

    this->onUpdateCurrentMesh3D();
    /*this->getCurrentMesh3DAnimated()->sendTrianglesToFrame(
            &ComponentsManager::get()->getComponentRender()->getFrameTriangles()
    );*/
}

void Mesh3DAnimationCollection::onUpdateCurrentMesh3D() {
    if (this->currentAnimation < 0) return;

    this->getCurrentMesh3DAnimated()->setPosition(this->getPosition());
    this->getCurrentMesh3DAnimated()->setRotation(this->getRotation() * rotationFixed);
    this->getCurrentMesh3DAnimated()->updateFrameTransformations();
}

void Mesh3DAnimationCollection::setAnimation(int index) {
    this->currentAnimation = index;
}

Mesh3DAnimation *Mesh3DAnimationCollection::getCurrentMesh3DAnimated() const {
    return this->mesh3Danimated[currentAnimation];
}

const M3 &Mesh3DAnimationCollection::getRotationFixed() const {
    return rotationFixed;
}

void Mesh3DAnimationCollection::setRotationFixed(const M3 &rotationFixed) {
    Mesh3DAnimationCollection::rotationFixed = rotationFixed;
}
