
#ifndef SDL2_3D_ENGINE_CAMERA_H
#define SDL2_3D_ENGINE_CAMERA_H

#include <SDL_surface.h>
#include "Vertex3D.h"
#include "../Render/Frustum.h"
#include "Object3D.h"
#include "../Render/EngineSetup.h"

typedef float vec3_t[3];

class Camera3D {
private:
    float yaw;			// Direction of travel
    float pitch;		// Neck angle

public:
    float speed;		// Speed along heading
    float strafe;       // Speed along heading

    Vertex3D axis_up;
    Vertex3D axis_side;
    Vertex3D axis_eye;

    vec3_t head;		// Position of head
    vec3_t view;      // Normal along viewing direction

    float pixelPerUnit;
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

    Vertex3D eyeVector();
    Vertex3D upVector();
    Vertex3D rightVector();

    void consoleInfo();

    bool isFront();

    void UpdatePosition(void);
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
    void CreateLookAt(Vertex3D eye, Vertex3D target, Vertex3D up);

    float getYaw();

    float getPitch();
};


#endif //SDL2_3D_ENGINE_CAMERA_H
