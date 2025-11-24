//
// Created by Eduardo on 24/11/2025.
//

#include "../../include/Persistence/LightPointSerializer.h"
#include "../../include/Brakeza3D.h"
#include "../../include/Persistence/JSONSerializerRegistry.h"

cJSON * LightPointSerializer::JsonByObject(Object3D *o)
{
    auto light = dynamic_cast<LightPoint*>(o);

    cJSON *root = JSONSerializerRegistry::GetJsonByObject(o);

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
    std::cout << "[LightPointSerializer LoadJSONObject]"  << std::endl;
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

void LightPointSerializer::ApplyJsonToObject(cJSON *json, Object3D *o)
{
    std::cout << "[Mesh3DSerializer ApplyJsonToObject] " << o->getTypeObject() << std::endl;

    auto light = dynamic_cast<LightPoint*>(o);

    Object3DSerializer::ApplyJsonToObject(json, o);

    light->setBelongToScene(true);

    auto ambient = cJSON_GetObjectItemCaseSensitive(json, "ambient");
    light->ambient.x = (float) cJSON_GetObjectItemCaseSensitive(ambient, "x")->valuedouble;
    light->ambient.y = (float) cJSON_GetObjectItemCaseSensitive(ambient, "y")->valuedouble;
    light->ambient.z = (float) cJSON_GetObjectItemCaseSensitive(ambient, "z")->valuedouble;

    auto diffuse = cJSON_GetObjectItemCaseSensitive(json, "diffuse");
    light->diffuse.x = (float) cJSON_GetObjectItemCaseSensitive(diffuse, "x")->valuedouble;
    light->diffuse.y = (float) cJSON_GetObjectItemCaseSensitive(diffuse, "y")->valuedouble;
    light->diffuse.z = (float) cJSON_GetObjectItemCaseSensitive(diffuse, "z")->valuedouble;

    auto specular = cJSON_GetObjectItemCaseSensitive(json, "specular");
    light->specular.x = (float) cJSON_GetObjectItemCaseSensitive(specular, "x")->valuedouble;
    light->specular.y = (float) cJSON_GetObjectItemCaseSensitive(specular, "y")->valuedouble;
    light->specular.z = (float) cJSON_GetObjectItemCaseSensitive(specular, "z")->valuedouble;

    light->setConstant((float)cJSON_GetObjectItemCaseSensitive(json, "constant")->valuedouble);
    light->setLinear((float)cJSON_GetObjectItemCaseSensitive(json, "linear")->valuedouble);
    light->setCuadratic((float)cJSON_GetObjectItemCaseSensitive(json, "quadratic")->valuedouble);
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
    o->setBelongToScene(true);

    Brakeza3D::get()->addObject3D(o, Brakeza3D::uniqueObjectLabel("LightPoint3D"));
}
