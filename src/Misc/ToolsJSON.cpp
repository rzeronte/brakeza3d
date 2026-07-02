//
// Created by Eduardo on 18/01/2025.
//

#include "../../include/Misc/ToolsJSON.h"

Vertex3D ToolsJSON::getVertex3DByJSON(cJSON *vertex3DJSON)
{
    const auto x = (float) cJSON_GetObjectItemCaseSensitive(vertex3DJSON, "x")->valuedouble;
    const auto y = (float) cJSON_GetObjectItemCaseSensitive(vertex3DJSON, "y")->valuedouble;
    const auto z = (float) cJSON_GetObjectItemCaseSensitive(vertex3DJSON, "z")->valuedouble;

    return {x, y, z};
}

M3 ToolsJSON::getRotationByJSON(cJSON *rotationJSON)
{
    M3 MRX = M3::RX(static_cast<float>(cJSON_GetObjectItemCaseSensitive(rotationJSON, "x")->valuedouble));
    M3 MRY = M3::RY(static_cast<float>(cJSON_GetObjectItemCaseSensitive(rotationJSON, "y")->valuedouble));
    M3 MRZ = M3::RZ(static_cast<float>(cJSON_GetObjectItemCaseSensitive(rotationJSON, "z")->valuedouble));

    auto r = MRZ * MRY * MRX;
    M3::normalize(r);

    return r;
}

Color ToolsJSON::getColorByJSON(cJSON *color)
{
    cJSON *rItem = cJSON_GetObjectItemCaseSensitive(color, "r");
    cJSON *gItem = cJSON_GetObjectItemCaseSensitive(color, "g");
    cJSON *bItem = cJSON_GetObjectItemCaseSensitive(color, "b");
    cJSON *aItem = cJSON_GetObjectItemCaseSensitive(color, "a");

    float r = rItem ? static_cast<float>(rItem->valuedouble) : 0.0f;
    float g = gItem ? static_cast<float>(gItem->valuedouble) : 0.0f;
    float b = bItem ? static_cast<float>(bItem->valuedouble) : 0.0f;
    float a = aItem ? static_cast<float>(aItem->valuedouble) : 1.0f;

    return Color(r, g, b, a);
}

cJSON * ToolsJSON::ColorToJSON(const Color &color)
{
    cJSON *json = cJSON_CreateObject();
    cJSON_AddNumberToObject(json, "r", color.r);
    cJSON_AddNumberToObject(json, "g", color.g);
    cJSON_AddNumberToObject(json, "b", color.b);
    cJSON_AddNumberToObject(json, "a", color.a);

    return json;
}

cJSON * ToolsJSON::Vertex3DToJSON(const Vertex3D &v)
{
    cJSON *json = cJSON_CreateObject();
    cJSON_AddNumberToObject(json, "x", v.x);
    cJSON_AddNumberToObject(json, "y", v.y);
    cJSON_AddNumberToObject(json, "z", v.z);

    return json;

}