
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
#define TURN_SPEED			3.0
#define PITCH_SPEED			1.0
#define STRAFE_SPEED		5.0
#define MOUSE_SENSITIVITY	0.3

Camera3D::Camera3D()
{
    this->pixelPerUnit = 10000.f;

    // Establecemos el FOV horizontal, el FOV vertical va en función del ratio y la nearDistance
    horizontal_fov = 90;
    aspectRatio = ( (float) EngineSetup::getInstance()->SCREEN_HEIGHT / (float) EngineSetup::getInstance()->SCREEN_WIDTH);
    farDistance = 10000;

    this->consoleInfo();

    // Inicializamos el frusutm que acompañará a la cámara
    frustum = new Frustum();
    frustum->setup(
        Vertex3D(head[0], head[1], head[2]),
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

    frustum->position  = Vertex3D(head[0], head[1], head[2]);
    frustum->direction = this->axis_eye;  //this->eyeVector();
    frustum->up        = this->axis_up;   // this->upVector();
    frustum->right     = this->axis_side; //this->rightVector();

    frustum->direction = this->eyeVector();
    frustum->up        = this->upVector();
    frustum->right     = this->rightVector();

    frustum->direction.consoleInfo("direction", false);
    frustum->up.consoleInfo("up", false);
    frustum->right.consoleInfo("right", true);

    frustum->updateCenters();
    frustum->updatePoints();
    frustum->updatePlanes();
}

Vertex3D Camera3D::rightVector()
{
    M3 MRX = M3::RX(pitch);
    M3 MRY = M3::RY(yaw);

    Vertex3D v = (MRY * MRX) * EngineSetup::getInstance()->right;
    v.z = -v.z;

    return v.getNormalize();
}

Vertex3D Camera3D::upVector()
{
    M3 MRX = M3::RX(pitch);
    M3 MRY = M3::RY(yaw);

    Vertex3D v = (MRY * MRX) * EngineSetup::getInstance()->up;
    v.z = -v.z;

    return v.getNormalize();
}

Vertex3D Camera3D::eyeVector()
{
    M3 MRX = M3::RX(pitch);
    M3 MRY = M3::RY(yaw);

    Vertex3D v = (MRY * MRX) * EngineSetup::getInstance()->eye;
    v.z = -v.z;

    return v.getNormalize();
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

bool Camera3D::isFront()
{
    Vertex3D oRight = this->rightVector();
    Vertex3D R = EngineSetup::getInstance()->right;

    float rads = Vertex3D::dotProduct(R, oRight) / (R.getNorm() * oRight.getNorm());

    if (rads < 0) {
        return false;
    }

    return true;
}

void Camera3D::Pitch(float pitch)
{
    this->pitch -= pitch * MOUSE_SENSITIVITY;
    if (pitch > 90.0f) {
        pitch = 90.0f;
    } else if(pitch < -90.0f) {
        pitch = -90.0f;
    }
}

void Camera3D::Yaw(float yaw)
{
    this->yaw -= yaw * MOUSE_SENSITIVITY;
    if (yaw < 0.0f) {
        yaw += 360.0f;
    } else if(yaw > 360.0f) {
        yaw -= 360.0f;
    }
}

void Camera3D::PitchUp(void)
{
    if (pitch < 89.0) {
        pitch += PITCH_SPEED;
    }
}

void Camera3D::PitchDown(void)
{
    if (pitch > -89.0) {
        pitch -= PITCH_SPEED;
    }
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
        head[0] += speed * cos(getYaw() * M_PI / 180.0);
        head[1] += speed * sin(getYaw() * M_PI / 180.0);
        head[2] += speed * sin(getPitch() * M_PI / 180.0);
    }

    // Move the camera sideways
    if ((fabs(strafe) > 0)) {
        head[0] += strafe * sin(getYaw() * M_PI / 180.0);
        head[1] -= strafe * cos(getYaw() * M_PI / 180.0);;
    }

    // Setup the view vector
    view[0] = cosf(getYaw() * M_PI / 180.0);
    view[1] = sinf(getYaw() * M_PI / 180.0);
    view[2] = sinf(getPitch() * M_PI / 180.0);

    this->CreateLookAt(
        Vertex3D(head[0], head[1], head[2]),
        Vertex3D(0, 0, 0),
        EngineSetup::getInstance()->up
    );

    // Reset speed
    speed = 0;
    strafe = 0;
}

void Camera3D::CreateLookAt(Vertex3D eye, Vertex3D target, Vertex3D up)
{
    Vertex3D forward = target;
    forward.subVertex(eye);
    forward = forward.getNormalize();

    Vertex3D side = Maths::crossProduct(forward, up);
    side = side.getNormalize();

    up = Maths::crossProduct(side, forward);
    up = up.getNormalize();

    this->axis_eye  = forward;
    this->axis_side = side;
    this->axis_up   = up;

    this->axis_eye.consoleInfo("axis_eye", false);
    this->axis_side.consoleInfo("axis_side", false);
    this->axis_up.consoleInfo("axis_up", true);

    M3 m = M3::MatrixIdentity();

    // Defaults to identity matrix
    /*
    m.m11 = side.x;		m.m12 = up.x;		m.m13 = -forward.x;
    m.m21 = side.y;		m.m22 = up.y;		m.m23 = -forward.y;
    m.m31 = side.z;		m.m32 = up.z;		m.m33 = -forward.z;

    m.m14 = -eye.x;
    m.m24 = -eye.y;
    m.m34 = -eye.z;
    */
}

float Camera3D::getYaw()
{
    return yaw;
}

float Camera3D::getPitch()
{
    return pitch;
}
