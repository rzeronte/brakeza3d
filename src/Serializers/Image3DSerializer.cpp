//
// Created by Eduardo on 24/11/2025.
//

#include "../../include/Serializers/Image3DSerializer.h"
#include "../../include/3D/Image3D.h"
#include "../../include/Serializers/Object3DSerializer.h"
#include "../../include/Brakeza.h"
#include "../../include/Components/Components.h"
#include "../../include/Misc/Logging.h"

cJSON * Image3DSerializer::JsonByObject(Object3D *o)
{
    Logging::Message("[Image3DSerializer] JsonByObject: %d", (int) o->getTypeObject());

    auto image = dynamic_cast<Image3D*>(o);

    auto root = Object3DSerializer().JsonByObject(o);

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
    Logging::Message("[Image3DSerializer] ApplyJsonToObject: %d", (int) o->getTypeObject());

    auto image = dynamic_cast<Image3D*>(o);

    Object3DSerializer().ApplyJsonToObject(json, o);
}

void Image3DSerializer::LoadFileIntoScene(const std::string &filename)
{
    auto *o = new Image3D(
        Components::get()->Camera()->getCamera()->getPosition(),
        1.0f,
        1.0f,
        new Image(filename)
    );

    Brakeza::get()->addObject3D(o, Brakeza::UniqueObjectLabel("Image3D"));
}
