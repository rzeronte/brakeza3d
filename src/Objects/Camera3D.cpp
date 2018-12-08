
#include <iostream>
#include "../../headers/Objects/Camera3D.h"
#include "../../headers/Render/Tools.h"
#include "../../headers/Objects/Line2D.h"
#include "../../headers/Render/Color.h"
#include "../../headers/Render/EngineSetup.h"
#include "../../headers/Objects/Vector3D.h"
#include "../../headers/Render/Drawable.h"
#include "../../headers/Render/Transforms.h"
#include "../../headers/Render/M3.h"
#include "../../headers/Render/Maths.h"
#include "../../headers/Render/Logging.h"

#define WALKING_SPEED		5.0
#define TURN_SPEED			2.0
#define PITCH_SPEED			1.0
#define STRAFE_SPEED		5.0
#define MOUSE_SENSITIVITY	0.3

Camera3D::Camera3D()
{
    // Establecemos el FOV horizontal, el FOV vertical va en función del ratio y la nearDistance
    horizontal_fov = 90;
    aspectRatio = ( (float) EngineSetup::getInstance()->SCREEN_HEIGHT / (float) EngineSetup::getInstance()->SCREEN_WIDTH);
    farDistance = 10000;

    this->consoleInfo();

    // Inicializamos el frusutm que acompañará a la cámara
    frustum = new Frustum();
    frustum->setup(
        *getPosition(),
        Vertex3D(0, 0, 1),
        EngineSetup::getInstance()->up,
        EngineSetup::getInstance()->right,
        getNearDistance(),
        calcCanvasNearHeight(), calcCanvasNearWidth(),
        farDistance,
        calcCanvasFarHeight(), calcCanvasFarWidth()
    );
}

float Camera3D::getNearDistance()
{
    return (1 / tanf( Maths::degreesToRadians(this->horizontal_fov/2) ));
}

float Camera3D::getVerticalFOV()
{
    float vfov = 2 * atanf( getScreenAspectRatio() / getNearDistance() );

    return Maths::radiansToDegrees( vfov );
}

float Camera3D::calcCanvasNearWidth()
{
    float width = ( 2 * tanf( Maths::degreesToRadians(horizontal_fov / 2) ) * getNearDistance() ) ;

    return width;
}

float Camera3D::calcCanvasNearHeight()
{
    float height = ( 2 * tanf( Maths::degreesToRadians( getVerticalFOV() / 2) ) * getNearDistance() ) * getScreenAspectRatio();

    return height;
}

float Camera3D::calcCanvasFarWidth()
{
    float width = ( 2 * tanf( Maths::degreesToRadians(horizontal_fov / 2) ) * farDistance) ;

    return width;
}

float Camera3D::calcCanvasFarHeight()
{
    float height = (2 * tanf( Maths::degreesToRadians( getVerticalFOV() / 2) ) * farDistance) * getScreenAspectRatio();

    return height;
}

float Camera3D::getScreenAspectRatio()
{
    return this->aspectRatio;
}

void Camera3D::syncFrustum()
{
    frustum->position  = *getPosition();

    frustum->direction = this->getRotation().getTranspose() * EngineSetup::getInstance()->forward;
    frustum->up        = this->getRotation().getTranspose() * EngineSetup::getInstance()->up;
    frustum->right     = this->getRotation().getTranspose() * EngineSetup::getInstance()->right;

    frustum->updateCenters();
    frustum->updatePoints();
    frustum->updatePlanes();
}

void Camera3D::consoleInfo()
{
    printf("Camera Aspect Ratio: %f | HFOV: %f | VFOV: %f | NearDistance: %f | Canvas W: %f | Canvas H: %f\r\n",
        aspectRatio,
        horizontal_fov,
        getVerticalFOV(),
        getNearDistance(),
        calcCanvasNearWidth(), calcCanvasNearHeight()
    );
}

void Camera3D::Pitch(float pitch)
{
    this->pitch += pitch * MOUSE_SENSITIVITY;
    limitPitch();
}

void Camera3D::Yaw(float yaw)
{
    this->yaw -= yaw * MOUSE_SENSITIVITY;
}

void Camera3D::PitchUp(void)
{
    pitch += PITCH_SPEED;
    limitPitch();
}

void Camera3D::PitchDown(void)
{
    pitch -= PITCH_SPEED;
    limitPitch();
}

void Camera3D::MoveForward(void)
{
    speed += WALKING_SPEED;
}

void Camera3D::MoveBackward(void)
{
    speed -= WALKING_SPEED;
}

void Camera3D::TurnRight(void)
{
    yaw -= TURN_SPEED;
}

void Camera3D::TurnLeft(void)
{
    yaw += TURN_SPEED;
}

void Camera3D::StrafeRight(void)
{
    strafe += STRAFE_SPEED;
}

void Camera3D::StrafeLeft(void)
{
    strafe -= STRAFE_SPEED;
}

void Camera3D::UpdatePosition(void)
{
    // Move the camera forward
    if ((fabs(speed) > 0)) {
        getPosition()->z+=speed;
    }

    // Move the camera sideways
    if ((fabs(strafe) > 0)) {
        getPosition()->x-=speed;
    }

    // Reset speed
    speed  = 0;
    strafe = 0;

}

void Camera3D::UpdateRotation()
{
    M3 im = M3::getMatrixRotationForEulerAngles(this->pitch, this->yaw, 0);

    this->setRotation(im);
}


void Camera3D::limitPitch()
{
    if (this->pitch >= 89) {
        this->pitch = 89;
    }
    if (this->pitch <= -89) {
        this->pitch = -89;
    }
}