//
// Created by Eduardo on 23/11/2025.
//

#ifndef BRAKEZA3D_MESH3DSERIALIZER_H
#define BRAKEZA3D_MESH3DSERIALIZER_H

#include "JSONSerializer.h"
#include "../3D/Object3D.h"

class Object3DSerializer;

class Mesh3DSerializer : public JSONSerializer
{
public:
    cJSON* JsonByObject(Object3D* o) override;
    Object3D* ObjectByJson(cJSON* json) override;
    void ApplyJsonToObject(const cJSON* json, Object3D* o) override;
    void LoadFileIntoScene(const std::string& config) override;
};

#endif
