
#include "../../include/3D/LightPoint.h"
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include "../../include/Render/Maths.h"
#include "../../include/Brakeza3D.h"
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

LightPoint *LightPoint::create(Vertex3D position)
{
    return nullptr;
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

const char *LightPoint::getTypeObject()
{
    return SceneObjectTypes::LIGHT_POINT;
}

const char *LightPoint::getTypeIcon()
{
    return IncosByObject::LIGHT_POINT;
}

void LightPoint::drawImGuiProperties()
{
    Object3D::drawImGuiProperties();
    LightPointGUI::drawImGuiProperties(this);
}