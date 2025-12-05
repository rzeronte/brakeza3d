//
// Created by Eduardo on 24/11/2025.
//

#include "../../include/Serializers/Image3DAnimationSerializer.h"
#include "../../include/Serializers/JSONSerializerRegistry.h"
#include "../../include/Brakeza.h"
#include "../../include/3D/Image3DAnimation.h"
#include "../../include/Serializers/Object3DSerializer.h"

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
    auto *o = new Image3DAnimation(
        ComponentsManager::get()->getComponentCamera()->getCamera()->getPosition(), 1, 1
    );

    ApplyJsonToObject(json, o);

    return o;
}

void Image3DAnimationSerializer::ApplyJsonToObject(const cJSON *json, Object3D *o)
{
    Logging::Message("[Image3DAnimationSerializer] ApplyJsonToObject: %d", (int) o->getTypeObject());

    auto image = dynamic_cast<Image3DAnimation*>(o);

    Object3DSerializer().ApplyJsonToObject(json, o);

    if (cJSON_GetObjectItemCaseSensitive(json, "animations") != nullptr) {
        cJSON *currentAnimation;
        cJSON_ArrayForEach(currentAnimation, cJSON_GetObjectItemCaseSensitive(json, "animations")) {
            auto spriteFile = cJSON_GetObjectItemCaseSensitive(currentAnimation, "sprite")->valuestring;
            auto width = cJSON_GetObjectItemCaseSensitive(currentAnimation, "width")->valueint;
            auto height = cJSON_GetObjectItemCaseSensitive(currentAnimation, "height")->valueint;
            auto fps = cJSON_GetObjectItemCaseSensitive(currentAnimation, "fps")->valueint;
            auto numberFrames = cJSON_GetObjectItemCaseSensitive(currentAnimation, "numberFrames")->valueint;

            image->addAnimation(spriteFile, width, height, numberFrames, fps);
        }

        image->width = static_cast<float>(cJSON_GetObjectItemCaseSensitive(json, "width")->valuedouble);
        image->height = static_cast<float>(cJSON_GetObjectItemCaseSensitive(json, "height")->valuedouble);

        image->updateBillboardSize();

        image->setAnimation(cJSON_GetObjectItemCaseSensitive(json, "currentIndexAnimation")->valueint);
        image->getCurrentTextureAnimation()->updateStep();
    }
}

void Image3DAnimationSerializer::LoadFileIntoScene(const std::string &file)
{
    auto *o = new Image3DAnimation(
        ComponentsManager::get()->getComponentCamera()->getCamera()->getPosition(), 1, 1
    );

    o->addAnimation(file,1,1,1,1);
    o->setAnimation(0);
    o->setPosition(ComponentsManager::get()->getComponentCamera()->getCamera()->forward().getScaled(2));

    Brakeza::get()->addObject3D(o, Brakeza::UniqueObjectLabel("BillboardAnimation"));
}
