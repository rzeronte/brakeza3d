
#ifndef SDL2_3D_ENGINE_CAMERA_H
#define SDL2_3D_ENGINE_CAMERA_H

#include <SDL_surface.h>
#include <BulletDynamics/Character/btKinematicCharacterController.h>
#include "Vertex3D.h"
#include "../Render/Frustum.h"
#include "Object3D.h"
#include "../Render/EngineSetup.h"
#include "../Render/M3.h"
#include "../../src/Collider.h"

typedef float vec3_t[3];

class Camera3D : public Object3D {
private:
public:
    float yaw   = 0;			// Direction of travel
    float pitch = 0;		// Neck angle
    float roll  = 0;

    float head_vertical = 180;

    float speed;		// Speed along heading
    float strafe;       // Speed along heading
    float jump;         // Speed along vertical

    float horizontal_fov;
    float farDistance ;
    float aspectRatio;

    Frustum *frustum;

    Collider *collider;

    btPairCachingGhostObject *m_ghostObject;
    btKinematicCharacterController* charCon;
    btConvexShape* capsule;

    Camera3D();

    float calcCanvasNearWidth();
    float calcCanvasNearHeight();
    float getScreenAspectRatio();
    float calcCanvasFarWidth();
    float calcCanvasFarHeight();
    float getNearDistance();
    float getVerticalFOV();

    void UpdateFrustum();
    void UpdateColliderForceMovement(void);
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

    void Jump();
    void limitPitch();

    void HeadBob();

    void consoleInfo();

};


#endif //SDL2_3D_ENGINE_CAMERA_H
