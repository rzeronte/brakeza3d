
#ifndef SDL2_3D_ENGINE_CAMERA_H
#define SDL2_3D_ENGINE_CAMERA_H

#include <SDL_surface.h>
#include "Vertex3D.h"
#include "../Render/Frustum.h"
#include "Object3D.h"
#include "../Render/EngineSetup.h"
#include "../Render/M3.h"

typedef float vec3_t[3];

class Camera3D : public Object3D {
public:
    float yaw;			// Direction of travel
    float pitch;		// Neck angle

    float speed;		// Speed along heading
    float strafe;       // Speed along heading

    vec3_t head;		// Position of head
    vec3_t view;      // Normal along viewing direction

    float horizontal_fov;

    float farDistance ;
    float aspectRatio;

    Frustum *frustum;

    Camera3D();

    float calcCanvasNearWidth();
    float calcCanvasNearHeight();
    float getScreenAspectRatio();
    float calcCanvasFarWidth();
    float calcCanvasFarHeight();
    float getNearDistance();
    float getVerticalFOV();
    void syncFrustum();

    void consoleInfo();

    void UpdatePosition(void);
    void UpdateRotation(float, float);

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

    void limitYaw();
    void limitPitch();
};


#endif //SDL2_3D_ENGINE_CAMERA_H
