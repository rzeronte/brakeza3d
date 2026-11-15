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

#ifndef BRAKEZA3D_LIGHTPOINTSERIALIZER_H
#define BRAKEZA3D_LIGHTPOINTSERIALIZER_H

#include "../Render/JSONSerializer.h"
#include "../3D/Object3D.h"

class LightPointSerializer : public JSONSerializer
{
public:
    Object3D* ObjectByJson(cJSON* json) override;
    cJSON* JsonByObject(Object3D *o) override;
    void ApplyJsonToObject(cJSON* json, Object3D* o) override;
    void MenuLoad(const std::string& model) override;
};


#endif //BRAKEZA3D_LIGHTPOINTSERIALIZER_H