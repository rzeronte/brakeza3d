
#include <glm/trigonometric.hpp>
#include "../../include/Objects/SpotLight3D.h"
#include "../../include/Brakeza3D.h"

SpotLight3D::SpotLight3D(
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
    LightPoint3D(ambient, diffuse, specular, constant, linear, quadratic),
    cutOff(cutOff),
    outerCutOff(outerCutOff),
    direction(direction)
{
}

void SpotLight3D::setCutOff(float cutOff) {
    SpotLight3D::cutOff = glm::cos(glm::radians(cutOff));
}

void SpotLight3D::setOuterCutOff(float outerCutOff) {
    SpotLight3D::outerCutOff = glm::cos(glm::radians(outerCutOff));
}

const char *SpotLight3D::getTypeObject() {
    return "SpotLight3D";
}

const char *SpotLight3D::getTypeIcon() {
    return "spotLightIcon";
}

void SpotLight3D::drawImGuiProperties() {
    LightPoint3D::drawImGuiProperties();

    ImGui::Separator();

    if (ImGui::TreeNode("SpotLight")) {
        if (ImGui::TreeNode("Direction##")) {
            const float range_sensibility = 0.01f;
            const float range_min = 0.0f;
            const float range_max = 1.0f;
            ImGui::DragScalar("x", ImGuiDataType_Float, &direction.x, range_sensibility, &range_min, &range_max, "%f", 1.0f);
            ImGui::DragScalar("y", ImGuiDataType_Float, &direction.y, range_sensibility, &range_min, &range_max, "%f", 1.0f);
            ImGui::DragScalar("z", ImGuiDataType_Float, &direction.z, range_sensibility, &range_min, &range_max, "%f", 1.0f);
            ImGui::TreePop();
        }

        const float range_sensibility = 0.01f;
        const float range_min = -10;
        const float range_max = 10;

        ImGui::DragScalar("cutOff", ImGuiDataType_Float, &cutOff, range_sensibility,&range_min, &range_max, "%f", 1.0f);
        ImGui::DragScalar("outerCutOff", ImGuiDataType_Float, &outerCutOff, range_sensibility,&range_min, &range_max, "%f", 1.0f);
   }
}

cJSON *SpotLight3D::getJSON() {
    cJSON *root = LightPoint3D::getJSON();

    cJSON_AddNumberToObject(root, "cutOff", cutOff);
    cJSON_AddNumberToObject(root, "outerCutOff", outerCutOff);

    return root;
}

void SpotLight3D::onUpdate() {
    LightPoint3D::onUpdate();
}

void SpotLight3D::createFromJSON(cJSON *object)
{
    auto o = new SpotLight3D(glm::vec4(), glm::vec4(), glm::vec4(),glm::vec4(),0,0,0,0,0);

    SpotLight3D::setPropertiesFromJSON(object, o);

    Brakeza3D::get()->addObject3D(o, cJSON_GetObjectItemCaseSensitive(object, "name")->valuestring);
}

void SpotLight3D::setPropertiesFromJSON(cJSON *object, SpotLight3D *o)
{
    LightPoint3D::setPropertiesFromJSON(object, o);

    o->setCutOff((float) cJSON_GetObjectItemCaseSensitive(object, "cutOff")->valuedouble);
    o->setOuterCutOff((float) cJSON_GetObjectItemCaseSensitive(object, "outerCutOff")->valuedouble);
}

