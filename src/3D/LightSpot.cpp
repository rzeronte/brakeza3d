
#include <glm/trigonometric.hpp>
#include "../../include/3D/LightSpot.h"
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include "../../include/Brakeza.h"
#include "../../include/GUI/Objects/LightSpotGUI.h"

LightSpot::LightSpot(
    const glm::vec4 &direction,
    const glm::vec4 &ambient,
    const glm::vec4 &diffuse,
    const glm::vec4 &specular,
    float constant,
    float linear,
    float quadratic,
    float cutOff,
    float outerCutOff
)
:
    LightPoint(ambient, diffuse, specular, constant, linear, quadratic),
    direction(direction),
    cutOff(cutOff),
    outerCutOff(outerCutOff)
{
}

void LightSpot::setCutOff(float value)
{
    cutOff = glm::cos(glm::radians(value));
}

void LightSpot::setOuterCutOff(float value)
{
    outerCutOff = glm::cos(glm::radians(value));
}

const char *LightSpot::getTypeObject()
{
    return SceneObjectTypes::LIGHT_SPOT;
}

float LightSpot::getCutOff() const
{
    return cutOff;
}

float LightSpot::getOuterCutOff() const
{
    return outerCutOff;
}

const char *LightSpot::getTypeIcon()
{
    return IconsByObject::LIGHT_SPOT;
}

void LightSpot::drawImGuiProperties()
{
    LightPoint::drawImGuiProperties();
    LightSpotGUI::DrawPropertiesGUI(this);
}

void LightSpot::onUpdate()
{
    LightPoint::onUpdate();
}

LightSpot *LightSpot::create(const Vertex3D &position, const Vertex3D &direction)
{
    return nullptr;
}

void LightSpot::setDirection(const Vertex3D &d)
{
    direction = glm::vec4(d.x, d.y, d.z, 0);
}

glm::mat4 LightSpot::getLightSpaceMatrix()
{
    float aspect = 1.0f; // Aspect ratio cuadrado para el shadow map
    float fov = cutOff;  // Ángulo total del cono (cutOffAngle debería ser el semi-ángulo)

    glm::mat4 lightProjection = glm::perspective(
        fov,
        aspect,
        BrakezaSetup::get()->SHADOW_MAPPING_DEPTH_FRUSTUM_NEAR_PLANE,
        BrakezaSetup::get()->SHADOW_MAPPING_DEPTH_FRUSTUM_FAR_PLANE
    );

    Vertex3D forward = getRotation() * Vertex3D(0, 0, -1);
    const auto p = position.toGLM();

    glm::mat4 lightView = glm::lookAt(
        p,
        p + forward.toGLM(),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );

    return lightProjection * lightView;
}