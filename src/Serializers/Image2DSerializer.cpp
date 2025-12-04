//
// Created by Eduardo on 24/11/2025.
//

#include "../../include/Serializers/Image2DSerializer.h"
#include "../../include/2D/Image2D.h"
#include "../../include/Serializers/JSONSerializerRegistry.h"
#include "../../include/Brakeza.h"
#include "../../include/Serializers/Object3DSerializer.h"

cJSON * Image2DSerializer::JsonByObject(Object3D *o)
{
    Logging::Message("[Image2DSerializer] JsonByObject: %s", o->getTypeObject());

    auto image = dynamic_cast<Image2D*>(o);

    auto root = Object3DSerializer().JsonByObject(o);

    cJSON_AddNumberToObject(root, "x", image->x);
    cJSON_AddNumberToObject(root, "y", image->y);
    cJSON_AddStringToObject(root, "image", image->image->getFileName().c_str());

    return root;
}

Object3D * Image2DSerializer::ObjectByJson(cJSON *json)
{
    auto o = new Image2D(
        cJSON_GetObjectItemCaseSensitive(json, "x")->valueint,
        cJSON_GetObjectItemCaseSensitive(json, "y")->valueint,
        new Image(cJSON_GetObjectItemCaseSensitive(json, "image")->valuestring)
    );

    ApplyJsonToObject(json, o);

    return o;
}

void Image2DSerializer::ApplyJsonToObject(const cJSON *json, Object3D *o)
{
    Logging::Message("[Image2DSerializer] ApplyJsonToObject: %s", o->getTypeObject());

    auto image = dynamic_cast<Image2D*>(o);

    Object3DSerializer().ApplyJsonToObject(json, o);
}

void Image2DSerializer::LoadFileIntoScene(const std::string &file)
{
    auto *o = new Image2D(
        BrakezaSetup::get()->screenWidth/2,
        BrakezaSetup::get()->screenHeight/2,
        new Image(file)
    );

    o->setPosition(ComponentsManager::get()->getComponentCamera()->getCamera()->getPosition());

    Brakeza::get()->addObject3D(o, Brakeza::UniqueObjectLabel("Image2D"));
}