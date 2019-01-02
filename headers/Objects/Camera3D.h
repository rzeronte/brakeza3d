
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
private:
    bool isInCollision = false;
public:
    float yaw;			// Direction of travel
    float pitch;		// Neck angle

    float speed;		// Speed along heading
    float strafe;       // Speed along heading

    float horizontal_fov;
    float farDistance ;
    float aspectRatio;

    Frustum *frustum;

    // Camera velocity vector (for collision detection)
    Vertex3D V1;
    Vertex3D V2;

    bool isIsInCollision() const;
    void setIsInCollision(bool isInCollision);

    Camera3D();

    float calcCanvasNearWidth();
    float calcCanvasNearHeight();
    float getScreenAspectRatio();
    float calcCanvasFarWidth();
    float calcCanvasFarHeight();
    float getNearDistance();
    float getVerticalFOV();

    void UpdateFrustum();
    void UpdatePosition(void);
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

    void consoleInfo();

    Vertex3D getVelocity();
};


#endif //SDL2_3D_ENGINE_CAMERA_H
