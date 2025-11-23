
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
    auto o = new LightPoint(
        glm::vec4(0.05f, 0.05f, 0.05f, 0),
        glm::vec4(1.0f, 0.0f, 0.0f, 0),
        glm::vec4(1.0f, 1.0f, 1.0f, 0),
        1.0f,
        0.09f,
        0.032f
    );

    o->setPosition(position);

    return o;
}

void LightPoint::setColor(Color value) {
    this->diffuse = glm::vec4(value.r, value.g, value.b, 1);
}

void LightPoint::setAmbient(Color value) {
    this->ambient = glm::vec4(value.r, value.g, value.b, 1);
}

void LightPoint::setColorSpecular(Color c) {
    this->specular = glm::vec4(c.r, c.g, c.b, 1);
}

void LightPoint::onUpdate()
{
    Object3D::onUpdate();
}

void LightPoint::setConstant(float value) {
    constant = value;
}

void LightPoint::setLinear(float value) {
    linear = value;
}

void LightPoint::setCuadratic(float value) {
    quadratic = value;
}

const char *LightPoint::getTypeObject() {
    return "LightPoint3D";
}

const char *LightPoint::getTypeIcon() {
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

cJSON *LightPoint::getJSON()
{
    cJSON *root = Object3D::getJSON();

    cJSON *ambientJSON = cJSON_CreateObject();
    cJSON_AddNumberToObject(ambientJSON, "x", ambient.x);
    cJSON_AddNumberToObject(ambientJSON, "y", ambient.y);
    cJSON_AddNumberToObject(ambientJSON, "z", ambient.z);
    cJSON_AddItemToObject(root, "ambient", ambientJSON);

    cJSON *diffuseJSON = cJSON_CreateObject();
    cJSON_AddNumberToObject(diffuseJSON, "x", diffuse.x);
    cJSON_AddNumberToObject(diffuseJSON, "y", diffuse.y);
    cJSON_AddNumberToObject(diffuseJSON, "z", diffuse.z);
    cJSON_AddItemToObject(root, "diffuse", diffuseJSON);

    cJSON *specularJSON = cJSON_CreateObject();
    cJSON_AddNumberToObject(specularJSON, "x", specular.x);
    cJSON_AddNumberToObject(specularJSON, "y", specular.y);
    cJSON_AddNumberToObject(specularJSON, "z", specular.z);
    cJSON_AddItemToObject(root, "specular", specularJSON);

    cJSON_AddNumberToObject(root, "constant", constant);
    cJSON_AddNumberToObject(root, "linear", linear);
    cJSON_AddNumberToObject(root, "quadratic", quadratic);

    return root;
}

void LightPoint::setPropertiesFromJSON(cJSON *object, LightPoint *o)
{
    o->setBelongToScene(true);
    Object3D::setPropertiesFromJSON(object, o);

    auto ambient = cJSON_GetObjectItemCaseSensitive(object, "ambient");
    o->ambient.x = (float) cJSON_GetObjectItemCaseSensitive(ambient, "x")->valuedouble;
    o->ambient.y = (float) cJSON_GetObjectItemCaseSensitive(ambient, "y")->valuedouble;
    o->ambient.z = (float) cJSON_GetObjectItemCaseSensitive(ambient, "z")->valuedouble;

    auto diffuse = cJSON_GetObjectItemCaseSensitive(object, "diffuse");
    o->diffuse.x = (float) cJSON_GetObjectItemCaseSensitive(diffuse, "x")->valuedouble;
    o->diffuse.y = (float) cJSON_GetObjectItemCaseSensitive(diffuse, "y")->valuedouble;
    o->diffuse.z = (float) cJSON_GetObjectItemCaseSensitive(diffuse, "z")->valuedouble;

    auto specular = cJSON_GetObjectItemCaseSensitive(object, "specular");
    o->specular.x = (float) cJSON_GetObjectItemCaseSensitive(specular, "x")->valuedouble;
    o->specular.y = (float) cJSON_GetObjectItemCaseSensitive(specular, "y")->valuedouble;
    o->specular.z = (float) cJSON_GetObjectItemCaseSensitive(specular, "z")->valuedouble;

    o->setConstant((float)cJSON_GetObjectItemCaseSensitive(object, "constant")->valuedouble);
    o->setLinear((float)cJSON_GetObjectItemCaseSensitive(object, "linear")->valuedouble);
    o->setCuadratic((float)cJSON_GetObjectItemCaseSensitive(object, "quadratic")->valuedouble);
}

void LightPoint::createFromJSON(cJSON *object)
{
    auto o = new LightPoint(
        glm::vec4(),
        glm::vec4(),
        glm::vec4(),
        0,
        0,
        0
    );

    setPropertiesFromJSON(object, o);

    Brakeza3D::get()->addObject3D(o, cJSON_GetObjectItemCaseSensitive(object, "name")->valuestring);
}
