
#include "../../include/Components/ComponentCamera.h"
#include "../../include/Components/Components.h"

void ComponentCamera::onStart()
{
    Component::onStart();
    camera = new Camera3D();
    setEnabled(true);
}

void ComponentCamera::preUpdate()
{
    setProjectionMatrix(Camera3D::getGLMMat4ProjectionMatrix());
    setViewMatrix(camera->getGLMMat4ViewMatrix());

    if (!isEnabled()) return;

    ResetVelocityFrame();
}

void ComponentCamera::onUpdate()
{
    if (!isEnabled()) return;

    camera->UpdateVelocity();
}

void ComponentCamera::postUpdate()
{
    if (!isEnabled()) return;

    if (Components::get()->Input()->isEnabled()) {
        camera->UpdatePositionForVelocity();
    }
}

void ComponentCamera::ResetVelocityFrame() const
{
    camera->getVelocity().vertex1 = camera->getPosition();
    camera->getVelocity().vertex2 = camera->getPosition();
}

void ComponentCamera::onEnd()
{
}

void ComponentCamera::onSDLPollEvent(SDL_Event *e, bool &finish)
{
}

void ComponentCamera::setViewMatrix(const glm::mat4 &viewMatrix)
{
    ViewMatrix = viewMatrix;
}

void ComponentCamera::setProjectionMatrix(const glm::mat4 &projectionMatrix)
{
    ProjectionMatrix = projectionMatrix;
}
