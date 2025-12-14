//
// Created by Eduardo on 24/11/2025.
//

#ifndef BRAKEZA3D_IMAGE3DANIMATIONSERIALIZER_H
#define BRAKEZA3D_IMAGE3DANIMATIONSERIALIZER_H

#include "../Render/JSONSerializer.h"
#include "../3D/Image3DAnimation.h"

class Image3DAnimationSerializer : public JSONSerializer
{
public:
    Object3D* ObjectByJson(cJSON* json) override;
    cJSON* JsonByObject(Object3D *o) override;
    void ApplyJsonToObject(cJSON* json, Object3D* o) override;
    void MenuLoad(const std::string& file) override;

    static void ApplyAnimationsBackground(Image3DAnimation *image, cJSON* json);
    static void ApplyAnimationsMainThread(Image3DAnimation *image);
};

#endif //BRAKEZA3D_IMAGE3DANIMATIONSERIALIZER_H