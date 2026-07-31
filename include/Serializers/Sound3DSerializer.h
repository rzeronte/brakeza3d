//
// Created by Eduardo on 10/07/2026.
//

#ifndef BRAKEZA3D_SOUND3DSERIALIZER_H
#define BRAKEZA3D_SOUND3DSERIALIZER_H

#include "../Render/JSONSerializer.h"
#include "../3D/Object3D.h"

class Sound3DSerializer : public JSONSerializer
{
public:
    Object3D* ObjectByJson(cJSON* json) override;
    cJSON*    JsonByObject(Object3D* o)  override;
    void      ApplyJsonToObject(cJSON* json, Object3D* o) override;
    void      MenuLoad(const std::string& model) override;
};

#endif //BRAKEZA3D_SOUND3DSERIALIZER_H
