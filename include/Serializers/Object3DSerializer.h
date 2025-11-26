//
// Created by Eduardo on 23/11/2025.
//

#ifndef BRAKEZA3D_OBJECT3DJSERIALIZER_H
#define BRAKEZA3D_OBJECT3DJSERIALIZER_H

#include "JSONSerializer.h"
#include "../3D/Object3D.h"

class Object3D;

class Object3DSerializer : public JSONSerializer
{
public:
    Object3D* ObjectByJson(cJSON* json) override;
    cJSON* JsonByObject(Object3D *o) override;
    void ApplyJsonToObject(const cJSON* json, Object3D* o) override;
    void LoadFileIntoScene(const std::string& file) override;
};

#endif //BRAKEZA3D_OBJECT3DJSERIALIZER_H
