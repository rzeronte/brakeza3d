//
// Created by Eduardo on 23/11/2025.
//

#include "../../include/Serializers/Mesh3DSerializer.h"
#include "../../include/Components/Components.h"
#include "../../include/OpenGL/Code/ShaderOGLCustomCodeMesh3D.h"
#include "../../include/OpenGL/Code/ShaderBaseCustomOGLCode.h"
#include "../../include/OpenGL/Nodes/ShaderNodesMesh3D.h"
#include "../../include/GUI/ShaderNodeEditorManager.h"
#include "../../include/Brakeza.h"
#include "../../include/GUI/Objects/ShadersGUI.h"
#include "../../include/Threads/ThreadJobLoadMesh3D.h"
#include "../../include/Render/JSONSerializerRegistry.h"
#include "../../include/Serializers/Object3DSerializer.h"

cJSON* Mesh3DSerializer::JsonByObject(Object3D *o)
{
    LOG_MESSAGE("[Mesh3DSerializer] JsonByObject: %d",  o->getTypeObject());

    auto *mesh = dynamic_cast<Mesh3D*>(o);

    auto root = Object3DSerializer().JsonByObject(o);

    cJSON_AddStringToObject(root, "model", mesh->sourceFile.c_str());
    cJSON_AddBoolToObject(root, "enableLights", mesh->isEnableLights());
    cJSON_AddBoolToObject(root, "renderDefaultPipeline", mesh->isRenderPipelineDefault());
    cJSON_AddBoolToObject(root, "frustumCullSubmeshes", mesh->isFrustumCullSubmeshes());

    // Shaders
    cJSON *effectsArrayJSON = cJSON_CreateArray();
    for (auto &s : mesh->customShaders) {
        cJSON_AddItemToArray(effectsArrayJSON, s->getTypesJSON());
    }
    cJSON_AddItemToObject(root, "shaders", effectsArrayJSON);

    // grid
    if (mesh->grid != nullptr) {
        cJSON_AddItemToObject(root, "grid", mesh->grid->getJSON());
    }

    // octree
    if (mesh->octree != nullptr) {
        cJSON_AddItemToObject(root, "octree", mesh->octree->getJSON());
    }

    return root;
}

void Mesh3DSerializer::ApplyJsonToObject(cJSON *json, Object3D *o)
{
    std::lock_guard<std::mutex> lock(mtx);
    LOG_MESSAGE("[Mesh3DSerializer] ApplyJsonToObject %d", o->getTypeObject());

    auto mesh = dynamic_cast<Mesh3D*>(o);

    Object3DSerializer().ApplyJsonToObject(json, o);
    mesh->setEnableLights(cJSON_GetObjectItemCaseSensitive(json, "enableLights")->valueint);
    auto *renderDefaultItem = cJSON_GetObjectItemCaseSensitive(json, "renderDefaultPipeline");
    if (renderDefaultItem) mesh->setRenderPipelineDefault(renderDefaultItem->valueint);
    auto *frustumCullItem = cJSON_GetObjectItemCaseSensitive(json, "frustumCullSubmeshes");
    if (frustumCullItem) mesh->setFrustumCullSubmeshes(frustumCullItem->valueint);
}

Object3D* Mesh3DSerializer::ObjectByJson(cJSON *json)
{
    LOG_MESSAGE("[Mesh3DSerializer] ObjectByJson");

    auto o = new Mesh3D();
    ApplyJsonToObject(json, o);

    Brakeza::get()->PoolCompute().enqueueWithMainThreadCallback(std::make_shared<ThreadJobLoadMesh3D>(o, json));

    return o;
}

void Mesh3DSerializer::MenuLoad(const std::string& model)
{
    auto *o = new Mesh3D(model);
    o->setName(Brakeza::UniqueObjectLabel("Mesh3D"));
    o->setPosition(Components::get()->Camera()->getCamera()->getPosition());

    auto json = Mesh3DSerializer::JsonByObject(o);
    Brakeza::get()->PoolCompute().enqueueWithMainThreadCallback(std::make_shared<ThreadJobLoadMesh3D>(o, json));
}

void Mesh3DSerializer::ApplyGeometryFromFile(Mesh3D *m, cJSON* json)
{
    m->AssimpLoadGeometryFromFile(ExtractFileModelPath(json));
}

void Mesh3DSerializer::ApplyCollider(Mesh3D *m, cJSON* json)
{
    if (cJSON_GetObjectItemCaseSensitive(json, "isCollisionsEnabled") != nullptr) {
        bool collisionsEnabled = cJSON_GetObjectItemCaseSensitive(json, "isCollisionsEnabled")->valueint;
        cJSON *colliderJSON = cJSON_GetObjectItemCaseSensitive(json, "collider");

        if (collisionsEnabled) {
            m->setCollisionsEnabled(true);
            int mode = cJSON_GetObjectItemCaseSensitive(colliderJSON, "mode")->valueint;
            int shape = cJSON_GetObjectItemCaseSensitive(colliderJSON, "shape")->valueint;

            if (cJSON_GetObjectItemCaseSensitive(colliderJSON, "colliderStatic") != nullptr) {
                m->setColliderStatic(cJSON_GetObjectItemCaseSensitive(colliderJSON, "colliderStatic")->valueint);
            }

            switch (mode) {
                case GHOST:
                    if (shape == SIMPLE_SHAPE) {
                        m->SetupGhostCollider(SIMPLE_SHAPE);
                    }
                    if (shape == CAPSULE_SHAPE) {
                        m->SetupGhostCollider(CAPSULE_SHAPE);
                    }

                    if (shape == TRIANGLE_MESH_SHAPE) {
                        m->SetupGhostCollider(TRIANGLE_MESH_SHAPE);
                    }
                    break;
                case BODY:
                    if (shape == SIMPLE_SHAPE) {
                        m->SetupRigidBodyCollider(SIMPLE_SHAPE);
                    }
                    if (shape == CAPSULE_SHAPE) {
                        m->SetupRigidBodyCollider(CAPSULE_SHAPE);
                    }
                    if (shape == TRIANGLE_MESH_SHAPE) {
                        m->SetupRigidBodyCollider(TRIANGLE_MESH_SHAPE);
                    }
                    break;
                case KINEMATIC:
                    m->setupKinematicCollider();
                    break;
                default: {
                    LOG_ERROR("[Mesh3D] Unknown collision mode: %d", mode);
                }
            }
        }
    }
}

void Mesh3DSerializer::ApplyShadersFileRead(Mesh3D *mesh, cJSON* json)
{
    if (cJSON_GetObjectItemCaseSensitive(json, "shaders") != nullptr) {
        cJSON *currentShaderJSON;
        cJSON_ArrayForEach(currentShaderJSON, cJSON_GetObjectItemCaseSensitive(json, "shaders")) {
            auto typesFile = cJSON_GetObjectItemCaseSensitive(currentShaderJSON, "typesFile")->valuestring;
            auto metaInfo = ShadersGUI::ExtractShaderCustomCodeMetainfo(typesFile);

            ShaderBaseCustom* shader = nullptr;

            if (ShaderBaseCustom::getShaderTypeFromString(metaInfo.type) == SHADER_OBJECT) {
                auto s = new ShaderOGLCustomCodeMesh3D(mesh, metaInfo.name, metaInfo.typesFile, metaInfo.vsFile, metaInfo.fsFile);
                s->PrepareBackground();
                shader = s;
            } else if (ShaderBaseCustom::getShaderTypeFromString(metaInfo.type) == SHADER_NODE_OBJECT) {
                auto manager = new ShaderNodeEditorManager(SHADER_NODE_OBJECT);
                manager->LoadFromFile(metaInfo.typesFile.c_str());
                shader = new ShaderNodesMesh3D(metaInfo.name, metaInfo.typesFile, SHADER_NODE_OBJECT, manager, mesh);
            } else {
                LOG_ERROR("[Mesh3D] Unsupported shader type for Mesh3D: %s", metaInfo.type.c_str());
                continue;
            }

            if (shader != nullptr) {
                auto typesArray = cJSON_GetObjectItemCaseSensitive(currentShaderJSON, "types");
                if (typesArray) {
                    if (auto* codeShader = dynamic_cast<ShaderBaseCustomOGLCode*>(shader)) {
                        codeShader->overrideDataTypesFromJSON(typesArray);
                    }
                }
                mesh->AddCustomShader(shader);
            } else {
                LOG_ERROR("[Mesh3D] Cannot load shader %s...", typesFile);
            }
        }
    }
}

void Mesh3DSerializer::ApplyShadersBackground(Mesh3D *m)
{
    for (auto &s : m->getCustomShaders()) {
        s->PrepareBackground();
    }
}

void Mesh3DSerializer::ApplyCustomShadersMainThread(Mesh3D *m)
{
    for (auto &s : m->getCustomShaders()) {
        s->PrepareMainThread();
    }
}

const char* Mesh3DSerializer::ExtractFileModelPath(cJSON *json)
{
    return cJSON_GetObjectItemCaseSensitive(json, "model")->valuestring;
}
