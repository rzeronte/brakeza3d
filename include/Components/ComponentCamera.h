
#ifndef SDL2_3D_ENGINE_CAMERA_H
#define SDL2_3D_ENGINE_CAMERA_H

#include "../3D/Object3D.h"
#include "Component.h"
#include "Camera3D.h"
#include <glm/vec3.hpp>

typedef float vec3_t[3];

class ComponentCamera : public Component
{
    Camera3D *camera = nullptr;
    glm::mat4 ViewMatrix = glm::mat4(1.0f);
    glm::mat4 ProjectionMatrix = glm::mat4(1.0f);
public:
    ComponentCamera() = default;

    void onStart() override;
    void preUpdate() override;
    void onUpdate() override;
    void postUpdate() override;
    void onEnd() override;
    void onSDLPollEvent(SDL_Event *event, bool &finish) override;
    void ResetVelocityFrame() const;
    void setViewMatrix(const glm::mat4 &viewMatrix);
    void setProjectionMatrix(const glm::mat4 &projectionMatrix);
    [[nodiscard]] const glm::mat4 &getGLMMat4ViewMatrix() const             { return ViewMatrix; }
    [[nodiscard]] const glm::mat4 &getGLMMat4ProjectionMatrix() const       { return ProjectionMatrix; }
    [[nodiscard]] Camera3D *getCamera() const                               { return camera; }

    // Projects a world-space position to screen pixels.
    // Returns glm::vec3 where x,y = screen coords, z > 0 means visible (in front of camera).
    // Pass viewport width and height from ComponentWindow::getWidthRender/getHeightRender.
    [[nodiscard]] glm::vec3 worldToScreen(Vertex3D worldPos, int viewportW, int viewportH) const {
        glm::vec4 clip = ProjectionMatrix * ViewMatrix * glm::vec4(worldPos.x, worldPos.y, worldPos.z, 1.0f);
        if (clip.w == 0.0f) return glm::vec3(0, 0, -1);
        glm::vec3 ndc = glm::vec3(clip) / clip.w;
        float sx = (ndc.x + 1.0f) * 0.5f * static_cast<float>(viewportW);
        float sy = (1.0f - ndc.y) * 0.5f * static_cast<float>(viewportH);
        return glm::vec3(sx, sy, clip.w);
    }
};


#endif //SDL2_3D_ENGINE_CAMERA_H
