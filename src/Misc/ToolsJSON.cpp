//
// Created by Eduardo on 18/01/2025.
//

#include "../../include/Misc/ToolsJSON.h"

Vertex3D ToolsJSON::parseVertex3DJSON(cJSON *vertex3DJSON)
{
    const auto x = (float) cJSON_GetObjectItemCaseSensitive(vertex3DJSON, "x")->valuedouble;
    const auto y = (float) cJSON_GetObjectItemCaseSensitive(vertex3DJSON, "y")->valuedouble;
    const auto z = (float) cJSON_GetObjectItemCaseSensitive(vertex3DJSON, "z")->valuedouble;

    return {x, y, z};
}

Color ToolsJSON::parseColorJSON(cJSON *color)
{
    return Color(
        cJSON_GetObjectItemCaseSensitive(color, "r")->valueint,
        cJSON_GetObjectItemCaseSensitive(color, "g")->valueint,
        cJSON_GetObjectItemCaseSensitive(color, "b")->valueint
    );
}