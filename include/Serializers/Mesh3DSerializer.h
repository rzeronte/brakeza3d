//
// Created by Eduardo on 23/11/2025.
//

#ifndef BRAKEZA3D_MESH3DSERIALIZER_H
#define BRAKEZA3D_MESH3DSERIALIZER_H

#include <mutex>

#include "JSONSerializer.h"
#include "../3D/Mesh3D.h"
#include "../3D/Object3D.h"

class Object3DSerializer;

class Mesh3DSerializer : public JSONSerializer
{
    std::mutex mtx;
public:
    cJSON* JsonByObject(Object3D* o) override;
    Object3D* ObjectByJson(cJSON* json) override;
    void ApplyJsonToObject(cJSON* json, Object3D* o) override;
    void MenuLoad(const std::string& config) override;

    static void ApplyGeometryFromFile(Mesh3D *m, cJSON* json);
    static void ApplyCollider(Mesh3D* m, cJSON* json);
    static void ApplyShadersCreation(Mesh3D* m, cJSON* json);
    static void ApplyShadersBackground(Mesh3D* m);
    static void ApplyCustomShadersMainThread(Mesh3D* m);
    static const char* ExtractFileModelPath(cJSON* json);
};

#endif
