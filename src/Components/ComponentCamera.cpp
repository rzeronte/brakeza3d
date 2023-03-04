
#include "../../include/Components/ComponentCamera.h"
#include "../../include/Brakeza3D.h"

ComponentCamera::ComponentCamera()
:
    camera(new Camera3D()),
    freeLook(false),
    autoScroll(false)
{
}

void ComponentCamera::onStart()
{
    Logging::Log("ComponentCamera onStart");
}

void ComponentCamera::preUpdate()
{
    getCamera()->getVelocity().vertex1 = getCamera()->getPosition();
    getCamera()->getVelocity().vertex2 = getCamera()->getPosition();
}

void ComponentCamera::onUpdate()
{
    if (isAutoScroll()) {
        getCamera()->addToPosition(autoScrollSpeed);
    }
    getCamera()->UpdateVelocity();
}

void ComponentCamera::postUpdate()
{
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

void ComponentCamera::onSDLPollEvent(SDL_Event *e, bool &finish)
{
}

Camera3D *ComponentCamera::getCamera() const
{
    return camera;
}

bool ComponentCamera::isFreeLookEnabled() const
{
    return freeLook;
}

void ComponentCamera::setFreeLook(bool value)
{
    freeLook = value;
}

Vertex3D &ComponentCamera::getAutoScrollSpeed() {
    return autoScrollSpeed;
}

void ComponentCamera::setAutoScrollSpeed(const Vertex3D &autoScrollSpeed) {
    ComponentCamera::autoScrollSpeed = autoScrollSpeed;
}

bool &ComponentCamera::isAutoScroll(){
    return autoScroll;
}

void ComponentCamera::setAutoScroll(bool autoScroll) {
    ComponentCamera::autoScroll = autoScroll;
}





