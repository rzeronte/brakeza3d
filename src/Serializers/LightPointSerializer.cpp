//
// Created by Eduardo on 24/11/2025.
//

#include "../../include/Serializers/LightPointSerializer.h"
#include "../../include/Brakeza.h"
#include "../../include/3D/LightPoint.h"
#include "../../include/Components/ComponentsManager.h"
#include "../../include/Misc/ToolsJSON.h"
#include "../../include/Serializers/JSONSerializerRegistry.h"
#include "../../include/Serializers/Object3DSerializer.h"

cJSON * LightPointSerializer::JsonByObject(Object3D *o)
{
    Logging::Message("[LightPointSerializer] JsonByObject: %d", (int) o->getTypeObject());

    auto light = dynamic_cast<LightPoint*>(o);

    auto root = Object3DSerializer().JsonByObject(o);

    cJSON_AddItemToObject(root, "ambient", ToolsJSON::Vertex3DToJSON(Vertex3D::fromGLM(light->ambient)));
    cJSON_AddItemToObject(root, "diffuse", ToolsJSON::Vertex3DToJSON(Vertex3D::fromGLM(light->diffuse)));
    cJSON_AddItemToObject(root, "specular", ToolsJSON::Vertex3DToJSON(Vertex3D::fromGLM(light->specular)));

    cJSON_AddNumberToObject(root, "constant", light->constant);
    cJSON_AddNumberToObject(root, "linear", light->linear);
    cJSON_AddNumberToObject(root, "quadratic", light->quadratic);

    return root;
}

Object3D * LightPointSerializer::ObjectByJson(cJSON *json)
{
    Logging::Message("[LightPointSerializer] ObjectByJson");

    auto o = new LightPoint(
        glm::vec4(0.05f, 0.05f, 0.05f, 0),
        glm::vec4(1.0f, 0.0f, 0.0f, 0),
        glm::vec4(1.0f, 1.0f, 1.0f, 0),
        1.0f,
        0.09f,
        0.032f
    );

    ApplyJsonToObject(json, o);

    return o;
}

void LightPointSerializer::ApplyJsonToObject(const cJSON *json, Object3D *o)
{
    Logging::Message("[LightPointSerializer] ApplyJsonToObject %d", (int) o->getTypeObject());

    auto light = dynamic_cast<LightPoint*>(o);

    Object3DSerializer().ApplyJsonToObject(json, o);

    light->ambient = ToolsJSON::getVertex3DByJSON(cJSON_GetObjectItemCaseSensitive(json, "ambient")).toGLM4();
    light->diffuse = ToolsJSON::getVertex3DByJSON(cJSON_GetObjectItemCaseSensitive(json, "diffuse")).toGLM4();
    light->specular = ToolsJSON::getVertex3DByJSON(cJSON_GetObjectItemCaseSensitive(json, "specular")).toGLM4();

    light->setConstant(static_cast<float>(cJSON_GetObjectItemCaseSensitive(json, "constant")->valuedouble));
    light->setLinear(static_cast<float>(cJSON_GetObjectItemCaseSensitive(json, "linear")->valuedouble));
    light->setCuadratic(static_cast<float>(cJSON_GetObjectItemCaseSensitive(json, "quadratic")->valuedouble));
}

void LightPointSerializer::LoadFileIntoScene(const std::string &model)
{
    auto o = new LightPoint(
        glm::vec4(0.05f, 0.05f, 0.05f, 0),
        glm::vec4(1.0f, 0.0f, 0.0f, 0),
        glm::vec4(1.0f, 1.0f, 1.0f, 0),
        1.0f,
        0.09f,
        0.032f
    );

    o->setPosition(ComponentsManager::get()->getComponentCamera()->getCamera()->getPosition());

    Brakeza::get()->addObject3D(o, Brakeza::UniqueObjectLabel("LightPoint3D"));
}
