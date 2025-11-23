//
// Created by Eduardo on 23/11/2025.
//

#ifndef BRAKEZA3D_JSONSERIALIZERREGISTRY_H
#define BRAKEZA3D_JSONSERIALIZERREGISTRY_H

#include "JSONSerializer.h"
#include <unordered_map>
#include <memory>
#include <string>
#include "../Objects/Object3D.h"

class JSONSerializerRegistry
{
    std::unordered_map<std::string, std::shared_ptr<JSONSerializer>> serializers;
    JSONSerializerRegistry() = default;
public:
    JSONSerializerRegistry(const JSONSerializerRegistry&) = delete;
    JSONSerializerRegistry& operator=(const JSONSerializerRegistry&) = delete;

    static JSONSerializerRegistry& instance()
    {
        static JSONSerializerRegistry registry;
        return registry;
    }

    void registerSerializer(const std::string& type, std::shared_ptr<JSONSerializer> serializer)
    {
        serializers[type] = serializer;
    }

    JSONSerializer* getSerializer(const std::string& type)
    {
        auto it = serializers.find(type);
        return (it != serializers.end()) ? it->second.get() : nullptr;
    }

    static cJSON * GetJsonByObject(Object3D *object)
    {
        std::cout << "[JSONSerializerRegistry GetJsonByObject] " << object->getTypeObject() << std::endl;
        if (auto serializer = instance().getSerializer(object->getTypeObject())) {
            return serializer->JsonByObject(object);
        }

        return nullptr;
    }

    cJSON* serialize(Object3D* obj);
    Object3D* deserialize(cJSON* json);
};

#endif //BRAKEZA3D_JSONSERIALIZERREGISTRY_H