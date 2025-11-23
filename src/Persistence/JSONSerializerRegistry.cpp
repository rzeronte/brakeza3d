//
// Created by Eduardo on 23/11/2025.
//

#include "../../include/Persistence/JSONSerializerRegistry.h"
#include "../../include/Objects/Object3D.h"

cJSON* JSONSerializerRegistry::serialize(Object3D* obj)
{
    if (obj == nullptr) return nullptr;

    std::cout << "[JSONSerializerRegistry serialize] Serializing object of type " << obj->getTypeObject() << std::endl;

    auto serializer = getSerializer(obj->getTypeObject());

    if (!serializer) {
        std::cout << "[JSONSerializerRegistry serialize] No serialization for " << obj->getTypeObject() << std::endl;
        exit(-1);
    }

    return serializer->JsonByObject(obj);
}

Object3D* JSONSerializerRegistry::deserialize(cJSON* json)
{
    if (!json) return nullptr;

    cJSON* typeItem = cJSON_GetObjectItem(json, "type");

    std::cout << "[JSONSerializerRegistry deserialize] Deserializing object of type " << typeItem->valuestring << std::endl;

    auto serializer = getSerializer(typeItem->valuestring);

    if (!serializer) {
        std::cout << "[JSONSerializerRegistry deserialize] No serialization for " << typeItem->valuestring << std::endl;
        exit(-1);
    }

    return serializer->ObjectByJson(json);
}