#ifndef BRAKEDA3D_CAMERA3D_H
#define BRAKEDA3D_CAMERA3D_H

#include <glm/ext/matrix_float4x4.hpp>
#include "../Render/Frustum.h"
#include "../Objects/Vector3D.h"

class Camera3D : public Object3D {

    float speed = 0.f;
    float strafe = 0.f;
    float jump = 0.f;

    float pitch = 0.f;
    float yaw = 0.f;
    float roll = 0.f;

    Frustum *frustum = nullptr;

    Vector3D velocity;

    glm::mat4 ViewMatrix = glm::mat4(1);
    glm::mat4 ProjectionMatrix = glm::mat4(1);
public:
    Camera3D();
    void UpdateVelocity();
    void UpdatePositionForVelocity();
    void Pitch(float newPitch);
    void Yaw(float value);
    void MoveVertical(float v);
    void MoveForward(float v);
    void MoveBackward(float v);
    void StrafeRight();
    void StrafeLeft();
    void limitPitch();
    void setRotationFromEulerAngles(float x, float y, float z);
    float &getYaw();
    float &getPitch();
    float &getRoll();
    [[nodiscard]] Frustum *getFrustum() const;
    [[nodiscard]] glm::mat4 getGLMMat4ViewMatrix();
    [[nodiscard]] static glm::mat4 getGLMMat4ProjectionMatrix() ;
    Vector3D &getVelocity();
    M3 getM3ViewMatrix();
    static M3 getM3ProjectionMatrix();
    static void setFOV(float v);
};


#endif //BRAKEDA3D_CAMERA3D_H
