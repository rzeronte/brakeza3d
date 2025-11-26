//
// Created by Eduardo on 24/11/2025.
//

#include "../../include/Serializers/LightPointSerializer.h"
#include "../../include/Brakeza.h"
#include "../../include/Serializers/JSONSerializerRegistry.h"

cJSON * LightPointSerializer::JsonByObject(Object3D *o)
{
    Logging::Message("[LightPointSerializer] JsonByObject: %s", o->getTypeObject());

    auto light = dynamic_cast<LightPoint*>(o);

    auto root = Object3DSerializer().JsonByObject(o);

    cJSON *ambientJSON = cJSON_CreateObject();
    cJSON_AddNumberToObject(ambientJSON, "x", light->ambient.x);
    cJSON_AddNumberToObject(ambientJSON, "y", light->ambient.y);
    cJSON_AddNumberToObject(ambientJSON, "z", light->ambient.z);
    cJSON_AddItemToObject(root, "ambient", ambientJSON);

    cJSON *diffuseJSON = cJSON_CreateObject();
    cJSON_AddNumberToObject(diffuseJSON, "x", light->diffuse.x);
    cJSON_AddNumberToObject(diffuseJSON, "y", light->diffuse.y);
    cJSON_AddNumberToObject(diffuseJSON, "z", light->diffuse.z);
    cJSON_AddItemToObject(root, "diffuse", diffuseJSON);

    cJSON *specularJSON = cJSON_CreateObject();
    cJSON_AddNumberToObject(specularJSON, "x", light->specular.x);
    cJSON_AddNumberToObject(specularJSON, "y", light->specular.y);
    cJSON_AddNumberToObject(specularJSON, "z", light->specular.z);
    cJSON_AddItemToObject(root, "specular", specularJSON);

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
    Logging::Message("[LightPointSerializer] ApplyJsonToObject", o->getTypeObject());

    auto light = dynamic_cast<LightPoint*>(o);

    Object3DSerializer().ApplyJsonToObject(json, o);

    auto ambient = cJSON_GetObjectItemCaseSensitive(json, "ambient");
    light->ambient.x = static_cast<float>(cJSON_GetObjectItemCaseSensitive(ambient, "x")->valuedouble);
    light->ambient.y = static_cast<float>(cJSON_GetObjectItemCaseSensitive(ambient, "y")->valuedouble);
    light->ambient.z = static_cast<float>(cJSON_GetObjectItemCaseSensitive(ambient, "z")->valuedouble);

    auto diffuse = cJSON_GetObjectItemCaseSensitive(json, "diffuse");
    light->diffuse.x = static_cast<float>(cJSON_GetObjectItemCaseSensitive(diffuse, "x")->valuedouble);
    light->diffuse.y = static_cast<float>(cJSON_GetObjectItemCaseSensitive(diffuse, "y")->valuedouble);
    light->diffuse.z = static_cast<float>(cJSON_GetObjectItemCaseSensitive(diffuse, "z")->valuedouble);

    auto specular = cJSON_GetObjectItemCaseSensitive(json, "specular");
    light->specular.x = static_cast<float>(cJSON_GetObjectItemCaseSensitive(specular, "x")->valuedouble);
    light->specular.y = static_cast<float>(cJSON_GetObjectItemCaseSensitive(specular, "y")->valuedouble);
    light->specular.z = static_cast<float>(cJSON_GetObjectItemCaseSensitive(specular, "z")->valuedouble);

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

    Brakeza::get()->addObject3D(o, Brakeza::uniqueObjectLabel("LightPoint3D"));
}
