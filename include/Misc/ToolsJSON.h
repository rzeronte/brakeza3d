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
// Created by Eduardo on 18/01/2025.
//

#ifndef BRAKEZA3D_TOOLSJSON_H
#define BRAKEZA3D_TOOLSJSON_H


#include "../3D/Vertex3D.h"
#include "cJSON.h"
#include "../Render/Color.h"
#include "../include/Render/M3.h"

class ToolsJSON {

public:
    static Color getColorByJSON(cJSON *color);
    static cJSON* ColorToJSON(const Color &color);

    static cJSON *Vertex3DToJSON(const Vertex3D &v);

    static Vertex3D getVertex3DByJSON(cJSON *vertex3DJSON);
    static M3 getRotationByJSON(cJSON *rotationJSON);
};


#endif //BRAKEZA3D_TOOLSJSON_H
