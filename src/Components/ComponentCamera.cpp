
#include "../../include/Components/ComponentCamera.h"
#include "../../include/Brakeza3D.h"

ComponentCamera::ComponentCamera() {
    this->camera = new Camera3D();
}

void ComponentCamera::onStart() {
    Logging::Log("ComponentCamera onStart", "ComponentCamera");
}

void ComponentCamera::preUpdate() {
    getCamera()->velocity.vertex1 = getCamera()->getPosition();
    getCamera()->velocity.vertex2 = getCamera()->getPosition();
}

void ComponentCamera::onUpdate() {
    if (isAutoScroll()) {
        getCamera()->addToPosition(autoScrollSpeed);
    }
    getCamera()->UpdateVelocity();
}

void ComponentCamera::postUpdate() {
    if (this->freeLook) {
        getCamera()->UpdateRotation();
        getCamera()->UpdatePositionForVelocity();
    }

    /*if (getCamera()->getFollowTo() != nullptr) {
        updatePositionForTrackingObject();
    }*/

    this->getCamera()->updateFrustum();
}

void ComponentCamera::onEnd() {

}

void ComponentCamera::onSDLPollEvent(SDL_Event *e, bool &finish) {
}

Camera3D *ComponentCamera::getCamera() const {
    return camera;
}

bool ComponentCamera::isFreeLookEnabled() const {
    return freeLook;
}

void ComponentCamera::setFreeLook(bool value) {
    freeLook = value;
}

void ComponentCamera::updatePositionForTrackingObject() {
    getCamera()->setPosition(getCamera()->getFollowTo()->getPosition() + getCamera()->followToPositionOffset);
}

const Vertex3D &ComponentCamera::getAutoScrollSpeed() const {
    return autoScrollSpeed;
}

void ComponentCamera::setAutoScrollSpeed(const Vertex3D &autoScrollSpeed) {
    ComponentCamera::autoScrollSpeed = autoScrollSpeed;
}

bool ComponentCamera::isAutoScroll() const {
    return autoScroll;
}

void ComponentCamera::setAutoScroll(bool autoScroll) {
    ComponentCamera::autoScroll = autoScroll;
}





