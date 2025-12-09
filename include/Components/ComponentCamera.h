
#ifndef SDL2_3D_ENGINE_CAMERA_H
#define SDL2_3D_ENGINE_CAMERA_H

#include "../3D/Object3D.h"
#include "Component.h"
#include "Camera3D.h"

typedef float vec3_t[3];

class ComponentCamera : public Component
{
    Camera3D *camera = nullptr;
    glm::mat4 ViewMatrix;
    glm::mat4 ProjectionMatrix;
public:
    ComponentCamera() = default;

    void onStart() override;
    void preUpdate() override;
    void onUpdate() override;
    void postUpdate() override;
    void onEnd() override;
    void onSDLPollEvent(SDL_Event *event, bool &finish) override;
    void setViewMatrix(const glm::mat4 &viewMatrix);
    void setProjectionMatrix(const glm::mat4 &projectionMatrix);
    const glm::mat4 &getGLMMat4ViewMatrix() const                   { return ViewMatrix; }
    const glm::mat4 &getGLMMat4ProjectionMatrix() const             { return ProjectionMatrix; }
    [[nodiscard]] Camera3D *getCamera() const                       { return camera; }
};


#endif //SDL2_3D_ENGINE_CAMERA_H
