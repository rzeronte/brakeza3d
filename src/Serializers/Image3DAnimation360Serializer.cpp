//
// Created by Eduardo on 24/11/2025.
//

#include "../../include/Serializers/Image3DAnimation360Serializer.h"
#include "../../include/Brakeza.h"
#include "../../include/Misc/ToolsJSON.h"
#include "../../include/3D/Image3DAnimation360.h"
#include "../../include/Components/Components.h"
#include "../../include/Serializers/Object3DSerializer.h"
#include "../../include/Threads/ThreadJobLoadImage3DAnimation360.h"

cJSON * Image3DAnimation360Serializer::JsonByObject(Object3D *o)
{
    Logging::Message("[Image3DAnimation8DirectionsSerializer] JsonByObject: %d", o->getTypeObject());

    auto image = dynamic_cast<Image3DAnimation360*>(o);

    auto root = Object3DSerializer().JsonByObject(o);

    cJSON_AddNumberToObject(root, "width", image->width);
    cJSON_AddNumberToObject(root, "height", image->height);

    cJSON *animationsArrayJSON = cJSON_CreateArray();
    for (auto &a : image->animations) {
        if (!a->isLoaded()) continue;
        cJSON *animationJSON = cJSON_CreateObject();
        cJSON_AddStringToObject(animationJSON, "folder", a->getBaseFile().c_str());
        cJSON_AddNumberToObject(animationJSON, "frames", a->getNumFrames());
        cJSON_AddNumberToObject(animationJSON, "fps", a->getFps());
        cJSON_AddItemToArray(animationsArrayJSON, animationJSON);
    }
    cJSON_AddItemToObject(root, "animations", animationsArrayJSON);
    cJSON_AddNumberToObject(root, "currentIndexAnimation", image->currentIndexAnimation);

    return root;
}

Object3D * Image3DAnimation360Serializer::ObjectByJson(cJSON *json)
{
    auto *o = new Image3DAnimation360(Components::get()->Camera()->getCamera()->getPosition(), 1, 1);

    ApplyJsonToObject(json, o);

    Brakeza::get()->PoolCompute().enqueueWithMainThreadCallback(std::make_shared<ThreadJobLoadImage3DAnimation360>(o, json));

    return o;
}

void Image3DAnimation360Serializer::ApplyJsonToObject(cJSON *json, Object3D *o)
{
    Logging::Message("[Image3DAnimation8DirectionsSerializer] ApplyJsonToObject: %d", o->getTypeObject());

    auto image = dynamic_cast<Image3DAnimation360*>(o);

    Object3DSerializer().ApplyJsonToObject(json, o);

    image->width = (float) cJSON_GetObjectItemCaseSensitive(json, "width")->valueint;
    image->height = (float) cJSON_GetObjectItemCaseSensitive(json, "height")->valueint;
}

void Image3DAnimation360Serializer::MenuLoad(const std::string &file)
{
    auto o = new Image3DAnimation360(Components::get()->Camera()->getCamera()->getPosition(), 1, 1);
    o->setName(Brakeza::UniqueObjectLabel("Image3DAnimation360"));

    auto json = Image3DAnimation360Serializer::JsonByObject(o);
    Brakeza::get()->PoolCompute().enqueueWithMainThreadCallback(std::make_shared<ThreadJobLoadImage3DAnimation360>(o, json));
}

void Image3DAnimation360Serializer::LoadAnimationTexturesMainThread(Image3DAnimation360 *image, cJSON *json)
{
    image->LoadAnimations();
    image->UpdateBillboardSize();

    int animationIndex = cJSON_GetObjectItemCaseSensitive(json, "currentIndexAnimation")->valueint;
    image->setAnimation(animationIndex);
}

void Image3DAnimation360Serializer::ApplyCreateDirectionalAnimations(Image3DAnimation360 *image, cJSON *json)
{
    if (cJSON_GetObjectItemCaseSensitive(json, "animations") != nullptr) {
        cJSON *currentAnimation;
        cJSON_ArrayForEach(currentAnimation, cJSON_GetObjectItemCaseSensitive(json, "animations")) {
            auto folder = cJSON_GetObjectItemCaseSensitive(currentAnimation, "folder")->valuestring;
            auto frames = cJSON_GetObjectItemCaseSensitive(currentAnimation, "frames")->valueint;
            auto fps = cJSON_GetObjectItemCaseSensitive(currentAnimation, "fps")->valueint;

            image->CreateAnimationDirectional2D(folder, frames, fps, false, -1);
        }
    }
}