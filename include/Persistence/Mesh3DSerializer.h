//
// Created by Eduardo on 23/11/2025.
//

#ifndef BRAKEZA3D_MESH3DSERIALIZER_H
#define BRAKEZA3D_MESH3DSERIALIZER_H

#include "JSONSerializer.h"
#include "../Objects/Object3D.h"


class Mesh3DSerializer : public JSONSerializer
{
    cJSON* JsonByObject(Object3D *object) override;
    Object3D* ObjectByJson(cJSON* json) override;
    void ApplyJsonToObject(cJSON* json, Object3D* obj) override;

    static void LoadFileIntoScene(const std::string& model);
};

#endif
