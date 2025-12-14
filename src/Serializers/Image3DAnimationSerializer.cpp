//
// Created by Eduardo on 24/11/2025.
//

#include "../../include/Serializers/Image3DAnimationSerializer.h"
#include "../../include/Serializers/JSONSerializerRegistry.h"
#include "../../include/Brakeza.h"
#include "../../include/3D/Image3DAnimation.h"
#include "../../include/Serializers/Object3DSerializer.h"
#include "../../include/Components/Components.h"
#include "../../include/Threads/ThreadJobLoadImage3DAnimation.h"

cJSON * Image3DAnimationSerializer::JsonByObject(Object3D *o)
{
    Logging::Message("[Image3DAnimationSerializer] JsonByObject: %d", (int) o->getTypeObject());

    auto image = dynamic_cast<Image3DAnimation*>(o);

    auto root = Object3DSerializer().JsonByObject(o);

    cJSON_AddNumberToObject(root, "width", image->width);
    cJSON_AddNumberToObject(root, "height", image->height);

    cJSON *animationsArrayJSON = cJSON_CreateArray();
    for (auto a : image->animations) {
        cJSON *animationJSON = cJSON_CreateObject();
        cJSON_AddStringToObject(animationJSON, "sprite", a->getBaseFilename().c_str());
        cJSON_AddNumberToObject(animationJSON, "width", (int) a->currentSpriteWidth);
        cJSON_AddNumberToObject(animationJSON, "height", (int) a->currentspriteHeight);
        cJSON_AddNumberToObject(animationJSON, "numberFrames", (int) a->numberFramesToLoad);
        cJSON_AddNumberToObject(animationJSON, "fps", (int) a->fps);
        cJSON_AddItemToArray(animationsArrayJSON, animationJSON);
    }

    cJSON_AddItemToObject(root, "animations", animationsArrayJSON);
    cJSON_AddNumberToObject(root, "currentIndexAnimation", image->currentAnimationIndex);

    return root;
}

Object3D * Image3DAnimationSerializer::ObjectByJson(cJSON *json)
{
    auto *o = new Image3DAnimation(Components::get()->Camera()->getCamera()->getPosition(), 1, 1);
    ApplyJsonToObject(json, o);
    Brakeza::get()->Pool().enqueueWithMainThreadCallback(std::make_shared<ThreadJobLoadImage3DAnimation>(o, json));
    return o;
}

void Image3DAnimationSerializer::ApplyJsonToObject(cJSON *json, Object3D *o)
{
    Logging::Message("[Image3DAnimationSerializer] ApplyJsonToObject: %d", (int) o->getTypeObject());

    auto image = dynamic_cast<Image3DAnimation*>(o);

    Object3DSerializer().ApplyJsonToObject(json, o);
    if (cJSON_GetObjectItemCaseSensitive(json, "animations") != nullptr) {
        image->width = (float) cJSON_GetObjectItemCaseSensitive(json, "width")->valuedouble;
        image->height = (float) cJSON_GetObjectItemCaseSensitive(json, "height")->valuedouble;

        image->setAnimation(cJSON_GetObjectItemCaseSensitive(json, "currentIndexAnimation")->valueint);
    }
}

void Image3DAnimationSerializer::MenuLoad(const std::string &file)
{
    auto *o = new Image3DAnimation(Components::get()->Camera()->getCamera()->getPosition(), 1, 1);
    o->setName(Brakeza::UniqueObjectLabel("Image3DAnimation"));

    o->CreateAnimation(file,1,1,1,1);
    o->setAnimation(0);
    o->setPosition(Components::get()->Camera()->getCamera()->forward().getScaled(2));

    auto json = Image3DAnimationSerializer::JsonByObject(o);
    Brakeza::get()->Pool().enqueueWithMainThreadCallback(std::make_shared<ThreadJobLoadImage3DAnimation>(o, json));
}

void Image3DAnimationSerializer::ApplyAnimationsBackground(Image3DAnimation *image, cJSON* json)
{
    if (cJSON_GetObjectItemCaseSensitive(json, "animations") != nullptr) {
        cJSON *currentAnimation;
        cJSON_ArrayForEach(currentAnimation, cJSON_GetObjectItemCaseSensitive(json, "animations")) {

            auto spriteFile = cJSON_GetObjectItemCaseSensitive(currentAnimation, "sprite")->valuestring;
            auto width = cJSON_GetObjectItemCaseSensitive(currentAnimation, "width")->valueint;
            auto height = cJSON_GetObjectItemCaseSensitive(currentAnimation, "height")->valueint;
            auto fps = cJSON_GetObjectItemCaseSensitive(currentAnimation, "fps")->valueint;
            auto numberFrames = cJSON_GetObjectItemCaseSensitive(currentAnimation, "numberFrames")->valueint;

            image->CreateAnimation(spriteFile, width, height, numberFrames, fps);
        }
    }
}

void Image3DAnimationSerializer::ApplyAnimationsMainThread(Image3DAnimation *image)
{
    image->LoadAnimationFiles();
    image->UpdateBillboardSize();
}
