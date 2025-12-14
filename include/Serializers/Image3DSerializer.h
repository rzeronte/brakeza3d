//
// Created by Eduardo on 24/11/2025.
//

#ifndef BRAKEZA3D_IMAGE3DSERIALIZER_H
#define BRAKEZA3D_IMAGE3DSERIALIZER_H

#include "../Render/JSONSerializer.h"
#include "../3D/Image3D.h"
#include "../3D/Object3D.h"

class Image3DSerializer : public JSONSerializer
{
public:
    Object3D* ObjectByJson(cJSON* json) override;
    cJSON* JsonByObject(Object3D *o) override;
    void ApplyJsonToObject(cJSON* json, Object3D* o) override;
    void MenuLoad(const std::string& file) override;

    static std::string ExtractFilePath(cJSON * json);

    static void ApplyImageFromFile(Image3D * image, cJSON * json);

    static void ApplySize(Image3D * image, cJSON * json);
};

#endif //BRAKEZA3D_IMAGE3DSERIALIZER_H