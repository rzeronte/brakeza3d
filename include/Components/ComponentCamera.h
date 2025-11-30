
#ifndef SDL2_3D_ENGINE_CAMERA_H
#define SDL2_3D_ENGINE_CAMERA_H

#include "../3D/Object3D.h"
#include "Component.h"
#include "Camera3D.h"

typedef float vec3_t[3];

class ComponentCamera : public Component
{
    Camera3D *camera;
    glm::mat4 ViewMatrix;
    glm::mat4 ProjectionMatrix;
public:
    ComponentCamera();

    void onStart() override;
    void preUpdate() override;
    void onUpdate() override;
    void postUpdate() override;
    void onEnd() override;
    void onSDLPollEvent(SDL_Event *event, bool &finish) override;
    void setViewMatrix(const glm::mat4 &viewMatrix);
    void setProjectionMatrix(const glm::mat4 &projectionMatrix);
    const glm::mat4 &getGLMMat4ViewMatrix() const;
    const glm::mat4 &getGLMMat4ProjectionMatrix() const;
    [[nodiscard]] Camera3D *getCamera() const;
};


#endif //SDL2_3D_ENGINE_CAMERA_H
