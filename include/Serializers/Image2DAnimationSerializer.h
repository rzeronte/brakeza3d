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
    Object3D* ObjectByJson(cJSON* json) override;
    cJSON* JsonByObject(Object3D *o) override;
    void ApplyJsonToObject(cJSON* json, Object3D* o) override;
    void LoadFileIntoScene(const std::string& file) override;
};

#endif //BRAKEZA3D_IMAGE2DANIMATIONSERIALIZER_H