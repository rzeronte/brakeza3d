//
// Created by Eduardo on 24/11/2025.
//

#include "../../include/Serializers/Image3DAnimation8DirectionsSerializer.h"
#include "../../include/Serializers/JSONSerializerRegistry.h"
#include "../../include/Brakeza.h"
#include "../../include/Misc/ToolsJSON.h"
#include "../../include/3D/Image3DAnimation8Directions.h"

cJSON * Image3DAnimation8DirectionsSerializer::JsonByObject(Object3D *o)
{
    Logging::Message("[Image3DAnimation8DirectionsSerializer] JsonByObject: %s", o->getTypeObject());

    auto image = dynamic_cast<Image3DAnimation8Directions*>(o);

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

Object3D * Image3DAnimation8DirectionsSerializer::ObjectByJson(cJSON *json)
{
    auto *o = new Image3DAnimation8Directions(
        ComponentsManager::get()->getComponentCamera()->getCamera()->getPosition(),
        1,
        1
    );

    ApplyJsonToObject(json, o);

    return o;
}

void Image3DAnimation8DirectionsSerializer::ApplyJsonToObject(const cJSON *json, Object3D *o)
{
    Logging::Message("[Image3DAnimation8DirectionsSerializer] ApplyJsonToObject: %s", o->getTypeObject());

    auto image = dynamic_cast<Image3DAnimation8Directions*>(o);

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

void Image3DAnimation8DirectionsSerializer::LoadFileIntoScene(const std::string &file)
{
    auto o = new Image3DAnimation8Directions(
        ComponentsManager::get()->getComponentCamera()->getCamera()->getPosition(),
        1,
        1
    );

    if (!file.empty()) {
        o->addAnimationDirectional2D(file, 1, 1, false, -1);
        o->setAnimation(0);
    }

    Brakeza::get()->addObject3D(o, Brakeza::uniqueObjectLabel("Billboard8D"));
}