//
// Created by Eduardo on 24/11/2025.
//

#include "../include/Serializers/Mesh3DAnimationSerializer.h"
#include "../../include/Brakeza.h"
#include "../../include/Serializers/JSONSerializerRegistry.h"

cJSON* Mesh3DAnimationSerializer::JsonByObject(Object3D *o)
{
    Logging::Message("[Mesh3DSerializer json] Alive here...");
    std::cout << "[Mesh3DSerializer json] " << o->getTypeObject() << std::endl;

    auto *mesh = dynamic_cast<Mesh3DAnimation*>(o);

    cJSON *root = JSONSerializerRegistry::GetJsonByObject(o);

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

            cJSON *position = cJSON_CreateObject();
            cJSON_AddNumberToObject(position, "x", boneInfoCollider.position.x);
            cJSON_AddNumberToObject(position, "y", boneInfoCollider.position.y);
            cJSON_AddNumberToObject(position, "z", boneInfoCollider.position.z);
            cJSON_AddItemToObject(boneInfoColliderJSON, "position", position);

            cJSON *size = cJSON_CreateObject();
            cJSON_AddNumberToObject(size, "x", boneInfoCollider.size.x);
            cJSON_AddNumberToObject(size, "y", boneInfoCollider.size.y);
            cJSON_AddNumberToObject(size, "z", boneInfoCollider.size.z);
            cJSON_AddItemToObject(boneInfoColliderJSON, "size", size);

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
    std::cout << "[Mesh3DSerializer ApplyJsonToObject] " << o->getTypeObject() << std::endl;
    auto mesh = dynamic_cast<Mesh3DAnimation*>(o);
    Mesh3DSerializer::ApplyJsonToObject(json, o);

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
    std::cout << "[Mesh3DSerializer LoadJSONObject]"  << std::endl;

    auto o = new Mesh3DAnimation();

    ApplyJsonToObject(json, o);

    return o;
}

void Mesh3DAnimationSerializer::LoadFileIntoScene(const std::string& model)
{
    auto *o = new Mesh3DAnimation();
    o->setPosition(ComponentsManager::get()->getComponentCamera()->getCamera()->getPosition());
    o->AssimpLoadGeometryFromFile(model);

    Brakeza::get()->addObject3D(o, Brakeza::uniqueObjectLabel("Mesh3D"));
}
