//
// Created by Eduardo on 23/11/2025.
//

#ifndef BRAKEZA3D_JSONSERIALIZERREGISTRY_H
#define BRAKEZA3D_JSONSERIALIZERREGISTRY_H

#include "JSONSerializer.h"
#include <unordered_map>
#include <memory>
#include <mutex>

#include "../3D/Object3D.h"
#include "../Misc/Logging.h"

class JSONSerializerRegistry
{
    std::unordered_map<ObjectType, std::shared_ptr<JSONSerializer>> serializers;
    JSONSerializerRegistry() = default;

    std::mutex mtx;

public:
    JSONSerializerRegistry(const JSONSerializerRegistry&) = delete;
    JSONSerializerRegistry& operator=(const JSONSerializerRegistry&) = delete;

    static JSONSerializerRegistry& instance()
    {
        static JSONSerializerRegistry registry;
        return registry;
    }

    void registerSerializer(ObjectType type, std::shared_ptr<JSONSerializer> serializer)
    {
        serializers[type] = serializer;
    }

    JSONSerializer* getSerializer(ObjectType type)
    {
        auto it = serializers.find(type);
        return (it != serializers.end()) ? it->second.get() : nullptr;
    }

    static cJSON * GetJsonByObject(Object3D *object)
    {
        if (!object) {
            Logging::Message("[JSONSerializerRegistry GetJsonByObject] Error: objeto es null");
            return nullptr;
        }

        auto type = object->getTypeObject();
        Logging::Message("[JSONSerializerRegistry GetJsonByObject] %d", type);

        auto serializer = instance().getSerializer(type);

        if (!serializer) {
            Logging::Message("[JSONSerializerRegistry GetJsonByObject] No serializer para tipo %s", type);
            return nullptr;
        }

        cJSON* json = serializer->JsonByObject(object);

        if (!json) {
            Logging::Message("[JSONSerializerRegistry GetJsonByObject] JsonByObject devolvió null para %s", type);
        }

        return json;
    }

    cJSON* serialize(Object3D* obj);
    Object3D* deserialize(cJSON* json);
};

#endif //BRAKEZA3D_JSONSERIALIZERREGISTRY_H