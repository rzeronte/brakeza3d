
#include "../../include/Components/ComponentCamera.h"
#include "../../include/ComponentsManager.h"
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
    getCamera()->UpdateVelocity();
}

void ComponentCamera::postUpdate() {
    if (this->is_fly_mode) {
        getCamera()->UpdateRotation();
        getCamera()->UpdatePositionForVelocity();
    }

    this->getCamera()->UpdateFrustum();
}

void ComponentCamera::onEnd() {

}

void ComponentCamera::onSDLPollEvent(SDL_Event *e, bool &finish) {
}

Camera3D *ComponentCamera::getCamera() const {
    return camera;
}

bool ComponentCamera::isFlyMode() const {
    return is_fly_mode;
}

void ComponentCamera::setIsFlyMode(bool isFlyMode) {
    is_fly_mode = isFlyMode;
}