//
// Created by Eduardo on 23/11/2025.
//

#include "../../include/Persistence/JSONSerializerRegistry.h"
#include "../../include/Objects/Object3D.h"

cJSON* JSONSerializerRegistry::serialize(Object3D* obj)
{
    if (obj == nullptr) return nullptr;

    std::cout << "[JSONSerializerRegistry serialize] Serializing object of type " << obj->getLabel().c_str() << std::endl;

    const char* typeName = obj->getTypeObject();
    auto serializer = getSerializer(typeName);

    if (!serializer) {
        // No hay serializador registrado para este tipo
        return nullptr;
    }

    return serializer->JsonByObject(obj);
}

Object3D* JSONSerializerRegistry::deserialize(cJSON* json)
{
    if (!json) return nullptr;

    cJSON* typeItem = cJSON_GetObjectItem(json, "type");

    std::cout << "[JSONSerializerRegistry deserialize] Deserializing object of type " << typeItem->valuestring << std::endl;

    const char* typeName = typeItem->valuestring;
    auto serializer = getSerializer(typeName);

    if (!serializer) {
        std::cout << "[JSONSerializerRegistry deserialize] No serialization for " << typeItem->valuestring << std::endl;
        return nullptr;
    }

    return serializer->ObjectByJson(json);
}