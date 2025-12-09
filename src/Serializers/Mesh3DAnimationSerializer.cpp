//
// Created by Eduardo on 24/11/2025.
//

#include "../include/Serializers/Mesh3DAnimationSerializer.h"
#include "../../include/Brakeza.h"
#include "../../include/3D/Mesh3D.h"
#include "../../include/3D/Mesh3DAnimation.h"
#include "../../include/Components/Components.h"
#include "../../include/Misc/ToolsJSON.h"
#include "../../include/Serializers/JSONSerializerRegistry.h"
#include "../../include/Serializers/Mesh3DSerializer.h"

cJSON* Mesh3DAnimationSerializer::JsonByObject(Object3D *o)
{
    Logging::Message("[Mesh3DAnimationSerializer] JsonByObject: %d", (int) o->getTypeObject());

    auto *mesh = dynamic_cast<Mesh3DAnimation*>(o);

    auto root = Mesh3DSerializer().JsonByObject(o);

    cJSON_AddNumberToObject(root, "animationSpeed", mesh->animation_speed);
    cJSON_AddBoolToObject(root, "boneColliderEnabled", mesh->boneColliderEnabled);
    cJSON_AddNumberToObject(root, "indexCurrentAnimation", mesh->indexCurrentAnimation);

    cJSON *bonesCollidersJSON = cJSON_CreateArray();
    for (const auto& b : mesh->boneMappingColliders) {
        cJSON *bonesColliderJSON = cJSON_CreateObject();

        cJSON_AddStringToObject(bonesColliderJSON, "nameMapping", b.nameMapping.c_str());

        cJSON *bonesInfoJSON = cJSON_CreateArray();
        for (const auto& boneInfoCollider : b.boneColliderInfo) {
            if (!boneInfoCollider.enabled) continue;

            cJSON *boneInfoColliderJSON = cJSON_CreateObject();

            cJSON_AddStringToObject(boneInfoColliderJSON, "name", boneInfoCollider.name.c_str());
            cJSON_AddNumberToObject(boneInfoColliderJSON, "boneId", boneInfoCollider.boneId);
            cJSON_AddBoolToObject(boneInfoColliderJSON, "enabled", boneInfoCollider.enabled);
            cJSON_AddNumberToObject(boneInfoColliderJSON, "shape", boneInfoCollider.shape);


            cJSON_AddItemToObject(boneInfoColliderJSON, "position", ToolsJSON::Vertex3DToJSON(boneInfoCollider.position));
            cJSON_AddItemToObject(boneInfoColliderJSON, "size", ToolsJSON::Vertex3DToJSON(boneInfoCollider.size));

            cJSON_AddItemToArray(bonesInfoJSON, boneInfoColliderJSON);
        }
        cJSON_AddItemToObject(bonesColliderJSON, "bonesInfoColliders", bonesInfoJSON);
        cJSON_AddItemToArray(bonesCollidersJSON, bonesColliderJSON);
    }
    cJSON_AddItemToObject(root, "bonesColliders", bonesCollidersJSON);

    return root;
}

void Mesh3DAnimationSerializer::ApplyJsonToObject(const cJSON *json, Object3D *o)
{
    Logging::Message("[Mesh3DAnimationSerializer] ApplyJsonToObject %d", (int) o->getTypeObject());

    auto mesh = dynamic_cast<Mesh3DAnimation*>(o);

    Mesh3DSerializer().ApplyJsonToObject(json, o);

    // speed
    auto speed = cJSON_GetObjectItemCaseSensitive(json, "animationSpeed")->valuedouble;
    mesh->setAnimationSpeed(static_cast<float>(speed));
    mesh->setIndexCurrentAnimation(cJSON_GetObjectItemCaseSensitive(json, "indexCurrentAnimation")->valueint);

    cJSON *currentMapCollider;
    cJSON_ArrayForEach(currentMapCollider, cJSON_GetObjectItemCaseSensitive(json, "bonesColliders")) {
        std::string nameMapping = cJSON_GetObjectItemCaseSensitive(currentMapCollider, "nameMapping")->valuestring;
        mesh->createBonesMappingColliders(nameMapping);

        cJSON *currentBoneInfoCollider;
        cJSON_ArrayForEach(currentBoneInfoCollider, cJSON_GetObjectItemCaseSensitive(currentMapCollider, "bonesInfoColliders")) {
            std::string boneName = cJSON_GetObjectItemCaseSensitive(currentBoneInfoCollider, "name")->valuestring;
            unsigned int boneId = cJSON_GetObjectItemCaseSensitive(currentBoneInfoCollider, "boneId")->valueint;
            unsigned int shape = cJSON_GetObjectItemCaseSensitive(currentBoneInfoCollider, "shape")->valueint;
            bool enabled = static_cast<bool>(cJSON_GetObjectItemCaseSensitive(currentBoneInfoCollider, "enabled")->valueint);

            //auto position = ToolsJSON::parseVertex3DJSON(cJSON_GetObjectItemCaseSensitive(currentBoneInfoCollider, "position"));
            //auto size = ToolsJSON::parseVertex3DJSON(cJSON_GetObjectItemCaseSensitive(currentBoneInfoCollider, "size"));

            mesh->SetMappingBoneColliderInfo(nameMapping, boneId, enabled, BoneCollisionShape::BONE_CAPSULE);
        }
    }
}

Object3D* Mesh3DAnimationSerializer::ObjectByJson(cJSON *json)
{
    Logging::Message("[Mesh3DAnimationSerializer] ObjectByJson");

    auto o = new Mesh3DAnimation();
    ApplyJsonToObject(json, o);
    o->AssimpLoadAnimation(cJSON_GetObjectItemCaseSensitive(json, "model")->valuestring);

    return o;
}

void Mesh3DAnimationSerializer::LoadFileIntoScene(const std::string& file)
{
    auto *o = new Mesh3DAnimation();
    o->setPosition(Components::get()->Camera()->getCamera()->getPosition());
    o->AssimpLoadAnimation(file);

    Brakeza::get()->addObject3D(o, Brakeza::UniqueObjectLabel("Mesh3D"));
}
