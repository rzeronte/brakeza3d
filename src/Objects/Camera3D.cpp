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

void Camera3D::MoveForward(float v) {
    speed += v;
}

void Camera3D::MoveVertical(float v)
{
    setPosition(getPosition() + getRotation().Y().getScaled(v));
}

void Camera3D::MoveBackward(float v) {
    speed -= v;
}

void Camera3D::StrafeRight()
{
    strafe += EngineSetup::get()->STRAFE_SPEED;
}

void Camera3D::StrafeLeft()
{
    strafe -= EngineSetup::get()->STRAFE_SPEED;
}

void Camera3D::UpdatePositionForVelocity()
{
    this->addToPosition(velocity.getComponent());
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

glm::mat4 Camera3D::getGLMMat4ViewMatrix()
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

glm::mat4 Camera3D::getGLMMat4ProjectionMatrix()
{
    float horizontalFOV = EngineSetup::get()->HORIZONTAL_FOV;
    float aspectRatio = 4.0f / 3.0f;
    float verticalFOV = glm::degrees(2.0f * atan(tan(glm::radians(horizontalFOV) / 2.0f) / aspectRatio));

    return glm::perspective(glm::radians(verticalFOV), aspectRatio, 0.1f, EngineSetup::get()->FRUSTUM_FARPLANE_DISTANCE);
}

M3 Camera3D::getM3ViewMatrix()
{
    auto m = glm::mat3(Camera3D::getGLMMat4ViewMatrix());

    return M3::fromMat3GLM(m);
}

M3 Camera3D::getM3ProjectionMatrix()
{
    auto m = glm::mat3(Camera3D::getGLMMat4ProjectionMatrix());

    return M3::fromMat3GLM(m);
}

void Camera3D::setFOV(float v)
{
    EngineSetup::get()->HORIZONTAL_FOV = v;
}