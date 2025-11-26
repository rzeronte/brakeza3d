//
// Created by Eduardo on 23/11/2025.
//

#include "../../include/Serializers/JSONSerializerRegistry.h"
#include "../../include/3D/Object3D.h"
#include "../../include/Misc/Logging.h"

cJSON* JSONSerializerRegistry::serialize(Object3D* obj)
{
    if (obj == nullptr) return nullptr;

    Logging::Message("[JSONSerializerRegistry serialize] Serializing object of type", obj->getTypeObject());

    auto serializer = getSerializer(obj->getTypeObject());

    if (!serializer) {
        Logging::Message("[JSONSerializerRegistry serialize] No serialization for object: ", obj->getTypeObject());
        exit(-1);
    }

    return serializer->JsonByObject(obj);
}

Object3D* JSONSerializerRegistry::deserialize(cJSON* json)
{
    if (!json) return nullptr;

    cJSON* typeItem = cJSON_GetObjectItem(json, "type");

    Logging::Message("[JSONSerializerRegistry serialize] Deserializing object of type: %s", typeItem->valuestring);

    auto serializer = getSerializer(typeItem->valuestring);

    if (!serializer) {
        Logging::Message("[JSONSerializerRegistry serialize] No serialization for object: %s", typeItem->valuestring);
        exit(-1);
    }

    return serializer->ObjectByJson(json);
}