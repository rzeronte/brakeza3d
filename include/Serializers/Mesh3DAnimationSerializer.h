//
// Created by Eduardo on 24/11/2025.
//

#ifndef BRAKEZA3D_MESH3DANIMATIONSERIALIZER_H
#define BRAKEZA3D_MESH3DANIMATIONSERIALIZER_H

#include "JSONSerializer.h"
#include "../3D/Mesh3DAnimation.h"
#include "../3D/Object3D.h"

class Mesh3DAnimationSerializer : public JSONSerializer
{
public:
    Object3D* ObjectByJson(cJSON* json) override;
    cJSON* JsonByObject(Object3D *o) override;
    void ApplyJsonToObject(cJSON* json, Object3D* o) override;
    void LoadFileIntoScene(const std::string& file) override;

    static void ApplyBonesColliders(Mesh3DAnimation *mesh, cJSON *json);
    static void ApplyGeometryAnimationFromFile(Mesh3DAnimation *m, cJSON *json);

    static const char *ExtractFileModelPath(cJSON *json);
};

#endif //BRAKEZA3D_MESH3DANIMATIONSERIALIZER_H