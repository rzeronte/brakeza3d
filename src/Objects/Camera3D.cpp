
#include "../../headers/Objects/Camera3D.h"
#include "../../headers/Render/Tools.h"
#include "../../headers/Render/Drawable.h"
#include "../../headers/Render/Transforms.h"
#include "../../headers/Render/Maths.h"

#include <btBulletDynamicsCommon.h>
#include "BulletCollision/CollisionDispatch/btGhostObject.h"
#include "BulletDynamics/Character/btKinematicCharacterController.h"
#include "LinearMath/btTransform.h"
#include "../../headers/Render/Engine.h"
#include "../../headers/Brakeza3D.h"

Camera3D::Camera3D()
{
    // Establecemos el FOV horizontal, el FOV vertical va en función del ratio y la nearDistance
    horizontal_fov = EngineSetup::getInstance()->HORIZONTAL_FOV;
    aspectRatio    = ((float) EngineSetup::getInstance()->screenHeight / (float) EngineSetup::getInstance()->screenWidth);
    farDistance    = EngineSetup::getInstance()->FRUSTUM_FARPLANE_DISTANCE;

    this->consoleInfo();

    // Inicializamos el frustum que acompañará a la cámara
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

    this->velocity = Vector3D( Vertex3D::zero(), Vertex3D::zero() );

    // Bullet Physics
    btTransform startTransform;
    startTransform.setIdentity();
    startTransform.setOrigin (btVector3(0, 0, 0));

    m_ghostObject = new btPairCachingGhostObject();
    m_ghostObject->setWorldTransform(startTransform);

    btConvexShape* capsule = new btCapsuleShape(1.55f, 3.5f);
    m_ghostObject->setCollisionShape(capsule);
    m_ghostObject->setUserPointer(this);

    kinematicController = new btKinematicCharacterController(m_ghostObject, capsule, 1.75f);
    setLabel(EngineSetup::getInstance()->cameraNameIdentifier);
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

void Camera3D::UpdateFrustum()
{
    frustum->position  = *getPosition();

    frustum->direction = this->getRotation().getTranspose() * EngineSetup::getInstance()->forward;
    frustum->up        = this->getRotation().getTranspose() * EngineSetup::getInstance()->up;
    frustum->right     = this->getRotation().getTranspose() * EngineSetup::getInstance()->right;

    frustum->updateCenters();
    frustum->updatePoints();
    frustum->updatePlanes();

    // Cacheamos los espacios de coordenadas de las 4 esquinas para reutilizarlos en la transformación NDC
    frustum->vNLs = Transforms::cameraSpace(frustum->near_left.vertex1, this);
    frustum->vNRs = Transforms::cameraSpace(frustum->near_right.vertex1, this);
    frustum->vNTs = Transforms::cameraSpace(frustum->near_top.vertex1, this);
    frustum->vNBs = Transforms::cameraSpace(frustum->near_bottom.vertex1, this);

    // cacheamos las coordenadas 2D de los marcos del near plane
    frustum->vNLpers = Transforms::perspectiveDivision(frustum->vNLs, this);
    frustum->vNRpers = Transforms::perspectiveDivision(frustum->vNRs, this);
    frustum->vNTpers = Transforms::perspectiveDivision(frustum->vNTs, this);
    frustum->vNBpers = Transforms::perspectiveDivision(frustum->vNBs, this);
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
    this->pitch += pitch * EngineSetup::getInstance()->MOUSE_SENSITIVITY;
    limitPitch();
}

void Camera3D::Yaw(float yaw)
{
    this->yaw -= yaw * EngineSetup::getInstance()->MOUSE_SENSITIVITY;
}

void Camera3D::PitchUp(void)
{
    pitch += EngineSetup::getInstance()->PITCH_SPEED;
    limitPitch();
}

void Camera3D::PitchDown(void)
{
    pitch -= EngineSetup::getInstance()->PITCH_SPEED;
    limitPitch();
}

void Camera3D::MoveForward(void)
{
    speed += EngineSetup::getInstance()->WALKING_SPEED;
}

void Camera3D::MoveBackward(void)
{
    speed -= EngineSetup::getInstance()->WALKING_SPEED;
}

void Camera3D::TurnRight(void)
{
    yaw -= EngineSetup::getInstance()->TURN_SPEED;
}

void Camera3D::TurnLeft(void)
{
    yaw += EngineSetup::getInstance()->TURN_SPEED;
}

void Camera3D::StrafeRight(void)
{
    strafe += EngineSetup::getInstance()->STRAFE_SPEED;

}

void Camera3D::StrafeLeft(void)
{
    strafe -= EngineSetup::getInstance()->STRAFE_SPEED;
}

void Camera3D::UpdateVelocity(void)
{
    float liquidSpeedDivisor = EngineSetup::getInstance()->WALKING_SPEED_LIQUID_DIVISOR;
    // Move the camera forward
    if (Brakeza3D::get()->getBSP()->isLoaded()) {
        if (Brakeza3D::get()->getBSP()->isCurrentLeafLiquid()) {
            speed  /= liquidSpeedDivisor;
            strafe /= liquidSpeedDivisor;
        }
    }

    if ((fabs(speed) > 0)) {
        this->velocity.vertex2.z = getPosition()->z + speed * (float) cos(-yaw * M_PI / 180.0);
        this->velocity.vertex2.x = getPosition()->x + speed * (float) sin(-yaw * M_PI / 180.0);
        if (Brakeza3D::get()->getBSP()->isLoaded()) {
            if (Brakeza3D::get()->getBSP()->isCurrentLeafLiquid()) {
                this->velocity.vertex2.y = getPosition()->y + speed * (float) sin(pitch * M_PI / 180.0); // VERTICAL
            }
        }
    }

    // Move the camera side ways
    if ((fabs(strafe) > 0)) {
        this->velocity.vertex2.z += strafe * (float) -sin(-yaw * M_PI / 180.0);
        this->velocity.vertex2.x += - strafe * (float) -cos(-yaw * M_PI / 180.0);
    }

    // Reset speed
    speed  = 0;
    strafe = 0;
    jump   = 0;
}

void Camera3D::UpdateRotation()
{
    M3 im = M3::getMatrixRotationForEulerAngles(this->pitch, this->yaw, this->roll);

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