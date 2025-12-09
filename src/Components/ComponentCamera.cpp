
#include "../../include/Components/ComponentCamera.h"
#include "../../include/Components/Components.h"

void ComponentCamera::onStart()
{
    Component::onStart();
    camera = new Camera3D();
}

void ComponentCamera::preUpdate()
{
    Component::preUpdate();

    camera->getVelocity().vertex1 = camera->getPosition();
    camera->getVelocity().vertex2 = camera->getPosition();

    setProjectionMatrix(Camera3D::getGLMMat4ProjectionMatrix());
    setViewMatrix(camera->getGLMMat4ViewMatrix());
}

void ComponentCamera::onUpdate()
{
    Component::onUpdate();
    camera->UpdateVelocity();
}

void ComponentCamera::postUpdate()
{
    Component::postUpdate();

    if (Components::get()->Input()->isEnabled()) {
        camera->UpdatePositionForVelocity();
    }
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