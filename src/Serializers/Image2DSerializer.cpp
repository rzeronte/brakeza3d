//
// Created by Eduardo on 24/11/2025.
//

#include "../../include/Serializers/Image2DSerializer.h"
#include "../../include/2D/Image2D.h"
#include "../../include/Brakeza.h"
#include "../../include/Components/Components.h"
#include "../../include/Serializers/Object3DSerializer.h"
#include "../../include/Threads/ThreadJobLoadImage2D.h"

cJSON * Image2DSerializer::JsonByObject(Object3D *o)
{
    Logging::Message("[Image2DSerializer] JsonByObject: %d", (int) o->getTypeObject());

    auto image = dynamic_cast<Image2D*>(o);

    auto root = Object3DSerializer().JsonByObject(o);

    cJSON_AddNumberToObject(root, "x", image->x);
    cJSON_AddNumberToObject(root, "y", image->y);
    cJSON_AddNumberToObject(root, "width", image->width);
    cJSON_AddNumberToObject(root, "height", image->height);

    cJSON_AddStringToObject(root, "image", image->filepath.c_str());

    return root;
}

Object3D * Image2DSerializer::ObjectByJson(cJSON *json)
{
    auto o = new Image2D();

    ApplyJsonToObject(json, o);

    Brakeza::get()->Pool().enqueueWithMainThreadCallback(std::make_shared<ThreadJobLoadImage2D>(o, json));

    return o;
}

void Image2DSerializer::ApplyJsonToObject(cJSON *json, Object3D *o)
{
    Logging::Message("[Image2DSerializer] ApplyJsonToObject: %d", o->getTypeObject());

    auto image = dynamic_cast<Image2D*>(o);

    image->setFilePath(cJSON_GetObjectItemCaseSensitive(json, "image")->valuestring);
    image->setSize(
        cJSON_GetObjectItemCaseSensitive(json, "width")->valueint,
        cJSON_GetObjectItemCaseSensitive(json, "height")->valueint
    );
    image->setScreenPosition(
        cJSON_GetObjectItemCaseSensitive(json, "x")->valueint,
        cJSON_GetObjectItemCaseSensitive(json, "y")->valueint
    );

    Object3DSerializer().ApplyJsonToObject(json, o);
}

void Image2DSerializer::MenuLoad(const std::string &file)
{
    auto *o = new Image2D(file, Config::get()->screenWidth/4, Config::get()->screenHeight/4);

    const auto x = Config::get()->screenWidth/2 - 100;
    const auto y = Config::get()->screenHeight/2 - 100;

    o->setPosition(Components::get()->Camera()->getCamera()->getPosition());
    o->setScreenPosition(x, y);
    o->setName(Brakeza::UniqueObjectLabel("Image2D"));

    auto json = Image2DSerializer::JsonByObject(o);
    Brakeza::get()->Pool().enqueueWithMainThreadCallback(std::make_shared<ThreadJobLoadImage2D>(o, json));
}

const char* Image2DSerializer::ExtractFilePath(cJSON *json)
{
    return cJSON_GetObjectItemCaseSensitive(json, "image")->valuestring;
}

void Image2DSerializer::ApplyImageFromFile(Image2D *image)
{
    image->setImage(new Image(image->filepath.c_str()));
}
