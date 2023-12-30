#ifndef BRAKEDA3D_CAMERA3D_H
#define BRAKEDA3D_CAMERA3D_H


#include <bullet/BulletCollision/CollisionDispatch/btGhostObject.h>
#include <bullet/BulletDynamics/Character/btKinematicCharacterController.h>
#include <ext/matrix_float4x4.hpp>
#include "../Render/Frustum.h"
#include "../Objects/Vector3D.h"

class Camera3D : public Object3D {
private:

    float speed;        // Speed along heading
    float strafe;       // Speed along heading
    float jump;         // Speed along vertical

    Frustum *frustum;

    Vector3D velocity;

    Object3D *follow_to = nullptr;
    Vertex3D followToPositionOffset;

    btPairCachingGhostObject *m_ghostObject;

    glm::mat4 ViewMatrix;
    glm::mat4 ProjectionMatrix;

public:

    [[nodiscard]] Object3D *getFollowTo() const;

    void setFollowTo(Object3D *followTo);

    Camera3D();

    void UpdateVelocity();

    void UpdatePositionForVelocity();

    void UpdateRotation();

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

    void makeKineticCharacter(const btTransform& transform, btConvexShape *capsule);

    [[nodiscard]] btPairCachingGhostObject *getGhostObject() const;

    void setRotationFromEulerAngles(float x, float y, float z);

    void setFollowToPositionOffset(Vertex3D v);

    [[nodiscard]] Frustum *getFrustum() const;

    float &getYaw();

    float &getPitch();

    float &getRoll();

    Vector3D &getVelocity();

    [[nodiscard]] const Vertex3D &getFollowToPositionOffset() const;

    float pitch = 0;
    float yaw = 0;
    float roll = 0;

    [[nodiscard]] glm::mat4 getViewMatrix();

    [[nodiscard]] static glm::mat4 getProjectionMatrix() ;
};


#endif //BRAKEDA3D_CAMERA3D_H
