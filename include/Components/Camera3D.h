#ifndef BRAKEDA3D_CAMERA3D_H
#define BRAKEDA3D_CAMERA3D_H


#include <bullet/BulletCollision/CollisionDispatch/btGhostObject.h>
#include <bullet/BulletDynamics/Character/btKinematicCharacterController.h>
#include <glm/ext/matrix_float4x4.hpp>
#include "../Render/Frustum.h"
#include "../Objects/Vector3D.h"

class Camera3D : public Object3D {
private:

    float speed;        // Speed along heading
    float strafe;       // Speed along heading
    float jump;         // Speed along vertical

    Frustum *frustum;

    Vector3D velocity;

    glm::mat4 ViewMatrix;
    glm::mat4 ProjectionMatrix;

public:

    Camera3D();

    void UpdateVelocity();

    void UpdatePositionForVelocity();

    void Pitch(float newPitch);

    void Yaw(float newYaw);

    void PitchUp();

    void PitchDown();

    void MoveForward();

    void MoveBackward();

    void TurnRight();

    void TurnLeft();

    void StrafeRight();

    void StrafeLeft();

    void limitPitch();

    void setRotationFromEulerAngles(float x, float y, float z);

    [[nodiscard]] Frustum *getFrustum() const;

    float &getYaw();

    float &getPitch();

    float &getRoll();

    Vector3D &getVelocity();

    float pitch = 0;
    float yaw = 0;
    float roll = 0;

    [[nodiscard]] glm::mat4 getViewMatrix();

    [[nodiscard]] static glm::mat4 getProjectionMatrix() ;
};


#endif //BRAKEDA3D_CAMERA3D_H
