
#include "../../include/Components/ComponentCamera.h"
#include "../../include/Render/Logging.h"
#include "../../include/ComponentsManager.h"

ComponentCamera::ComponentCamera()
:
    camera(new Camera3D())
{
}

void ComponentCamera::onStart()
{
    Logging::Message("ComponentCamera onStart");
}

void ComponentCamera::preUpdate()
{
    getCamera()->getVelocity().vertex1 = getCamera()->getPosition();
    getCamera()->getVelocity().vertex2 = getCamera()->getPosition();
}

void ComponentCamera::onUpdate()
{
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