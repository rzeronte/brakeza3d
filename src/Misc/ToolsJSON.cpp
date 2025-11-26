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

M3 ToolsJSON::parseRotation3DJSON(cJSON *rotationJSON)
{
    M3 MRX = M3::RX(static_cast<float>(cJSON_GetObjectItemCaseSensitive(rotationJSON, "x")->valuedouble));
    M3 MRY = M3::RY(static_cast<float>(cJSON_GetObjectItemCaseSensitive(rotationJSON, "y")->valuedouble));
    M3 MRZ = M3::RZ(static_cast<float>(cJSON_GetObjectItemCaseSensitive(rotationJSON, "z")->valuedouble));

    auto r = MRZ * MRY * MRX;
    M3::normalize(r);

    return r;
}

Color ToolsJSON::parseColorJSON(cJSON *color)
{
    return Color(
        cJSON_GetObjectItemCaseSensitive(color, "r")->valueint,
        cJSON_GetObjectItemCaseSensitive(color, "g")->valueint,
        cJSON_GetObjectItemCaseSensitive(color, "b")->valueint
    );
}

cJSON * ToolsJSON::ColorToJSON(const Color &color)
{
    cJSON *colorFromJSON = cJSON_CreateObject();
    cJSON_AddNumberToObject(colorFromJSON, "r", color.r);
    cJSON_AddNumberToObject(colorFromJSON, "g", color.g);
    cJSON_AddNumberToObject(colorFromJSON, "b", color.b);

    return colorFromJSON;
}
