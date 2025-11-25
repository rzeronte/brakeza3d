//
// Created by Eduardo on 24/11/2025.
//

#include "../../include/Persistence/Image3DSerializer.h"
#include "../../include/3D/Image3D.h"
#include "../../include/Persistence/JSONSerializerRegistry.h"
#include "../../include/Persistence/Object3DSerializer.h"
#include "../../include/Brakeza.h"

cJSON * Image3DSerializer::JsonByObject(Object3D *o)
{
    auto image = dynamic_cast<Image3D*>(o);

    cJSON *root = JSONSerializerRegistry::GetJsonByObject(o);

    cJSON_AddNumberToObject(root, "width", image->width);
    cJSON_AddNumberToObject(root, "height", image->height);
    cJSON_AddStringToObject(root, "image", image->image->getFileName().c_str());

    return root;

}

Object3D * Image3DSerializer::ObjectByJson(cJSON *json)
{
    auto o = new Image3D(
        Vertex3D(),
        static_cast<float>(cJSON_GetObjectItemCaseSensitive(json, "width")->valueint),
        static_cast<float>(cJSON_GetObjectItemCaseSensitive(json, "height")->valueint),
        new Image(cJSON_GetObjectItemCaseSensitive(json, "image")->valuestring)
    );

    ApplyJsonToObject(json, o);

    return o;
}

void Image3DSerializer::ApplyJsonToObject(const cJSON *json, Object3D *o)
{
    std::cout << "[Mesh3DSerializer ApplyJsonToObject] " << o->getTypeObject() << std::endl;

    auto image = dynamic_cast<Image3D*>(o);

    Object3DSerializer::ApplyJsonToObject(json, o);
}

void Image3DSerializer::LoadFileIntoScene(const std::string &filename)
{
    auto *o = new Image3D(
        ComponentsManager::get()->getComponentCamera()->getCamera()->getPosition(),
        1.0f,
        1.0f,
        new Image(filename)
    );

    Brakeza::get()->addObject3D(o, Brakeza::uniqueObjectLabel("Image3D"));
}
