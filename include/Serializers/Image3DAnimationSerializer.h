/*
 * Brakeza3D - 3D/2D game engine
 * Author:  Eduardo Rodríguez Álvarez
 * GitHub:  https://github.com/rzeronte/brakeza3d
 * Web:     https://brakeza.com
 * Started: 2018-09-30
 * Copyright (C) 2018-2026 Eduardo Rodríguez Álvarez
 * License: GNU General Public License v3.0 (see LICENSE)
 */
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