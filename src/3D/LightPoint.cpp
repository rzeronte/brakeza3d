
#include "../../include/3D/LightPoint.h"
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