//
// Created by Eduardo on 24/11/2025.
//

#include "../../include/Serializers/LightSpotSerializer.h"

#include <glm/detail/func_trigonometric.inl>
#include "../../include/3D/LightSpot.h"
#include "../../include/Brakeza.h"
#include "../../include/Components/Components.h"
#include "../../include/Render/JSONSerializerRegistry.h"
#include "../../include/Misc/ToolsJSON.h"
#include "../../include/Serializers/LightPointSerializer.h"
#include "../../include/Threads/ThreadJobLoadLightSpot.h"

cJSON * LightSpotSerializer::JsonByObject(Object3D *o)
{
    Logging::Message("[LightSpotSerializer] JsonByObject: %d", (int) o->getTypeObject());

    auto *spot = dynamic_cast<LightSpot*>(o);

    auto root = LightPointSerializer().JsonByObject(o);

    cJSON_AddNumberToObject(root, "cutOff", spot->getCutOff());
    cJSON_AddNumberToObject(root, "outerCutOff", spot->getOuterCutOff());

    return root;
}

Object3D * LightSpotSerializer::ObjectByJson(cJSON *json)
{
    auto o = new LightSpot(
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

    Brakeza::get()->PoolCompute().enqueueWithMainThreadCallback(std::make_shared<ThreadJobLoadLightSpot>(o, json));

    return o;
}

void LightSpotSerializer::ApplyJsonToObject(cJSON *json, Object3D *o)
{
    auto spot = dynamic_cast<LightSpot*>(o);

    LightPointSerializer().ApplyJsonToObject(json, o);

    spot->setCutOff(static_cast<float>(cJSON_GetObjectItemCaseSensitive(json, "cutOff")->valuedouble));
    spot->setOuterCutOff(static_cast<float>(cJSON_GetObjectItemCaseSensitive(json, "outerCutOff")->valuedouble));
}

void LightSpotSerializer::MenuLoad(const std::string &file)
{
    auto o = new LightSpot(
        glm::vec4(0.05f, 0.05f, 0.05f, 0),
        glm::vec4(1.0f, 0.0f, 0.0f, 0),
        glm::vec4(1.0f, 1.0f, 1.0f, 0),
        1.0f,
        0.09f,
        0.032f,
        0.9763,
        0.9659
    );

    o->setName(Brakeza::UniqueObjectLabel("LightSpot"));
    o->setPosition(Components::get()->Camera()->getCamera()->getPosition());

    auto json = LightSpotSerializer::JsonByObject(o);
    Brakeza::get()->PoolCompute().enqueueWithMainThreadCallback(std::make_shared<ThreadJobLoadLightSpot>(o, json));
}
