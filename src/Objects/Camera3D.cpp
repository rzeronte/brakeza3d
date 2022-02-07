#include <BulletCollision/CollisionShapes/btCapsuleShape.h>
#include "../../include/Components/Camera3D.h"
#include "../../include/Render/Maths.h"
#include "../../include/Render/Transforms.h"
#include "../../include/EngineSetup.h"
#include "../../include/Render/Logging.h"

Camera3D::Camera3D() {

    // Inicializamos el frustum que acompañará a la cámara
    frustum = new Frustum();
    frustum->setParent(this);
    frustum->setup(
        this->getPosition(),
        Vertex3D(0, 0, 1),
        EngineSetup::get()->up,
        EngineSetup::get()->right,
        EngineSetup::get()->HORIZONTAL_FOV,
        ((float) EngineSetup::get()->screenHeight / (float) EngineSetup::get()->screenWidth),
        EngineSetup::get()->FRUSTUM_FARPLANE_DISTANCE
    );

    btConvexShape *capsule = new btCapsuleShapeZ(
            EngineSetup::get()->PLAYER_CAPSULE_RADIUS,
        EngineSetup::get()->PLAYER_CAPSULE_HEIGHT
    );

    btTransform startTransform;
    startTransform.setIdentity();
    startTransform.setOrigin(btVector3(0, 0, 0));

    this->makeKineticCharacter(startTransform, capsule);

    this->setLabel(EngineSetup::get()->cameraNameIdentifier);

    this->followToPositionOffset = Vertex3D(10, -15, 0);

    this->consoleInfo();
}

void Camera3D::updateFrustum() {
    frustum->setPosition( this->getPosition() );
    frustum->setRotation( this->getRotation() );

    frustum->direction = this->getRotation().getTranspose() * EngineSetup::get()->forward;
    frustum->up = this->getRotation().getTranspose() * EngineSetup::get()->up;
    frustum->right = this->getRotation().getTranspose() * EngineSetup::get()->right;

    frustum->updateFrustum();
}

void Camera3D::consoleInfo() const {
    this->frustum->consoleInfo();
}

void Camera3D::Pitch(float newPitch) {
    this->pitch += newPitch * EngineSetup::get()->MOUSE_SENSITIVITY;
    limitPitch();
}

void Camera3D::Yaw(float newYaw) {
    this->yaw -= newYaw * EngineSetup::get()->MOUSE_SENSITIVITY;
}

void Camera3D::PitchUp() {
    pitch += EngineSetup::get()->PITCH_SPEED;
    limitPitch();
}

void Camera3D::PitchDown() {
    pitch -= EngineSetup::get()->PITCH_SPEED;
    limitPitch();
}

void Camera3D::MoveForward() {
    speed += EngineSetup::get()->WALKING_SPEED;
}

void Camera3D::MoveBackward() {
    speed -= EngineSetup::get()->WALKING_SPEED;
}

void Camera3D::TurnRight() {
    yaw -= EngineSetup::get()->TURN_SPEED;
}

void Camera3D::TurnLeft() {
    yaw += EngineSetup::get()->TURN_SPEED;
}

void Camera3D::StrafeRight() {
    strafe += EngineSetup::get()->STRAFE_SPEED;
}

void Camera3D::StrafeLeft() {
    strafe -= EngineSetup::get()->STRAFE_SPEED;
}

void Camera3D::UpdatePositionForVelocity() {
    this->setPosition(this->velocity.vertex2);
}

void Camera3D::UpdateVelocity() {
    if ((fabs(speed) > 0)) {
        this->velocity.vertex2.z = this->getPosition().z + speed * (float) cos(-yaw * M_PI / 180.0);
        this->velocity.vertex2.x = this->getPosition().x + speed * (float) sin(-yaw * M_PI / 180.0);
        this->velocity.vertex2.y = this->getPosition().y + speed * (float) sin(pitch * M_PI / 180.0); // VERTICAL
    }

    // Move the camera side ways
    if ((fabs(strafe) > 0)) {
        this->velocity.vertex2.z += strafe * (float) -sin(-yaw * M_PI / 180.0);
        this->velocity.vertex2.x += -strafe * (float) -cos(-yaw * M_PI / 180.0);
    }

    // Reset speed
    speed = 0;
    strafe = 0;
    jump = 0;
}

void Camera3D::UpdateRotation() {
    M3 im = M3::getMatrixRotationForEulerAngles(this->pitch, this->yaw, this->roll);

    this->setRotation(im);
}

void Camera3D::limitPitch() {
    if (this->pitch >= 89) {
        this->pitch = 89;
    }
    if (this->pitch <= -89) {
        this->pitch = -89;
    }
}

Object3D *Camera3D::getFollowTo() const {
    return follow_to;
}

void Camera3D::setFollowTo(Object3D *followTo) {
    follow_to = followTo;
}

void Camera3D::makeKineticCharacter(const btTransform& transform, btConvexShape *capsule) {
    m_ghostObject = new btPairCachingGhostObject();
    m_ghostObject->setWorldTransform(transform);

    m_ghostObject->setCollisionShape(capsule);
    m_ghostObject->setUserPointer(this);
}

btPairCachingGhostObject *Camera3D::getGhostObject() const {
    return m_ghostObject;
}