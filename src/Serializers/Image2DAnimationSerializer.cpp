//
// Created by Eduardo on 24/11/2025.
//

#include "../../include/Serializers/Image2DAnimationSerializer.h"
#include "../../include/Brakeza.h"
#include "../../include/2D/Image2DAnimation.h"
#include "../../include/Components/Components.h"
#include "../../include/Serializers/Object3DSerializer.h"
#include "../../include/Threads/ThreadJobLoadImage2DAnimation.h"

cJSON * Image2DAnimationSerializer::JsonByObject(Object3D *o)
{
    Logging::Message("[Image2DAnimationSerializer] JsonByObject: %d", o->getTypeObject());

    auto image = dynamic_cast<Image2DAnimation*>(o);

    auto root = Object3DSerializer().JsonByObject(o);

    cJSON_AddNumberToObject(root, "x", image->getX());
    cJSON_AddNumberToObject(root, "y", image->getY());

    cJSON *animationJSON = cJSON_CreateObject();
    cJSON_AddStringToObject(animationJSON, "sprite", image->getAnimation()->getBaseFilename().c_str());
    cJSON_AddNumberToObject(animationJSON, "width", image->getAnimation()->currentSpriteWidth);
    cJSON_AddNumberToObject(animationJSON, "height", image->getAnimation()->currentspriteHeight);
    cJSON_AddNumberToObject(animationJSON, "numberFrames", image->getAnimation()->numberFramesToLoad);
    cJSON_AddNumberToObject(animationJSON, "fps", image->getAnimation()->fps);

    cJSON_AddItemToObject(root, "animation", animationJSON);

    return root;
}

Object3D * Image2DAnimationSerializer::ObjectByJson(cJSON *json)
{
    auto animationJSON = cJSON_GetObjectItemCaseSensitive(json, "animation");

    auto *o = new Image2DAnimation(
        cJSON_GetObjectItemCaseSensitive(json, "x")->valueint,
        cJSON_GetObjectItemCaseSensitive(json, "y")->valueint,
        false,
        new TextureAnimated(
            cJSON_GetObjectItemCaseSensitive(animationJSON, "sprite")->valuestring,
            cJSON_GetObjectItemCaseSensitive(animationJSON, "width")->valueint,
            cJSON_GetObjectItemCaseSensitive(animationJSON, "height")->valueint,
            cJSON_GetObjectItemCaseSensitive(animationJSON, "numberFrames")->valueint,
            cJSON_GetObjectItemCaseSensitive(animationJSON, "fps")->valueint
        )
    );

    ApplyJsonToObject(json, o);

    Brakeza::get()->PoolCompute().enqueueWithMainThreadCallback(std::make_shared<ThreadJobLoadImage2DAnimation>(o, json));

    return o;
}

void Image2DAnimationSerializer::ApplyJsonToObject(cJSON *json, Object3D *o)
{
    Logging::Message("[Image2DAnimationSerializer] ApplyJsonToObject: %d", (int) o->getTypeObject());

    auto image = dynamic_cast<Image2DAnimation*>(o);

    Object3DSerializer().ApplyJsonToObject(json, o);
}

void Image2DAnimationSerializer::MenuLoad(const std::string &file)
{
    auto *o = new Image2DAnimation(
        Config::get()->screenWidth/2,
        Config::get()->screenHeight/2,
        false,
        new TextureAnimated(file,1,1,1,1)
    );

    o->setName(Brakeza::UniqueObjectLabel("Image2DAnimation"));
    o->setPosition(Components::get()->Camera()->getCamera()->forward().getScaled(2));

    auto json = Image2DAnimationSerializer::JsonByObject(o);
    Brakeza::get()->PoolCompute().enqueueWithMainThreadCallback(std::make_shared<ThreadJobLoadImage2DAnimation>(o, json));
}

void Image2DAnimationSerializer::ApplyAnimationLoad(Image2DAnimation *image)
{
    image->getAnimation()->LoadCurrentSetup();
}
