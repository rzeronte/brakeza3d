
#include "../../include/Components/ComponentCamera.h"
#include "../../include/Render/Logging.h"
#include "../../include/ComponentsManager.h"

ComponentCamera::ComponentCamera()
:
    camera(new Camera3D()),
    autoScroll(false)
{
}

void ComponentCamera::onStart()
{
    Logging::head("ComponentCamera onStart");
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
    if (ComponentsManager::get()->getComponentInput()->isEnabled()) {
        getCamera()->UpdatePositionForVelocity();
    }
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
