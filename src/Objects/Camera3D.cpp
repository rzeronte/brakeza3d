#include <BulletCollision/CollisionShapes/btCapsuleShape.h>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include "../../include/Components/Camera3D.h"
#include "../../include/Render/Maths.h"
#include "../../include/Render/Logging.h"

Camera3D::Camera3D(): speed(0), strafe(0), jump(0)
{
    frustum = new Frustum();

    this->setLabel(EngineSetup::get()->cameraNameIdentifier);
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
    this->addToPosition(this->velocity.getComponent());
}

void Camera3D::UpdateVelocity()
{
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

void Camera3D::setRotationFromEulerAngles(float pitch, float yaw, float roll)
{
    this->pitch = pitch;
    this->yaw = yaw;
    this->roll = roll;

    this->setRotation(M3::getMatrixRotationForEulerAngles(pitch, yaw, roll));
}

void Camera3D::limitPitch() {
    if (this->pitch >= 89) {
        this->pitch = 89;
    }
    if (this->pitch <= -89) {
        this->pitch = -89;
    }
}

Frustum *Camera3D::getFrustum() const {
    return frustum;
}

float &Camera3D::getYaw() {
    return yaw;
}

float &Camera3D::getPitch() {
    return pitch;
}

float &Camera3D::getRoll() {
    return roll;
}

Vector3D &Camera3D::getVelocity() {
    return velocity;
}

glm::mat4 Camera3D::getViewMatrix()
{
    Vertex3D forward = getRotation().getTranspose() * Vertex3D(0, 0, 1);

    const auto p = position.toGLM();
    ViewMatrix = glm::lookAt(
        p,
        p + forward.toGLM(),
        glm::vec3(0,-1,0)
    );

    return ViewMatrix;
}

glm::mat4 Camera3D::getProjectionMatrix()
{
    return glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, EngineSetup::get()->FRUSTUM_FARPLANE_DISTANCE);
}

