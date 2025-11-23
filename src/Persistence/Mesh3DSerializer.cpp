//
// Created by Eduardo on 23/11/2025.
//

#include "../../include/Persistence/Mesh3DSerializer.h"
#include "../../include/ComponentsManager.h"
#include "../../include/OpenGL/ShaderOpenGLCustomMesh3D.h"
#include "../../include/Brakeza3D.h"
#include "../../include/Persistence/JSONSerializerRegistry.h"

cJSON* Mesh3DSerializer::JsonByObject(Object3D *object)
{
    Logging::Message("[Mesh3DSerializer json] Alive here...");

    auto *mesh = dynamic_cast<Mesh3D*>(object);

    cJSON *root = JSONSerializerRegistry::GetJsonByObject(object);

    cJSON *effectsArrayJSON = cJSON_CreateArray();
    for (auto s : mesh->customShaders) {
        cJSON_AddItemToArray(effectsArrayJSON, s->getTypesJSON());
    }
    cJSON_AddItemToObject(root, "shaders", effectsArrayJSON);

    cJSON_AddStringToObject(root, "model", mesh->sourceFile.c_str());
    cJSON_AddBoolToObject(root, "enableLights", mesh->isEnableLights());

    if (mesh->grid != nullptr) {
        cJSON_AddItemToObject(root, "grid", mesh->grid->getJSON());
    }

    if (mesh->octree != nullptr) {
        cJSON_AddItemToObject(root, "octree", mesh->octree->getJSON());
    }

    return root;
}

void Mesh3DSerializer::ApplyJsonToObject(cJSON *object, Object3D *o)
{
    std::cout << "[Mesh3DSerializer setJSONPropertiesIntoObject3D] Alive " << std::endl;
    Mesh3D *mesh = dynamic_cast<Mesh3D*>(o);

    o->setBelongToScene(true);
    o->setEnableLights(cJSON_GetObjectItemCaseSensitive(object, "enableLights")->valueint);
    mesh->AssimpLoadGeometryFromFile(cJSON_GetObjectItemCaseSensitive(object, "model")->valuestring);

    if (cJSON_GetObjectItemCaseSensitive(object, "shaders") != nullptr) {
        auto shaderTypesMapping = ComponentsManager::get()->getComponentRender()->getShaderTypesMapping();
        cJSON *currentShaderJSON;
        cJSON_ArrayForEach(currentShaderJSON, cJSON_GetObjectItemCaseSensitive(object, "shaders")) {
            auto typeString = cJSON_GetObjectItemCaseSensitive(currentShaderJSON, "type")->valuestring;
            switch (auto type = ShaderOpenGLCustom::getShaderTypeFromString(typeString)) {
                case SHADER_OBJECT: {
                    auto name = cJSON_GetObjectItemCaseSensitive(currentShaderJSON, "name")->valuestring;
                    auto vertex = cJSON_GetObjectItemCaseSensitive(currentShaderJSON, "vertexshader")->valuestring;
                    auto fragment = cJSON_GetObjectItemCaseSensitive(currentShaderJSON, "fragmentshader")->valuestring;
                    auto types = cJSON_GetObjectItemCaseSensitive(currentShaderJSON, "types");
                    mesh->addCustomShader(new ShaderOpenGLCustomMesh3D(mesh, name, vertex, fragment, types));
                    break;
                }
                default: {
                    Logging::Message("[LoadAttributes] Unknown shader type: %d", type);
                };
            }
        }
    }

    if (cJSON_GetObjectItemCaseSensitive(object, "isCollisionsEnabled") != nullptr) {
        bool collisionsEnabled = cJSON_GetObjectItemCaseSensitive(object, "isCollisionsEnabled")->valueint;
        cJSON *colliderJSON = cJSON_GetObjectItemCaseSensitive(object, "collider");

        if (collisionsEnabled) {
            o->setCollisionsEnabled(true);
            int mode = (int) cJSON_GetObjectItemCaseSensitive(colliderJSON, "mode")->valueint;
            int shape = (int) cJSON_GetObjectItemCaseSensitive(colliderJSON, "shape")->valueint;

            if ((cJSON_GetObjectItemCaseSensitive(colliderJSON, "colliderStatic") != nullptr)) {
                o->setColliderStatic(cJSON_GetObjectItemCaseSensitive(colliderJSON, "colliderStatic")->valueint);
            }

            switch (mode) {
                case GHOST:
                    if (shape == SIMPLE_SHAPE) {
                        o->setupGhostCollider(SIMPLE_SHAPE);
                    }
                    if (shape == CAPSULE) {
                        o->setupGhostCollider(CAPSULE);
                    }

                    if (shape == TRIANGLE_MESH_SHAPE) {
                        o->setupGhostCollider(TRIANGLE_MESH_SHAPE);
                    }
                    break;
                case BODY:
                    if (shape == SIMPLE_SHAPE) {
                        o->setupRigidBodyCollider(SIMPLE_SHAPE);
                    }
                    if (shape == CAPSULE) {
                        o->setupRigidBodyCollider(CAPSULE);
                    }
                    if (shape == TRIANGLE_MESH_SHAPE) {
                        o->setupRigidBodyCollider(TRIANGLE_MESH_SHAPE);
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
    std::cout << "[Mesh3DSerializer LoadJSONObject] alive" << std::endl;
    auto o = new Mesh3D();
    ApplyJsonToObject(json, o);
    return o;
}

void Mesh3DSerializer::LoadFileIntoScene(const std::string& model)
{
    if (!Tools::fileExists(model.c_str())) {
        Logging::Message("[SceneLoader] File model '%s' not found!!", model.c_str());
        return;
    }

    auto *o = new Mesh3D();
    o->setBelongToScene(true);
    o->setPosition(ComponentsManager::get()->getComponentCamera()->getCamera()->getPosition());
    o->AssimpLoadGeometryFromFile(model);

    Brakeza3D::get()->addObject3D(o, Brakeza3D::uniqueObjectLabel("Mesh3D"));
}
