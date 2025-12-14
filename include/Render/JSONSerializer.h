//
// Created by Eduardo on 23/11/2025.
//

#ifndef BRAKEZA3D_JSONSERIALIZER_H
#define BRAKEZA3D_JSONSERIALIZER_H

#include "../Misc/cJSON.h"
#include <string>

class Object3D;

class JSONSerializer {
public:
    virtual ~JSONSerializer() = default;
    virtual Object3D* ObjectByJson(cJSON* json) = 0;
    virtual cJSON* JsonByObject(Object3D* obj) = 0;

    virtual void ApplyJsonToObject(cJSON* json, Object3D* o) = 0;
    virtual void MenuLoad(const std::string& model) = 0;
};

#endif //BRAKEZA3D_JSONSERIALIZER_H