//
// Created by Eduardo on 24/11/2025.
//

#include "../../include/Serializers/Image3DAnimation360Serializer.h"
#include "../../include/Serializers/JSONSerializerRegistry.h"
#include "../../include/Brakeza.h"
#include "../../include/Misc/ToolsJSON.h"
#include "../../include/3D/Image3DAnimation360.h"
#include "../../include/Components/Components.h"
#include "../../include/Serializers/Object3DSerializer.h"
#include "../../include/Serializers/Image3DAnimationSerializer.h"

cJSON * Image3DAnimation360Serializer::JsonByObject(Object3D *o)
{
    Logging::Message("[Image3DAnimation8DirectionsSerializer] JsonByObject: %d", (int) o->getTypeObject());

    auto image = dynamic_cast<Image3DAnimation360*>(o);

    auto root = Image3DAnimationSerializer().JsonByObject(o);

    cJSON_AddNumberToObject(root, "width", image->width);
    cJSON_AddNumberToObject(root, "height", image->height);

    cJSON *animationsArrayJSON = cJSON_CreateArray();
    for (auto a : image->animations) {
        if (!a->isLoaded()) continue;
        cJSON *animationJSON = cJSON_CreateObject();
        cJSON_AddStringToObject(animationJSON, "folder", a->getBaseFile().c_str());
        cJSON_AddNumberToObject(animationJSON, "frames", a->getNumFrames());
        cJSON_AddNumberToObject(animationJSON, "fps", a->getFps());
        cJSON_AddItemToArray(animationsArrayJSON, animationJSON);
    }
    cJSON_AddItemToObject(root, "animations", animationsArrayJSON);
    cJSON_AddNumberToObject(root, "currentIndexAnimation", image->currentAnimation);

    return root;
}

Object3D * Image3DAnimation360Serializer::ObjectByJson(cJSON *json)
{
    auto *o = new Image3DAnimation360(
        Components::get()->Camera()->getCamera()->getPosition(),
        1,
        1
    );

    ApplyJsonToObject(json, o);

    return o;
}

void Image3DAnimation360Serializer::ApplyJsonToObject(const cJSON *json, Object3D *o)
{
    Logging::Message("[Image3DAnimation8DirectionsSerializer] ApplyJsonToObject: %d", (int) o->getTypeObject());

    auto image = dynamic_cast<Image3DAnimation360*>(o);

    image->width = static_cast<float>(cJSON_GetObjectItemCaseSensitive(json, "width")->valueint);
    image->height = static_cast<float>(cJSON_GetObjectItemCaseSensitive(json, "height")->valueint);

    if (cJSON_GetObjectItemCaseSensitive(json, "animations") != nullptr) {
        cJSON *currentAnimation;
        cJSON_ArrayForEach(currentAnimation, cJSON_GetObjectItemCaseSensitive(json, "animations")) {
            auto folder = cJSON_GetObjectItemCaseSensitive(currentAnimation, "folder")->valuestring;
            auto frames = cJSON_GetObjectItemCaseSensitive(currentAnimation, "frames")->valueint;
            auto fps = cJSON_GetObjectItemCaseSensitive(currentAnimation, "fps")->valueint;

            image->addAnimationDirectional2D(folder, frames, fps, false, -1);
        }

        image->updateBillboardSize();

        int animationIndex = cJSON_GetObjectItemCaseSensitive(json, "currentIndexAnimation")->valueint;
        image->setAnimation(animationIndex);
        image->updateStep();
    }

    Object3DSerializer().ApplyJsonToObject(json, o);
}

void Image3DAnimation360Serializer::LoadFileIntoScene(const std::string &file)
{
    auto o = new Image3DAnimation360(
        Components::get()->Camera()->getCamera()->getPosition(),
        1,
        1
    );

    if (!file.empty()) {
        o->addAnimationDirectional2D(file, 1, 1, false, -1);
        o->setAnimation(0);
    }

    Brakeza::get()->addObject3D(o, Brakeza::UniqueObjectLabel("Billboard8D"));
}