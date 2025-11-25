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
    static Color parseColorJSON(cJSON *color);

    static Vertex3D parseVertex3DJSON(cJSON *vertex3DJSON);

    static M3 parseRotation3DJSON(cJSON *rotationJSON);
};


#endif //BRAKEZA3D_TOOLSJSON_H
