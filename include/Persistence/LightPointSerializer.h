//
// Created by Eduardo on 24/11/2025.
//

#ifndef BRAKEZA3D_LIGHTPOINTSERIALIZER_H
#define BRAKEZA3D_LIGHTPOINTSERIALIZER_H

#include "JSONSerializer.h"
#include "../Objects/Object3D.h"

class LightPointSerializer : public JSONSerializer
{
public:
    cJSON* JsonByObject(Object3D *o) override;
    Object3D* ObjectByJson(cJSON* json) override;

    static void ApplyJsonToObject(cJSON* json, Object3D* o);
    static void LoadFileIntoScene(const std::string& model);
};


#endif //BRAKEZA3D_LIGHTPOINTSERIALIZER_H