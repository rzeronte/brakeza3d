//
// Created by darkhead on 11/1/20.
//

#ifndef BRAKEDA3D_CAMERA3D_H
#define BRAKEDA3D_CAMERA3D_H


#include <BulletCollision/CollisionDispatch/btGhostObject.h>
#include <BulletDynamics/Character/btKinematicCharacterController.h>
#include "../Render/Frustum.h"
#include "../Objects/Vector3D.h"

class Camera3D : public Object3D {
public:
    float yaw   = 0;	    // Direction of travel
    float pitch = 0;		// Neck angle
    float roll  = 0;

    float speed;		// Speed along heading
    float strafe;       // Speed along heading
    float jump;         // Speed along vertical

    float horizontal_fov;
    float farDistance ;
    float aspectRatio;

    Frustum *frustum;

    Vector3D velocity;

    Object3D *follow_to = nullptr;
    btPairCachingGhostObject *m_ghostObject;

    Object3D *getFollowTo() const;

    void setFollowTo(Object3D *followTo);

    Camera3D();

    float calcCanvasNearWidth();
    float calcCanvasNearHeight();
    float getScreenAspectRatio();
    float calcCanvasFarWidth();
    float calcCanvasFarHeight();
    float getNearDistance();
    float getVerticalFOV();

    void UpdateFrustum();
    void UpdateVelocity();
    void UpdatePositionForVelocity();
    void UpdateRotation(void);

    void Pitch(float pitch);
    void Yaw(float yaw);
    void PitchUp(void);
    void PitchDown(void);
    void MoveForward(void);
    void MoveBackward(void);
    void TurnRight(void);
    void TurnLeft(void);
    void StrafeRight(void);
    void StrafeLeft(void);

    void limitPitch();

    void makeKineticCharacter(btTransform transform, btConvexShape *capsule);

    void consoleInfo();

    btPairCachingGhostObject* getGhostObject();
};


#endif //BRAKEDA3D_CAMERA3D_H
