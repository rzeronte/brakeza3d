//
// Created by Eduardo on 24/11/2025.
//

#ifndef BRAKEZA3D_PARTICLEEMMITTERSERIALIZER_H
#define BRAKEZA3D_PARTICLEEMMITTERSERIALIZER_H

#include "../Render/JSONSerializer.h"
#include "../3D/Object3D.h"

class ParticleEmmitterSerializer  : public JSONSerializer
{
public:
    Object3D* ObjectByJson(cJSON* json) override;
    cJSON* JsonByObject(Object3D *o) override;;
    void ApplyJsonToObject(cJSON* json, Object3D* o) override;
    void MenuLoad(const std::string& file) override;
};

#endif //BRAKEZA3D_PARTICLEEMMITTERSERIALIZER_H