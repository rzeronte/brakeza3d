//
// Created by Eduardo on 23/11/2025.
//

#include "../../include/Render/JSONSerializerRegistry.h"
#include "../../include/3D/Object3D.h"
#include "../../include/Misc/Logging.h"

cJSON* JSONSerializerRegistry::serialize(Object3D* obj)
{
    // std::lock_guard<std::mutex> lock(mtx);  // Bloquea

    if (obj == nullptr) return nullptr;

    LOG_MESSAGE("[JSONSerializerRegistry serialize] Serializing object of type: %d", (int) obj->getTypeObject());

    auto serializer = getSerializer(obj->getTypeObject());

    if (!serializer) {
        LOG_MESSAGE("[JSONSerializerRegistry serialize] No serialization for object type: %d ", (int) obj->getTypeObject());
        exit(-1);
    }

    return serializer->JsonByObject(obj);
}

Object3D* JSONSerializerRegistry::deserialize(cJSON* json)
{
    std::lock_guard<std::mutex> lock(mtx);  // Bloquea

    if (!json) return nullptr;

    cJSON* typeItem = cJSON_GetObjectItem(json, "type");

    if (!typeItem || !typeItem->valueint) {
        LOG_MESSAGE("[JSONSerializerRegistry deserialize] Missing or invalid 'type' field");
        return nullptr;
    }

    LOG_MESSAGE("[JSONSerializerRegistry deserialize] Deserializing object of type: %d", typeItem->valueint);

    auto serializer = getSerializer((ObjectType) typeItem->valueint);

    if (!serializer) {
        LOG_MESSAGE("[JSONSerializerRegistry deserialize] No serializer for object: %d", typeItem->valueint);
        exit(-1);
    }

    return serializer->ObjectByJson(json);
}