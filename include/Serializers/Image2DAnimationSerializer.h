//
// Created by Eduardo on 24/11/2025.
//

#ifndef BRAKEZA3D_IMAGE2DANIMATIONSERIALIZER_H
#define BRAKEZA3D_IMAGE2DANIMATIONSERIALIZER_H

#include "JSONSerializer.h"
#include "../3D/Object3D.h"

class Image2DAnimationSerializer : public JSONSerializer
{
    public:
    cJSON* JsonByObject(Object3D *o) override;
    Object3D* ObjectByJson(cJSON* json) override;

    static void ApplyJsonToObject(const cJSON* json, Object3D* o);
    static void LoadFileIntoScene(const std::string& file);
};

#endif //BRAKEZA3D_IMAGE2DANIMATIONSERIALIZER_H