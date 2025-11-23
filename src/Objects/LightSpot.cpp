
#include <glm/trigonometric.hpp>
#include "../../include/Objects/LightSpot.h"

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "../../include/Brakeza3D.h"

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

void LightSpot::setCutOff(float cutOff) {
    LightSpot::cutOff = glm::cos(glm::radians(cutOff));
}

void LightSpot::setOuterCutOff(float outerCutOff) {
    LightSpot::outerCutOff = glm::cos(glm::radians(outerCutOff));
}

const char *LightSpot::getTypeObject() {
    return "SpotLight3D";
}

const char *LightSpot::getTypeIcon() {
    return "spotLightIcon";
}

void LightSpot::drawImGuiProperties()
{
    LightPoint::drawImGuiProperties();

    if (ImGui::CollapsingHeader("SpotLight")) {
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

cJSON *LightSpot::getJSON()
{
    cJSON *root = LightPoint::getJSON();

    cJSON_AddNumberToObject(root, "cutOff", glm::degrees(glm::acos(cutOff)));
    cJSON_AddNumberToObject(root, "outerCutOff", glm::degrees(glm::acos(outerCutOff)));

    return root;
}

void LightSpot::onUpdate()
{
    LightPoint::onUpdate();
}

void LightSpot::createFromJSON(cJSON *object)
{
    auto o = new LightSpot(glm::vec4(), glm::vec4(), glm::vec4(),glm::vec4(),0,0,0,0,0);

    setPropertiesFromJSON(object, o);

    Brakeza3D::get()->addObject3D(o, cJSON_GetObjectItemCaseSensitive(object, "name")->valuestring);
}

void LightSpot::setPropertiesFromJSON(cJSON *object, LightSpot *o)
{
    LightPoint::setPropertiesFromJSON(object, o);

    o->setCutOff(static_cast<float>(cJSON_GetObjectItemCaseSensitive(object, "cutOff")->valuedouble));
    o->setOuterCutOff(static_cast<float>(cJSON_GetObjectItemCaseSensitive(object, "outerCutOff")->valuedouble));
}

LightSpot *LightSpot::create(const Vertex3D &position, const Vertex3D &direction)
{
    auto *o = new LightSpot(
        glm::vec4(direction.x, direction.y, direction.z, 0),
        glm::vec4(0.05f, 0.05f, 0.05f, 0),
        glm::vec4(1.0f, 0.0f, 0.0f, 0),
        glm::vec4(1.0f, 1.0f, 1.0f, 0),
        1.0f,
        0.09f,
        0.032f,
        0.9763,
        0.9659
    );

    o->setPosition(position);

    return o;
}

void LightSpot::setDirection(Vertex3D d)
{
    direction = glm::vec4(d.x, d.y, d.z, 0);
}

glm::mat4 LightSpot::getLightSpaceMatrix()
{
    float aspect = 1.0f;                            // Aspect ratio cuadrado para el shadow map
    float fov = cutOff; // Ángulo total del cono (cutOffAngle debería ser el semi-ángulo)

    glm::mat4 lightProjection = glm::perspective(
        fov,
        aspect,
        EngineSetup::get()->SHADOW_MAPPING_DEPTH_FRUSTUM_NEAR_PLANE,
        EngineSetup::get()->SHADOW_MAPPING_DEPTH_FRUSTUM_FAR_PLANE
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