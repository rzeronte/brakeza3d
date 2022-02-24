#ifndef BRAKEDA3D_CAMERA3D_H
#define BRAKEDA3D_CAMERA3D_H


#include <bullet/BulletCollision/CollisionDispatch/btGhostObject.h>
#include <bullet/BulletDynamics/Character/btKinematicCharacterController.h>
#include "../Render/Frustum.h"
#include "../Objects/Vector3D.h"

class Camera3D : public Object3D {
public:
    float yaw = 0;        // Direction of travel
    float pitch = 0;        // Neck angle
    float roll = 0;

    float speed;        // Speed along heading
    float strafe;       // Speed along heading
    float jump;         // Speed along vertical

    Frustum *frustum;

    Vector3D velocity;

    Object3D *follow_to = nullptr;
    Vertex3D followToPositionOffset;

    btPairCachingGhostObject *m_ghostObject{};

    Object3D *getFollowTo() const;

    void setFollowTo(Object3D *followTo);

    Camera3D();

    void updateFrustum();

    void UpdateVelocity();

    void UpdatePositionForVelocity();

    void UpdateRotation(void);

    void Pitch(float newPitch);

    void Yaw(float newYaw);

    void PitchUp(void);

    void PitchDown(void);

    void MoveForward(void);

    void MoveBackward(void);

    void TurnRight(void);

    void TurnLeft(void);

    void StrafeRight(void);

    void StrafeLeft(void);

    void limitPitch();

    void makeKineticCharacter(const btTransform& transform, btConvexShape *capsule);

    void consoleInfo() const;

    btPairCachingGhostObject *getGhostObject() const;

    void setRotationFromEulerAngles(float x, float y, float z);

    void setFollowToPositionOffset(Vertex3D v);
};


#endif //BRAKEDA3D_CAMERA3D_H
