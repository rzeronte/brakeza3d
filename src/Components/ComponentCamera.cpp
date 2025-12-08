
#include "../../include/Components/ComponentCamera.h"
#include "../../include/Misc/Logging.h"
#include "../../include/Components/ComponentsManager.h"

ComponentCamera::ComponentCamera()
:
    camera(new Camera3D())
{
}

void ComponentCamera::onStart()
{
    Component::onStart();
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

    if (ComponentsManager::get()->Input()->isEnabled()) {
        camera->UpdatePositionForVelocity();
    }
}

void ComponentCamera::onEnd()
{
}

void ComponentCamera::onSDLPollEvent(SDL_Event *e, bool &finish)
{
}

Camera3D *ComponentCamera::getCamera() const
{
    return camera;
}

const glm::mat4 &ComponentCamera::getGLMMat4ViewMatrix() const
{
    return ViewMatrix;
}

void ComponentCamera::setViewMatrix(const glm::mat4 &viewMatrix)
{
    ViewMatrix = viewMatrix;
}

const glm::mat4 &ComponentCamera::getGLMMat4ProjectionMatrix() const
{
    return ProjectionMatrix;
}

void ComponentCamera::setProjectionMatrix(const glm::mat4 &projectionMatrix)
{
    ProjectionMatrix = projectionMatrix;
}