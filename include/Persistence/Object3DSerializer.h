//
// Created by Eduardo on 23/11/2025.
//

#ifndef BRAKEZA3D_OBJECT3DJSERIALIZER_H
#define BRAKEZA3D_OBJECT3DJSERIALIZER_H

#include "JSONSerializer.h"
#include "../Objects/Object3D.h"

class Object3D;

class Object3DSerializer : public JSONSerializer {
public:
    cJSON* JsonByObject(Object3D *object) override;
    Object3D* ObjectByJson(cJSON* json) override;
    void ApplyJsonToObject(cJSON* json, Object3D* obj) override;

    static void LoadFileIntoScene(const std::string& file);
};

#endif //BRAKEZA3D_OBJECT3DJSERIALIZER_H
