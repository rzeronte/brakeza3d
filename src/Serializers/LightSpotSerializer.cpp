//
// Created by Eduardo on 24/11/2025.
//

#include "../../include/Serializers/LightSpotSerializer.h"

#include <glm/detail/func_trigonometric.inl>
#include "../../include/3D/LightSpot.h"
#include "../../include/Brakeza.h"
#include "../../include/Serializers/JSONSerializerRegistry.h"
#include "../../include/Misc/ToolsJSON.h"

cJSON * LightSpotSerializer::JsonByObject(Object3D *o)
{
    Logging::Message("[LightSpotSerializer json] Alive here...");
    std::cout << "[LightSpotSerializer json] " << o->getTypeObject() << std::endl;

    auto *spot = dynamic_cast<LightSpot*>(o);

    cJSON *root = JSONSerializerRegistry::GetJsonByObject(o);

    cJSON_AddNumberToObject(root, "cutOff", glm::degrees(glm::acos(spot->getCutOff())));
    cJSON_AddNumberToObject(root, "outerCutOff", glm::degrees(glm::acos(spot->getOuterCutOff())));

    return root;
}

Object3D * LightSpotSerializer::ObjectByJson(cJSON *json)
{
    auto direction = ToolsJSON::parseVertex3DJSON(cJSON_GetObjectItemCaseSensitive(json, "ambient"));

    auto o = new LightSpot(
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

    ApplyJsonToObject(json, o);

    return o;
}

void LightSpotSerializer::ApplyJsonToObject(const cJSON *json, Object3D *o)
{
    auto spot = dynamic_cast<LightSpot*>(o);

    LightPointSerializer::ApplyJsonToObject(json, o);

    spot->setCutOff(static_cast<float>(cJSON_GetObjectItemCaseSensitive(json, "cutOff")->valuedouble));
    spot->setOuterCutOff(static_cast<float>(cJSON_GetObjectItemCaseSensitive(json, "outerCutOff")->valuedouble));
}

void LightSpotSerializer::LoadFileIntoScene(const std::string &file)
{
    auto o = new LightSpot(
        glm::vec4(0, 0, 1, 0),
        glm::vec4(0.05f, 0.05f, 0.05f, 0),
        glm::vec4(1.0f, 0.0f, 0.0f, 0),
        glm::vec4(1.0f, 1.0f, 1.0f, 0),
        1.0f,
        0.09f,
        0.032f,
        0.9763,
        0.9659
    );

    o->setPosition(ComponentsManager::get()->getComponentCamera()->getCamera()->getPosition());

    Brakeza::get()->addObject3D(o, Brakeza::uniqueObjectLabel("SpotLight3D"));
}
