
#include "../../include/3D/LightPoint.h"
#include <algorithm>
#include <cmath>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include "../../include/Misc/ToolsMaths.h"
#include "../../include/Brakeza.h"
#include "../../include/GUI/Objects/LightPointGUI.h"

LightPoint::LightPoint(
    glm::vec4 ambient,
    glm::vec4 diffuse,
    glm::vec4 specular,
    float constant,
    float linear,
    float quadratic
) :
    ambient(ambient),
    diffuse(diffuse),
    specular(specular),
    constant(constant),
    linear(linear),
    quadratic(quadratic)
{
}

void LightPoint::setColor(const Color &c)
{
    diffuse = glm::vec4(c.r, c.g, c.b, 1);
}

void LightPoint::setAmbient(const Color &c)
{
    ambient = glm::vec4(c.r, c.g, c.b, 1);
}

void LightPoint::setColorSpecular(const Color &c)
{
    specular = glm::vec4(c.r, c.g, c.b, 1);
}

void LightPoint::onUpdate()
{
    Object3D::onUpdate();
}

void LightPoint::setConstant(float value)
{
    constant = value;
}

void LightPoint::setLinear(float value)
{
    linear = value;
}

void LightPoint::setCuadratic(float value)
{
    quadratic = value;
}

float LightPoint::getRadius() const
{
    float threshold = 1.0f / 256.0f;
    float maxChannel = std::max({diffuse.r, diffuse.g, diffuse.b});
    if (maxChannel < 0.001f) return 0.0f;

    float cTerm = constant - maxChannel / threshold;

    if (std::abs(quadratic) < 0.001f) {
        if (std::abs(linear) < 0.001f) {
            return (constant < maxChannel / threshold) ? 1000.0f : 0.0f;
        }
        return (maxChannel / threshold - constant) / linear;
    }

    float discriminant = linear * linear - 4.0f * quadratic * cTerm;
    if (discriminant < 0.0f) return 1000.0f;

    return (-linear + std::sqrt(discriminant)) / (2.0f * quadratic);
}

ObjectType LightPoint::getTypeObject() const
{
    return ObjectType::LightPoint;
}

GUIType::Sheet LightPoint::getIcon()
{
    return IconObject::LIGHT_POINT;
}

void LightPoint::DrawPropertiesGUI()
{
    Object3D::DrawPropertiesGUI();
    LightPointGUI::DrawPropertiesGUI(this);
}