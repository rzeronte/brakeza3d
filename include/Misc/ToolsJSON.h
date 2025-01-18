//
// Created by Eduardo on 18/01/2025.
//

#ifndef BRAKEZA3D_TOOLSJSON_H
#define BRAKEZA3D_TOOLSJSON_H


#include "../Objects/Vertex3D.h"
#include "cJSON.h"
#include "Color.h"

class ToolsJSON {

public:
    static Color parseColorJSON(cJSON *color);

    static Vertex3D parseVertex3DJSON(cJSON *vertex3DJSON);
};


#endif //BRAKEZA3D_TOOLSJSON_H
