//
// Created by Eduardo on 16/04/2026.
//

#ifndef BRAKEZA3D_SWARMSERIALIZER_H
#define BRAKEZA3D_SWARMSERIALIZER_H

#include "../Render/JSONSerializer.h"
#include "../3D/Object3D.h"

class SwarmSerializer : public JSONSerializer
{
public:
    Object3D* ObjectByJson(cJSON* json) override;
    cJSON* JsonByObject(Object3D *o) override;
    void ApplyJsonToObject(cJSON* json, Object3D* o) override;
    void MenuLoad(const std::string& file) override;
};

#endif //BRAKEZA3D_SWARMSERIALIZER_H
