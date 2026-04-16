//
// Created by Eduardo on 16/04/2026.
//

#include "../../include/Serializers/SwarmSerializer.h"
#include "../../include/Brakeza.h"
#include "../../include/3D/Swarm.h"
#include "../../include/Components/Components.h"
#include "../../include/Misc/ToolsJSON.h"
#include "../../include/Serializers/Object3DSerializer.h"
#include "../../include/Threads/ThreadJobLoadSwarm.h"

cJSON* SwarmSerializer::JsonByObject(Object3D *o)
{
    LOG_MESSAGE("[SwarmSerializer] JsonByObject: %d", o->getTypeObject());

    auto swarm = dynamic_cast<Swarm*>(o);
    auto root = Object3DSerializer().JsonByObject(o);

    cJSON_AddItemToObject(root, "size", ToolsJSON::Vertex3DToJSON(swarm->size));
    cJSON_AddBoolToObject(root, "debug", swarm->debug);

    cJSON_AddNumberToObject(root, "separationWeight",       swarm->separationWeight);
    cJSON_AddNumberToObject(root, "alignmentWeight",        swarm->alignmentWeight);
    cJSON_AddNumberToObject(root, "cohesionWeight",         swarm->cohesionWeight);
    cJSON_AddNumberToObject(root, "neighborDist",           swarm->neighborDist);
    cJSON_AddNumberToObject(root, "maxSpeed",               swarm->maxSpeed);
    cJSON_AddNumberToObject(root, "velocityBoidsFactor",    swarm->velocityBoidsFactor);
    cJSON_AddNumberToObject(root, "turnFactor",             swarm->turnFactor);
    cJSON_AddNumberToObject(root, "centerThreshold",        swarm->centerThreshold);
    cJSON_AddNumberToObject(root, "centerAttractionWeight", swarm->centerAttractionWeight);
    cJSON_AddNumberToObject(root, "predatorThreshold",      swarm->predatorThreshold);
    cJSON_AddNumberToObject(root, "predatorAvoidanceWeight",swarm->predatorAvoidanceWeight);
    cJSON_AddNumberToObject(root, "globalBiasSeparation",   swarm->globalBiasSeparation);
    cJSON_AddNumberToObject(root, "globalBiasAlignment",    swarm->globalBiasAlignment);
    cJSON_AddNumberToObject(root, "globalBiasCohesion",     swarm->globalBiasCohesion);

    cJSON* boidsArray = cJSON_CreateArray();
    for (auto* sw : swarm->objects)
        cJSON_AddItemToArray(boidsArray, cJSON_CreateString(sw->object->getName().c_str()));
    cJSON_AddItemToObject(root, "boids", boidsArray);

    cJSON* predatorsArray = cJSON_CreateArray();
    for (auto* sw : swarm->predators)
        cJSON_AddItemToArray(predatorsArray, cJSON_CreateString(sw->object->getName().c_str()));
    cJSON_AddItemToObject(root, "predators", predatorsArray);

    return root;
}

Object3D* SwarmSerializer::ObjectByJson(cJSON *json)
{
    auto *o = new Swarm(
        Components::get()->Camera()->getCamera()->getPosition(),
        Vertex3D(10, 10, 10)
    );

    ApplyJsonToObject(json, o);

    Brakeza::get()->PoolCompute().enqueueWithMainThreadCallback(std::make_shared<ThreadJobLoadSwarm>(o, json));

    return o;
}

void SwarmSerializer::ApplyJsonToObject(cJSON *json, Object3D *o)
{
    LOG_MESSAGE("[SwarmSerializer] ApplyJsonToObject: %d", o->getTypeObject());

    auto swarm = dynamic_cast<Swarm*>(o);

    Object3DSerializer().ApplyJsonToObject(json, o);

    auto sizeJSON = cJSON_GetObjectItemCaseSensitive(json, "size");
    if (sizeJSON) swarm->size = ToolsJSON::getVertex3DByJSON(sizeJSON);

    auto debugItem = cJSON_GetObjectItemCaseSensitive(json, "debug");
    if (debugItem) swarm->debug = cJSON_IsTrue(debugItem);

    auto readFloat = [&](const char* key, float& target) {
        auto item = cJSON_GetObjectItemCaseSensitive(json, key);
        if (item) target = static_cast<float>(item->valuedouble);
    };

    readFloat("separationWeight",       swarm->separationWeight);
    readFloat("alignmentWeight",        swarm->alignmentWeight);
    readFloat("cohesionWeight",         swarm->cohesionWeight);
    readFloat("neighborDist",           swarm->neighborDist);
    readFloat("maxSpeed",               swarm->maxSpeed);
    readFloat("velocityBoidsFactor",    swarm->velocityBoidsFactor);
    readFloat("turnFactor",             swarm->turnFactor);
    readFloat("centerThreshold",        swarm->centerThreshold);
    readFloat("centerAttractionWeight", swarm->centerAttractionWeight);
    readFloat("predatorThreshold",      swarm->predatorThreshold);
    readFloat("predatorAvoidanceWeight",swarm->predatorAvoidanceWeight);
    readFloat("globalBiasSeparation",   swarm->globalBiasSeparation);
    readFloat("globalBiasAlignment",    swarm->globalBiasAlignment);
    readFloat("globalBiasCohesion",     swarm->globalBiasCohesion);

    auto* boidsJSON = cJSON_GetObjectItemCaseSensitive(json, "boids");
    if (boidsJSON) {
        cJSON* item = nullptr;
        cJSON_ArrayForEach(item, boidsJSON)
            swarm->pendingBoidNames.emplace_back(item->valuestring);
    }

    auto* predatorsJSON = cJSON_GetObjectItemCaseSensitive(json, "predators");
    if (predatorsJSON) {
        cJSON* item = nullptr;
        cJSON_ArrayForEach(item, predatorsJSON)
            swarm->pendingPredatorNames.emplace_back(item->valuestring);
    }
}

void SwarmSerializer::MenuLoad(const std::string &file)
{
    auto *o = new Swarm(
        Components::get()->Camera()->getCamera()->getPosition(),
        Vertex3D(10, 10, 10)
    );

    o->setName(Brakeza::UniqueObjectLabel("Swarm"));
    o->setPosition(Components::get()->Camera()->getCamera()->getPosition());

    auto json = SwarmSerializer::JsonByObject(o);
    Brakeza::get()->PoolCompute().enqueueWithMainThreadCallback(std::make_shared<ThreadJobLoadSwarm>(o, json));
}
