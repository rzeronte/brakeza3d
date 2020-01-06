
#ifndef SDL2_3D_ENGINE_CAMERA_H
#define SDL2_3D_ENGINE_CAMERA_H

#include <SDL_surface.h>
#include "Vertex3D.h"
#include "Object3D.h"
#include "../Render/Frustum.h"
#include "../EngineSetup.h"
#include "../Render/M3.h"
#include <BulletDynamics/Character/btKinematicCharacterController.h>
#include <BulletCollision/CollisionShapes/btCapsuleShape.h>

typedef float vec3_t[3];

class Camera3D : public Object3D {
private:
public:
    float yaw   = 0;			// Direction of travel
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
    Vector3D oldVelocity;

    btPairCachingGhostObject *m_ghostObject;
    btKinematicCharacterController* kinematicController;

    Camera3D();

    float calcCanvasNearWidth();
    float calcCanvasNearHeight();
    float getScreenAspectRatio();
    float calcCanvasFarWidth();
    float calcCanvasFarHeight();
    float getNearDistance();
    float getVerticalFOV();

    void UpdateFrustum();
    void UpdateVelocity(void);
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

    void makeKineticCharacter(btTransform transform, btConvexShape *capsule);

    void Jump();
    void limitPitch();

    void consoleInfo();

};


#endif //SDL2_3D_ENGINE_CAMERA_H
