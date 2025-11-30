#include <glm/ext/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include "../../include/Components/Camera3D.h"
#include "../../include/Misc/ToolsMaths.h"

Camera3D::Camera3D()
{
    frustum = new Frustum();
    setLabel(BrakezaSetup::get()->CAMERA_OBJECT_NAME);
}

void Camera3D::Pitch(float value)
{
    pitch += value;
    limitPitch();
}

void Camera3D::Yaw(float value)
{
    yaw -= value;
}

void Camera3D::MoveForward(float v)
{
    speed += v;
}

void Camera3D::MoveVertical(float v)
{
    setPosition(getPosition() + getRotation().Y().getScaled(v));
}

void Camera3D::MoveBackward(float v)
{
    speed -= v;
}

void Camera3D::StrafeRight()
{
    strafe += BrakezaSetup::get()->STRAFE_SPEED;
}

void Camera3D::StrafeLeft()
{
    strafe -= BrakezaSetup::get()->STRAFE_SPEED;
}

void Camera3D::UpdatePositionForVelocity()
{
    AddToPosition(velocity.getComponent());
}

void Camera3D::UpdateVelocity()
{
    if (fabs(speed) > 0) {
        velocity.vertex2.z = getPosition().z + speed * static_cast<float>(cos(-yaw * M_PI / 180.0));
        velocity.vertex2.x = getPosition().x + speed * static_cast<float>(sin(-yaw * M_PI / 180.0));
        velocity.vertex2.y = getPosition().y + speed * static_cast<float>(sin(pitch * M_PI / 180.0)); // VERTICAL
    }

    if (fabs(strafe) > 0) {
        velocity.vertex2.z += strafe * static_cast<float>(-sin(-yaw * M_PI / 180.0));
        velocity.vertex2.x += -strafe * static_cast<float>(-cos(-yaw * M_PI / 180.0));
    }

    // Reset speed
    speed = 0;
    strafe = 0;
    jump = 0;
}

void Camera3D::setRotationFromEulerAngles(float nPitch, float nYaw, float nRoll)
{
    pitch = nPitch;
    yaw = nYaw;
    roll = nRoll;

    setRotation(M3::getMatrixRotationForEulerAngles(pitch, yaw, roll));
}

void Camera3D::limitPitch()
{
    if (pitch >= 89) {
        pitch = 89;
    }
    if (pitch <= -89) {
        pitch = -89;
    }
}

Frustum *Camera3D::getFrustum() const
{
    return frustum;
}

float &Camera3D::getYaw()
{
    return yaw;
}

float &Camera3D::getPitch()
{
    return pitch;
}

float &Camera3D::getRoll()
{
    return roll;
}

Vector3D &Camera3D::getVelocity()
{
    return velocity;
}

glm::mat4 Camera3D::getGLMMat4ViewMatrix()
{
    Vertex3D forward = getRotation().getTranspose() * Vertex3D(0, 0, -1);

    const auto p = position.toGLM();
    ViewMatrix = glm::lookAt(
        p,
        p + forward.toGLM(),
        glm::vec3(0,1,0)
    );

    return ViewMatrix;
}

glm::mat4 Camera3D::getGLMMat4ProjectionMatrix()
{
    float horizontalFOV = BrakezaSetup::get()->HORIZONTAL_FOV;
    float aspectRatio = 4.0f / 3.0f;
    float verticalFOV = glm::degrees(2.0f * atan(tan(glm::radians(horizontalFOV) / 2.0f) / aspectRatio));

    return glm::perspective(glm::radians(verticalFOV), aspectRatio, 0.1f, BrakezaSetup::get()->FRUSTUM_FARPLANE_DISTANCE);
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
    BrakezaSetup::get()->HORIZONTAL_FOV = v;
}