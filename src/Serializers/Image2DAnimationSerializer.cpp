//
// Created by Eduardo on 24/11/2025.
//

#include "../../include/Serializers/Image2DAnimationSerializer.h"
#include "../../include/Serializers/JSONSerializerRegistry.h"
#include "../../include/Brakeza.h"
#include "../../include/2D/Image2DAnimation.h"

cJSON * Image2DAnimationSerializer::JsonByObject(Object3D *o)
{
    Logging::Message("[Image2DAnimationSerializer] JsonByObject: %s", o->getTypeObject());

    auto image = dynamic_cast<Image2DAnimation*>(o);

    auto root = JSONSerializerRegistry::GetJsonByObject(o);

    cJSON_AddNumberToObject(root, "x", image->getX());
    cJSON_AddNumberToObject(root, "y", image->getY());

    cJSON *animationJSON = cJSON_CreateObject();
    cJSON_AddStringToObject(animationJSON, "sprite", image->getAnimation()->getBaseFilename().c_str());
    cJSON_AddNumberToObject(animationJSON, "width", (int) image->getAnimation()->currentSpriteWidth);
    cJSON_AddNumberToObject(animationJSON, "height", (int) image->getAnimation()->currentspriteHeight);
    cJSON_AddNumberToObject(animationJSON, "numberFrames", (int) image->getAnimation()->numberFramesToLoad);
    cJSON_AddNumberToObject(animationJSON, "fps", (int) image->getAnimation()->fps);

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

    return o;
}

void Image2DAnimationSerializer::ApplyJsonToObject(const cJSON *json, Object3D *o)
{
    Logging::Message("[Image2DAnimationSerializer] ApplyJsonToObject: %s", o->getTypeObject());

    auto image = dynamic_cast<Image2DAnimation*>(o);

    Object3DSerializer().ApplyJsonToObject(json, o);
}

void Image2DAnimationSerializer::LoadFileIntoScene(const std::string &file)
{
    auto *o = new Image2DAnimation(
        BrakezaSetup::get()->screenWidth/2,
        BrakezaSetup::get()->screenHeight/2,
        false,
        new TextureAnimated(file,1,1,1,1)
    );

    o->setPosition(ComponentsManager::get()->getComponentCamera()->getCamera()->forward().getScaled(2));

    Brakeza::get()->addObject3D(o, Brakeza::uniqueObjectLabel("Image2DAnimation"));
}
