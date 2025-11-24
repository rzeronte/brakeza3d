
#include "../../include/Objects/LightPoint.h"

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "../../include/Render/Maths.h"
#include "../../include/Brakeza3D.h"

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

void LightPoint::setColor(Color value)
{
    this->diffuse = glm::vec4(value.r, value.g, value.b, 1);
}

void LightPoint::setAmbient(Color value)
{
    this->ambient = glm::vec4(value.r, value.g, value.b, 1);
}

void LightPoint::setColorSpecular(Color c)
{
    this->specular = glm::vec4(c.r, c.g, c.b, 1);
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
    return "LightPoint3D";
}

const char *LightPoint::getTypeIcon()
{
    return "lightIcon";
}

void LightPoint::drawImGuiProperties()
{
    Object3D::drawImGuiProperties();

    if (ImGui::CollapsingHeader("LightPoint")) {

        const float range_potence_sensibility = 0.01f;
        const float range_min = -90000;
        const float range_max = 90000;

        ImVec4 color = {diffuse.x, diffuse.y, diffuse.z, 1};
        bool changed_color = ImGui::ColorEdit4("Diffuse##", (float *) &color, ImGuiColorEditFlags_NoOptions);
        if (changed_color) {
            setColor(Color(color.x,color.y,color.z));
        }
        color = {specular.x, specular.y, specular.z, 1};
        changed_color = ImGui::ColorEdit4("Specular##", (float *) &color, ImGuiColorEditFlags_NoOptions);
        if (changed_color) {
            setColorSpecular(Color(color.x, color.y, color.z));
        }

        color = {ambient.x, ambient.y, ambient.z, 1};
        changed_color = ImGui::ColorEdit4("Ambient##", (float *) &color, ImGuiColorEditFlags_NoOptions);
        if (changed_color) {
            setAmbient(Color(color.x, color.y,color.z));
        }

        ImGui::DragScalar("Constant", ImGuiDataType_Float, &constant, range_potence_sensibility,&range_min, &range_max, "%f", 1.0f);
        ImGui::DragScalar("Linear", ImGuiDataType_Float, &linear, range_potence_sensibility,&range_min, &range_max, "%f", 1.0f);
        ImGui::DragScalar("Quadratic", ImGuiDataType_Float, &quadratic, range_potence_sensibility,&range_min, &range_max, "%f", 1.0f);
    }
}