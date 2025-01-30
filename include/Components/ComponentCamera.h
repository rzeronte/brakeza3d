
#ifndef SDL2_3D_ENGINE_CAMERA_H
#define SDL2_3D_ENGINE_CAMERA_H

#include "../Objects/Vertex3D.h"
#include "../Objects/Object3D.h"
#include "../Render/Frustum.h"
#include "../EngineSetup.h"
#include "../Render/M3.h"
#include "Component.h"
#include "Camera3D.h"
#include "ComponentCollisions.h"
#include <BulletDynamics/Character/btKinematicCharacterController.h>
#include <BulletCollision/CollisionShapes/btCapsuleShape.h>
#include <btBulletDynamicsCommon.h>
#include <btBulletCollisionCommon.h>

typedef float vec3_t[3];

class ComponentCamera : public Component {
private:
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

    [[nodiscard]] Camera3D *getCamera() const;

    const glm::mat4 &getGLMMat4ViewMatrix() const;

    void setViewMatrix(const glm::mat4 &viewMatrix);

    const glm::mat4 &getGLMMat4ProjectionMatrix() const;

    void setProjectionMatrix(const glm::mat4 &projectionMatrix);
};


#endif //SDL2_3D_ENGINE_CAMERA_H
