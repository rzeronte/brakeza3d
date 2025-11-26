//
// Created by Eduardo on 23/11/2025.
//

#include "../../include/Serializers/Mesh3DSerializer.h"
#include "../../include/Components/ComponentsManager.h"
#include "../../include/OpenGL/ShaderOGLCustomMesh3D.h"
#include "../../include/Brakeza.h"
#include "../../include/GUI/Objects/Object3DGUI.h"
#include "../../include/Serializers/JSONSerializerRegistry.h"
#include "../../include/Serializers/Object3DSerializer.h"

cJSON* Mesh3DSerializer::JsonByObject(Object3D *o)
{
    Logging::Message("[Mesh3DSerializer] JsonByObject: %s", o->getTypeObject());

    auto *mesh = dynamic_cast<Mesh3D*>(o);

    auto root = Object3DSerializer().JsonByObject(o);

    cJSON_AddStringToObject(root, "model", mesh->sourceFile.c_str());
    cJSON_AddBoolToObject(root, "enableLights", mesh->isEnableLights());

    // Shaders
    cJSON *effectsArrayJSON = cJSON_CreateArray();
    for (auto s : mesh->customShaders) {
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

void Mesh3DSerializer::ApplyJsonToObject(const cJSON *json, Object3D *o)
{
    Logging::Message("[Mesh3DSerializer] ApplyJsonToObject", o->getTypeObject());

    auto mesh = dynamic_cast<Mesh3D*>(o);

    Object3DSerializer().ApplyJsonToObject(json, o);

    mesh->setEnableLights(cJSON_GetObjectItemCaseSensitive(json, "enableLights")->valueint);
    mesh->AssimpLoadGeometryFromFile(cJSON_GetObjectItemCaseSensitive(json, "model")->valuestring);

    if (cJSON_GetObjectItemCaseSensitive(json, "shaders") != nullptr) {
        auto shaderTypesMapping = ComponentsManager::get()->getComponentRender()->getShaderTypesMapping();
        cJSON *currentShaderJSON;
        cJSON_ArrayForEach(currentShaderJSON, cJSON_GetObjectItemCaseSensitive(json, "shaders")) {
            auto typeString = cJSON_GetObjectItemCaseSensitive(currentShaderJSON, "type")->valuestring;
            switch (auto type = ShaderOGLCustom::getShaderTypeFromString(typeString)) {
                case SHADER_OBJECT: {
                    auto name = cJSON_GetObjectItemCaseSensitive(currentShaderJSON, "name")->valuestring;
                    auto vertex = cJSON_GetObjectItemCaseSensitive(currentShaderJSON, "vertexshader")->valuestring;
                    auto fragment = cJSON_GetObjectItemCaseSensitive(currentShaderJSON, "fragmentshader")->valuestring;
                    auto types = cJSON_GetObjectItemCaseSensitive(currentShaderJSON, "types");
                    mesh->addCustomShader(new ShaderOGLCustomMesh3D(mesh, name, vertex, fragment, types));
                    break;
                }
                default: {
                    Logging::Message("[LoadAttributes] Unknown shader type: %d", type);
                };
            }
        }
    }

    if (cJSON_GetObjectItemCaseSensitive(json, "isCollisionsEnabled") != nullptr) {
        bool collisionsEnabled = cJSON_GetObjectItemCaseSensitive(json, "isCollisionsEnabled")->valueint;
        cJSON *colliderJSON = cJSON_GetObjectItemCaseSensitive(json, "collider");

        if (collisionsEnabled) {
            mesh->setCollisionsEnabled(true);
            int mode = cJSON_GetObjectItemCaseSensitive(colliderJSON, "mode")->valueint;
            int shape = cJSON_GetObjectItemCaseSensitive(colliderJSON, "shape")->valueint;

            if ((cJSON_GetObjectItemCaseSensitive(colliderJSON, "colliderStatic") != nullptr)) {
                mesh->setColliderStatic(cJSON_GetObjectItemCaseSensitive(colliderJSON, "colliderStatic")->valueint);
            }

            switch (mode) {
                case GHOST:
                    if (shape == SIMPLE_SHAPE) {
                        mesh->setupGhostCollider(SIMPLE_SHAPE);
                    }
                    if (shape == CAPSULE) {
                        mesh->setupGhostCollider(CAPSULE);
                    }

                    if (shape == TRIANGLE_MESH_SHAPE) {
                        mesh->setupGhostCollider(TRIANGLE_MESH_SHAPE);
                    }
                    break;
                case BODY:
                    if (shape == SIMPLE_SHAPE) {
                        mesh->setupRigidBodyCollider(SIMPLE_SHAPE);
                    }
                    if (shape == CAPSULE) {
                        mesh->setupRigidBodyCollider(CAPSULE);
                    }
                    if (shape == TRIANGLE_MESH_SHAPE) {
                        mesh->setupRigidBodyCollider(TRIANGLE_MESH_SHAPE);
                    }
                    break;
                default: {
                    Logging::Message("[LoadAttributes] Unknown collision mode: %d", mode);
                }
            }
        }
    }
}

Object3D* Mesh3DSerializer::ObjectByJson(cJSON *json)
{
    Logging::Message("[Mesh3DSerializer] ObjectByJson");

    auto o = new Mesh3D();
    ApplyJsonToObject(json, o);
    return o;
}

void Mesh3DSerializer::LoadFileIntoScene(const std::string& model)
{
    auto *o = new Mesh3D();

    o->setPosition(ComponentsManager::get()->getComponentCamera()->getCamera()->getPosition());
    o->AssimpLoadGeometryFromFile(model);

    Brakeza::get()->addObject3D(o, Brakeza::uniqueObjectLabel("Mesh3D"));
}
