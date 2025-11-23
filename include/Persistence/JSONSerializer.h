//
// Created by Eduardo on 23/11/2025.
//

#ifndef BRAKEZA3D_JSONSERIALIZER_H
#define BRAKEZA3D_JSONSERIALIZER_H

#include "../Misc/cJSON.h"
#include <string>

class Object3D;
class Mesh3D;

class JSONSerializer {
public:
    virtual ~JSONSerializer() = default;

    virtual cJSON* JsonByObject(Object3D* obj) = 0;
    virtual Object3D* ObjectByJson(cJSON* json) = 0;

    static void ApplyJsonToObject(cJSON* json, Object3D* obj) {};
    static void LoadFileIntoScene(const std::string& model) {};
};

#endif //BRAKEZA3D_JSONSERIALIZER_H