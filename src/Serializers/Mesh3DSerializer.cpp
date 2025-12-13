//
// Created by Eduardo on 23/11/2025.
//

#include "../../include/Serializers/Mesh3DSerializer.h"
#include "../../include/Components/Components.h"
#include "../../include/OpenGL/ShaderOGLCustomMesh3D.h"
#include "../../include/Brakeza.h"
#include "../../include/Pools/JobLoadMesh3D.h"
#include "../../include/Serializers/JSONSerializerRegistry.h"
#include "../../include/Serializers/Object3DSerializer.h"

cJSON* Mesh3DSerializer::JsonByObject(Object3D *o)
{
    Logging::Message("[Mesh3DSerializer] JsonByObject: %d", (int) o->getTypeObject());

    auto *mesh = dynamic_cast<Mesh3D*>(o);

    auto root = Object3DSerializer().JsonByObject(o);

    cJSON_AddStringToObject(root, "model", mesh->sourceFile.c_str());
    cJSON_AddBoolToObject(root, "enableLights", mesh->isEnableLights());

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
    Logging::Message("[Mesh3DSerializer] ApplyJsonToObject %d", o->getTypeObject());

    auto mesh = dynamic_cast<Mesh3D*>(o);

    Object3DSerializer().ApplyJsonToObject(json, o);
    mesh->setEnableLights(cJSON_GetObjectItemCaseSensitive(json, "enableLights")->valueint);
}

Object3D* Mesh3DSerializer::ObjectByJson(cJSON *json)
{
    Logging::Message("[Mesh3DSerializer] ObjectByJson");

    auto o = new Mesh3D();
    ApplyJsonToObject(json, o);

    Brakeza::get()->getPoolManager().Pool().enqueueWithMainThreadCallback(std::make_shared<JobLoadMesh3D>(o, json));

    return o;
}

void Mesh3DSerializer::LoadFileIntoScene(const std::string& model)
{
    auto *o = new Mesh3D();

    o->setPosition(Components::get()->Camera()->getCamera()->getPosition());
    o->AssimpLoadGeometryFromFile(model);

    Brakeza::get()->addObject3D(o, Brakeza::UniqueObjectLabel("Mesh3D"));
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
                    if (shape == CAPSULE) {
                        m->SetupGhostCollider(CAPSULE);
                    }

                    if (shape == TRIANGLE_MESH_SHAPE) {
                        m->SetupGhostCollider(TRIANGLE_MESH_SHAPE);
                    }
                    break;
                case BODY:
                    if (shape == SIMPLE_SHAPE) {
                        m->SetupRigidBodyCollider(SIMPLE_SHAPE);
                    }
                    if (shape == CAPSULE) {
                        m->SetupRigidBodyCollider(CAPSULE);
                    }
                    if (shape == TRIANGLE_MESH_SHAPE) {
                        m->SetupRigidBodyCollider(TRIANGLE_MESH_SHAPE);
                    }
                    break;
                default: {
                    Logging::Message("[LoadAttributes] Unknown collision mode: %d", mode);
                }
            }
        }
    }
}

void Mesh3DSerializer::ApplyShadersCreation(Mesh3D *mesh, cJSON* json)
{
    if (cJSON_GetObjectItemCaseSensitive(json, "shaders") != nullptr) {
        auto shaderTypesMapping = Components::get()->Render()->getShaderTypesMapping();
        cJSON *currentShaderJSON;
        cJSON_ArrayForEach(currentShaderJSON, cJSON_GetObjectItemCaseSensitive(json, "shaders")) {
            auto typeString = cJSON_GetObjectItemCaseSensitive(currentShaderJSON, "type")->valuestring;
            switch (auto type = ShaderOGLCustom::getShaderTypeFromString(typeString)) {
                case SHADER_OBJECT: {
                    auto name = cJSON_GetObjectItemCaseSensitive(currentShaderJSON, "name")->valuestring;
                    auto vs = cJSON_GetObjectItemCaseSensitive(currentShaderJSON, "vertexshader")->valuestring;
                    auto fs = cJSON_GetObjectItemCaseSensitive(currentShaderJSON, "fragmentshader")->valuestring;
                    auto types = cJSON_GetObjectItemCaseSensitive(currentShaderJSON, "types");
                    auto shader = new ShaderOGLCustomMesh3D(mesh, name, vs, fs, types);
                    mesh->AddCustomShader(shader);
                    break;
                }
                default: {
                    Logging::Message("[LoadAttributes] Unknown shader type: %d", type);
                };
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
