
#include "../../headers/Components/ComponentCamera.h"
#include "../../headers/ComponentsManager.h"

ComponentCamera::ComponentCamera()
{
    this->camera = new Camera3D();
}

void ComponentCamera::onStart() {
    std::cout << "ComponentCamera onStart" << std::endl;
}

void ComponentCamera::preUpdate()
{
    getCamera()->velocity.vertex1 = *getCamera()->getPosition();
    getCamera()->velocity.vertex2 = *getCamera()->getPosition();
}

void ComponentCamera::onUpdate() {

    float reduction = 0;
    bool  allowVertical = false;

    if (ComponentsManager::get()->getComponentBSP()->getBsp()->isLoaded()) {
        if (ComponentsManager::get()->getComponentBSP()->getBsp()->isCurrentLeafLiquid()) {
            reduction = EngineSetup::getInstance()->WALKING_SPEED_LIQUID_DIVISOR;
            allowVertical = true;
        }
    }
    getCamera()->UpdateVelocity( reduction, allowVertical );

}

void ComponentCamera::postUpdate()
{
    getCamera()->setPosition( ComponentsManager::get()->getComponentCollisions()->finalVelocity );

    this->getCamera()->UpdateRotation();
    this->getCamera()->UpdateFrustum();

}

void ComponentCamera::onEnd() {

}

void ComponentCamera::onSDLPollEvent(SDL_Event *e, bool &finish) {
}

Camera3D *ComponentCamera::getCamera() const {
    return camera;
}



