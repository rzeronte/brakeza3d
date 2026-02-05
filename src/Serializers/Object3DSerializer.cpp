//
// Created by Eduardo on 23/11/2025.
//

#ifndef BRAKEZA3D_OBJECT3DSERIALIZER_CPP_H
#define BRAKEZA3D_OBJECT3DSERIALIZER_CPP_H

#include "../../include/Serializers/Object3DSerializer.h"
#include "../../include/Render/JSONSerializerRegistry.h"
#include "../../include/Misc/ToolsJSON.h"
#include "../../include/Brakeza.h"
#include "../../include/Threads/ThreadJobLoadObject.h"

cJSON * Object3DSerializer::JsonByObject(Object3D* o)
{
    LOG_MESSAGE("[Object3DSerializer] JsonByObject: %d", o->getTypeObject());

    cJSON *root = cJSON_CreateObject();

    cJSON_AddStringToObject(root, "name", o->getName().c_str());
    cJSON_AddNumberToObject(root, "scale", o->getScale());
    cJSON_AddNumberToObject(root, "alpha", o->getAlpha());
    cJSON_AddBoolToObject(root, "enabled", o->isEnabled());
    cJSON_AddItemToObject(root, "position", ToolsJSON::Vertex3DToJSON(o->getPosition()));
    cJSON_AddItemToObject(root, "rotation", ToolsJSON::Vertex3DToJSON(o->getRotation().getVertex3DAngles()));

    cJSON *renderSettings = cJSON_CreateObject();
    cJSON_AddBoolToObject(renderSettings, "blend", o->getRenderSettings().blend);
    cJSON_AddBoolToObject(renderSettings, "culling", o->getRenderSettings().culling);
    cJSON_AddBoolToObject(renderSettings, "depthTest", o->getRenderSettings().depthTest);
    cJSON_AddBoolToObject(renderSettings, "writeDepth", o->getRenderSettings().writeDepth);
    cJSON_AddBoolToObject(renderSettings, "shadowMap", o->getRenderSettings().shadowMap);
    cJSON_AddNumberToObject(renderSettings, "blend_function_src", o->getRenderSettings().mode_dst);
    cJSON_AddNumberToObject(renderSettings, "blend_function_dst", o->getRenderSettings().mode_dst);
    cJSON_AddItemToObject(root, "renderSettings", renderSettings);

    cJSON_AddBoolToObject(root, "isCollisionsEnabled", o->isCollisionsEnabled());
    if (o->isCollisionsEnabled()) {
        cJSON *collider = cJSON_CreateObject();
        cJSON_AddNumberToObject(collider, "mode", o->getCollisionMode());
        cJSON_AddNumberToObject(collider, "shape", o->getCollisionShape());

        cJSON_AddNumberToObject(collider, "friction", o->friction);
        cJSON_AddNumberToObject(collider, "mass", o->mass);
        cJSON_AddNumberToObject(collider, "margin", o->shapeMargin);
        cJSON_AddNumberToObject(collider, "ccdMotionThreshold", o->ccdMotionThreshold);
        cJSON_AddNumberToObject(collider, "ccdSweptSphereRadius", o->ccdSweptSphereRadius);
        cJSON_AddNumberToObject(collider, "linearDamping", o->linearDamping);
        cJSON_AddNumberToObject(collider, "angularDamping", o->angularDamping);
        cJSON_AddNumberToObject(collider, "restitution", o->restitution);
        cJSON_AddBoolToObject(collider, "colliderStatic", o->colliderStatic);

        cJSON_AddItemToObject(collider, "simpleShapeSize", ToolsJSON::Vertex3DToJSON(o->simpleShapeSize));

        cJSON *kinematicCapsuleSizeJSON = cJSON_CreateObject();
        cJSON_AddNumberToObject(kinematicCapsuleSizeJSON, "x", o->kinematicCapsuleSize.x);
        cJSON_AddNumberToObject(kinematicCapsuleSizeJSON, "y", o->kinematicCapsuleSize.y);
        cJSON_AddItemToObject(collider, "kinematicCapsuleSize", kinematicCapsuleSizeJSON);

        cJSON_AddItemToObject(collider, "angularFactor", ToolsJSON::Vertex3DToJSON(o->angularFactor));
        cJSON_AddItemToObject(collider, "linearFactor", ToolsJSON::Vertex3DToJSON(o->linearFactor));

        cJSON_AddItemToObject(root, "collider", collider);
    }

    cJSON *scriptsArray = cJSON_CreateArray();
    for (auto script : o->scripts) {
        cJSON_AddItemToArray(scriptsArray, script->getTypesJSON());
    }
    cJSON_AddItemToObject(root, "scripts", scriptsArray);

    return root;
}

void Object3DSerializer::ApplyJsonToObject(cJSON *json, Object3D *o)
{
    std::lock_guard<std::mutex> lock(mtx);  // Bloquea

    LOG_MESSAGE("[Object3DSerializer] ApplyJsonToObject %d", (int) o->getTypeObject());

    o->setName(cJSON_GetObjectItem(json, "name")->valuestring);

    //Enabled
    o->setEnabled(false);
    if (cJSON_GetObjectItem(json, "enabled") != nullptr) {
        o->setEnabled(static_cast<bool>(cJSON_GetObjectItem(json, "enabled")->valueint));
    }

    // alpha
    o->setAlpha(1.0f);
    if (cJSON_GetObjectItem(json, "alpha") != nullptr) {
        o->setAlpha((float) cJSON_GetObjectItem(json, "alpha")->valuedouble);
    }

    // Render Settings
    if (cJSON_GetObjectItemCaseSensitive(json, "renderSettings") != nullptr) {
        auto renderSettingsJSON = cJSON_GetObjectItemCaseSensitive(json, "renderSettings");
        RenderSettings renderSettings;
        renderSettings.blend = (bool) cJSON_GetObjectItem(renderSettingsJSON, "blend")->valueint;
        renderSettings.culling = (bool) cJSON_GetObjectItem(renderSettingsJSON, "culling")->valueint;
        renderSettings.depthTest = (bool) cJSON_GetObjectItem(renderSettingsJSON, "depthTest")->valueint;
        renderSettings.writeDepth = (bool) cJSON_GetObjectItem(renderSettingsJSON, "writeDepth")->valueint;
        renderSettings.shadowMap = (bool) cJSON_GetObjectItem(renderSettingsJSON, "shadowMap")->valueint;
        renderSettings.mode_src = (bool) cJSON_GetObjectItem(renderSettingsJSON, "blend_function_src")->valueint;
        renderSettings.mode_dst = (bool) cJSON_GetObjectItem(renderSettingsJSON, "blend_function_dst")->valueint;
    }

    //Position
    o->setPosition(ToolsJSON::getVertex3DByJSON(cJSON_GetObjectItemCaseSensitive(json, "position")));
    o->setScale(static_cast<float>(cJSON_GetObjectItemCaseSensitive(json, "scale")->valuedouble));

    //Rotation
    if (cJSON_GetObjectItemCaseSensitive(json, "rotation") != nullptr) {
        o->setRotation(ToolsJSON::getRotationByJSON(cJSON_GetObjectItemCaseSensitive(json, "rotation")));
    }

    // Colliders
    if (cJSON_GetObjectItemCaseSensitive(json, "isCollisionsEnabled") != nullptr) {
        bool collisionsEnabled = cJSON_GetObjectItemCaseSensitive(json, "isCollisionsEnabled")->valueint;
        cJSON *colliderJSON = cJSON_GetObjectItemCaseSensitive(json, "collider");

        if (collisionsEnabled) {
            o->setCollisionsEnabled(true);
            if ((cJSON_GetObjectItemCaseSensitive(colliderJSON, "colliderStatic") != nullptr)) {
                o->setColliderStatic(cJSON_GetObjectItemCaseSensitive(colliderJSON, "colliderStatic")->valueint);
            }

            int mode = cJSON_GetObjectItemCaseSensitive(colliderJSON, "mode")->valueint;
            int shape = cJSON_GetObjectItemCaseSensitive(colliderJSON, "shape")->valueint;

            if (cJSON_GetObjectItemCaseSensitive(colliderJSON, "friction") != nullptr) {
                o->setFriction(static_cast<float>(cJSON_GetObjectItemCaseSensitive(colliderJSON, "friction")->valuedouble));
            }

            if (cJSON_GetObjectItemCaseSensitive(colliderJSON, "ccdMotionThreshold") != nullptr) {
                o->setCcdMotionThreshold(static_cast<float>(cJSON_GetObjectItemCaseSensitive(colliderJSON, "ccdMotionThreshold")->valuedouble));
            }

            if (cJSON_GetObjectItemCaseSensitive(colliderJSON, "ccdSweptSphereRadius") != nullptr) {
                o->setCcdSweptSphereRadius(static_cast<float>(cJSON_GetObjectItemCaseSensitive(colliderJSON, "ccdSweptSphereRadius")->valuedouble));
            }

            if (cJSON_GetObjectItemCaseSensitive(colliderJSON, "margin") != nullptr) {
                o->setShapeMargin(static_cast<float>(cJSON_GetObjectItemCaseSensitive(colliderJSON, "margin")->valuedouble));
            }

            if (cJSON_GetObjectItemCaseSensitive(colliderJSON, "restitution") != nullptr) {
                o->setRestitution(static_cast<float>(cJSON_GetObjectItemCaseSensitive(colliderJSON, "restitution")->valuedouble));
            }

            if (cJSON_GetObjectItemCaseSensitive(colliderJSON, "linearDamping") != nullptr) {
                o->setLinearDamping(static_cast<float>(cJSON_GetObjectItemCaseSensitive(colliderJSON, "linearDamping")->valuedouble));
            }

            if (cJSON_GetObjectItemCaseSensitive(colliderJSON, "angularDamping") != nullptr) {
                o->setAngularDamping(static_cast<float>(cJSON_GetObjectItemCaseSensitive(colliderJSON, "angularDamping")->valuedouble));
            }

            o->setSimpleShapeSize(ToolsJSON::getVertex3DByJSON(cJSON_GetObjectItemCaseSensitive(colliderJSON, "simpleShapeSize")));
            o->setMass(static_cast<float>(cJSON_GetObjectItemCaseSensitive(colliderJSON, "mass")->valuedouble));

            if (cJSON_GetObjectItemCaseSensitive(colliderJSON, "kinematicCapsuleSize") != nullptr) {
                cJSON *kinematicSizeJSON = cJSON_GetObjectItemCaseSensitive(colliderJSON, "kinematicCapsuleSize");

                o->setCapsuleColliderSize(
                    static_cast<float>(cJSON_GetObjectItemCaseSensitive(kinematicSizeJSON, "x")->valuedouble),
                    static_cast<float>(cJSON_GetObjectItemCaseSensitive(kinematicSizeJSON, "y")->valuedouble)
                );
            }

            if (cJSON_GetObjectItemCaseSensitive(colliderJSON, "angularFactor") != nullptr) {
                o->angularFactor = ToolsJSON::getVertex3DByJSON(
                    cJSON_GetObjectItemCaseSensitive(colliderJSON, "angularFactor")
                );
            }

            if (cJSON_GetObjectItemCaseSensitive(colliderJSON, "linearFactor") != nullptr) {
                o->linearFactor = ToolsJSON::getVertex3DByJSON(
                    cJSON_GetObjectItemCaseSensitive(colliderJSON, "linearFactor")
                );
            }

            switch(mode) {
                case GHOST:
                    if (shape == SIMPLE_SHAPE) {
                        o->SetupGhostCollider(SIMPLE_SHAPE);
                    }
                    break;
                case BODY:
                    if (shape == SIMPLE_SHAPE) {
                        o->SetupRigidBodyCollider(SIMPLE_SHAPE);
                    }
                    break;
                case KINEMATIC:
                    o->setupKinematicCollider();
                    break;
                default: {
                    LOG_MESSAGE("[Object3DSerializer ApplyJsonToObject %s] Fatal error: Unknown collision mode: %d", (int) o->getTypeObject(), mode);
                    exit(-1);
                }
            }
        }
    }

    // scripts
    if (cJSON_GetObjectItemCaseSensitive(json, "scripts") != nullptr) {
        cJSON *currentScript;
        cJSON_ArrayForEach(currentScript, cJSON_GetObjectItemCaseSensitive(json, "scripts")) {
            auto name = cJSON_GetObjectItemCaseSensitive(currentScript, "name")->valuestring;
            auto codeFile = cJSON_GetObjectItemCaseSensitive(currentScript, "codeFile")->valuestring;
            auto typesFile = cJSON_GetObjectItemCaseSensitive(currentScript, "typesFile")->valuestring;
            auto typesJSON = cJSON_GetObjectItemCaseSensitive(currentScript, "types");

            auto script = new ScriptLUA(name, codeFile, typesFile, typesJSON);

            // Read script type from JSON
            cJSON *typeJSON = cJSON_GetObjectItemCaseSensitive(currentScript, "type");
            if (typeJSON && typeJSON->valuestring) {
                script->setType((strcmp(typeJSON->valuestring, "Global") == 0) ? SCRIPT_GLOBAL : SCRIPT_OBJECT);
            }

            o->AttachScript(script);
        }
    }
}

Object3D * Object3DSerializer::ObjectByJson(cJSON *json)
{
    LOG_MESSAGE("[Object3DSerializer] ObjectByJson");

    auto obj = new Object3D();
    ApplyJsonToObject(json, obj);

    return obj;
}

void Object3DSerializer::MenuLoad(const std::string &file)
{
    auto o = new Object3D();
    Brakeza::get()->AddObject3D(o, Brakeza::UniqueObjectLabel("Object3D"));
}

#endif //BRAKEZA3D_OBJECT3DSERIALIZER_CPP_H