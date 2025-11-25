//
// Created by Eduardo on 23/11/2025.
//

#ifndef BRAKEZA3D_OBJECT3DSERIALIZER_CPP_H
#define BRAKEZA3D_OBJECT3DSERIALIZER_CPP_H

#include "../../include/Serializers/Object3DSerializer.h"
#include "../../include/Serializers/JSONSerializerRegistry.h"
#include "../../include/Misc/ToolsJSON.h"
#include "../../include/Brakeza.h"

cJSON * Object3DSerializer::JsonByObject(Object3D* object)
{
    cJSON *root = cJSON_CreateObject();

    cJSON_AddStringToObject(root, "name", object->getLabel().c_str());
    cJSON_AddNumberToObject(root, "scale", object->getScale());
    cJSON_AddBoolToObject(root, "enabled", object->isEnabled());

    cJSON *position = cJSON_CreateObject();
    cJSON_AddNumberToObject(position, "x", object->getPosition().x);
    cJSON_AddNumberToObject(position, "y", object->getPosition().y);
    cJSON_AddNumberToObject(position, "z", object->getPosition().z);
    cJSON_AddItemToObject(root, "position", position);

    cJSON *rotation = cJSON_CreateObject();
    cJSON_AddNumberToObject(rotation, "x", object->getRotation().getPitchDegree());
    cJSON_AddNumberToObject(rotation, "y", object->getRotation().getYawDegree());
    cJSON_AddNumberToObject(rotation, "z", object->getRotation().getRollDegree());
    cJSON_AddItemToObject(root, "rotation", rotation);

    cJSON_AddBoolToObject(root, "isCollisionsEnabled", object->isCollisionsEnabled());
    if (object->isCollisionsEnabled()) {
        cJSON *collider = cJSON_CreateObject();
        cJSON_AddNumberToObject(collider, "mode", object->getCollisionMode());
        cJSON_AddNumberToObject(collider, "shape", object->getCollisionShape());

        cJSON_AddNumberToObject(collider, "friction", object->friction);
        cJSON_AddNumberToObject(collider, "mass", object->mass);
        cJSON_AddNumberToObject(collider, "margin", object->shapeMargin);
        cJSON_AddNumberToObject(collider, "ccdMotionThreshold", object->ccdMotionThreshold);
        cJSON_AddNumberToObject(collider, "ccdSweptSphereRadius", object->ccdSweptSphereRadius);
        cJSON_AddNumberToObject(collider, "linearDamping", object->linearDamping);
        cJSON_AddNumberToObject(collider, "angularDamping", object->angularDamping);
        cJSON_AddNumberToObject(collider, "restitution", object->restitution);
        cJSON_AddBoolToObject(collider, "colliderStatic", object->colliderStatic);

        cJSON *simpleShapeSizeJSON = cJSON_CreateObject();
        cJSON_AddNumberToObject(simpleShapeSizeJSON, "x", object->simpleShapeSize.x);
        cJSON_AddNumberToObject(simpleShapeSizeJSON, "y", object->simpleShapeSize.y);
        cJSON_AddNumberToObject(simpleShapeSizeJSON, "z", object->simpleShapeSize.z);
        cJSON_AddItemToObject(collider, "simpleShapeSize", simpleShapeSizeJSON);

        cJSON *kinematicCapsuleSizeJSON = cJSON_CreateObject();
        cJSON_AddNumberToObject(kinematicCapsuleSizeJSON, "x", object->kinematicCapsuleSize.x);
        cJSON_AddNumberToObject(kinematicCapsuleSizeJSON, "y", object->kinematicCapsuleSize.y);
        cJSON_AddItemToObject(collider, "kinematicCapsuleSize", kinematicCapsuleSizeJSON);

        cJSON *angularFactorJSON = cJSON_CreateObject();
        cJSON_AddNumberToObject(angularFactorJSON, "x", object->angularFactor.x);
        cJSON_AddNumberToObject(angularFactorJSON, "y", object->angularFactor.y);
        cJSON_AddNumberToObject(angularFactorJSON, "z", object->angularFactor.z);
        cJSON_AddItemToObject(collider, "angularFactor", angularFactorJSON);

        cJSON *linearFactorJSON = cJSON_CreateObject();
        cJSON_AddNumberToObject(linearFactorJSON, "x", object->linearFactor.x);
        cJSON_AddNumberToObject(linearFactorJSON, "y", object->linearFactor.y);
        cJSON_AddNumberToObject(linearFactorJSON, "z", object->linearFactor.z);
        cJSON_AddItemToObject(collider, "linearFactor", linearFactorJSON);

        cJSON_AddItemToObject(root, "collider", collider);
    }

    cJSON *scriptsArray = cJSON_CreateArray();
    for (auto script : object->scripts) {
        cJSON_AddItemToArray(scriptsArray, script->getTypesJSON());
    }
    cJSON_AddItemToObject(root, "scripts", scriptsArray);

    return root;
}

void Object3DSerializer::ApplyJsonToObject(const cJSON *json, Object3D *o)
{
    std::cout << "[Object3DSerializer ApplyJsonToObject] " << o->getTypeObject() << std::endl;

    o->setLabel(cJSON_GetObjectItem(json, "name")->valuestring);

    //Enabled
    o->setEnabled(false);
    if (cJSON_GetObjectItem(json, "enabled") != nullptr) {
        o->setEnabled(static_cast<bool>(cJSON_GetObjectItem(json, "enabled")->valueint));
    }

    //Position
    o->setPosition(ToolsJSON::parseVertex3DJSON(cJSON_GetObjectItemCaseSensitive(json, "position")));
    o->setScale(static_cast<float>(cJSON_GetObjectItemCaseSensitive(json, "scale")->valuedouble));

    //Rotation
    if (cJSON_GetObjectItemCaseSensitive(json, "rotation") != nullptr) {
        o->setRotation(ToolsJSON::parseRotation3DJSON(cJSON_GetObjectItemCaseSensitive(json, "rotation")));
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

            o->setSimpleShapeSize(ToolsJSON::parseVertex3DJSON(cJSON_GetObjectItemCaseSensitive(colliderJSON, "simpleShapeSize")));
            o->setMass(static_cast<float>(cJSON_GetObjectItemCaseSensitive(colliderJSON, "mass")->valuedouble));

            if (cJSON_GetObjectItemCaseSensitive(colliderJSON, "kinematicCapsuleSize") != nullptr) {
                cJSON *kinematicSizeJSON = cJSON_GetObjectItemCaseSensitive(colliderJSON, "kinematicCapsuleSize");

                o->setCapsuleColliderSize(
                    static_cast<float>(cJSON_GetObjectItemCaseSensitive(kinematicSizeJSON, "x")->valuedouble),
                    static_cast<float>(cJSON_GetObjectItemCaseSensitive(kinematicSizeJSON, "y")->valuedouble)
                );
            }

            if (cJSON_GetObjectItemCaseSensitive(colliderJSON, "angularFactor") != nullptr) {
                o->angularFactor = ToolsJSON::parseVertex3DJSON(
                    cJSON_GetObjectItemCaseSensitive(colliderJSON, "angularFactor")
                );
            }

            if (cJSON_GetObjectItemCaseSensitive(colliderJSON, "linearFactor") != nullptr) {
                o->linearFactor = ToolsJSON::parseVertex3DJSON(
                    cJSON_GetObjectItemCaseSensitive(colliderJSON, "linearFactor")
                );
            }

            switch(mode) {
                case GHOST:
                    if (shape == SIMPLE_SHAPE) {
                        o->setupGhostCollider(SIMPLE_SHAPE);
                    }
                    break;
                case BODY:
                    if (shape == SIMPLE_SHAPE) {
                        o->setupRigidBodyCollider(SIMPLE_SHAPE);
                    }
                    break;
                case KINEMATIC:
                    o->setupKinematicCollider();
                    break;
                default: {
                    std::cout << "[Object3DSerializer ApplyJsonToObject] Fatal error: Unknown collision mode: " << mode << std::endl;
                    exit(-1);
                }
            }
        }
    }

    // scripts
    if (cJSON_GetObjectItemCaseSensitive(json, "scripts") != nullptr) {
        cJSON *currentScript;
        cJSON_ArrayForEach(currentScript, cJSON_GetObjectItemCaseSensitive(json, "scripts")) {
            auto filename = cJSON_GetObjectItemCaseSensitive(currentScript, "name")->valuestring;
            auto typesJSON = cJSON_GetObjectItemCaseSensitive(currentScript, "types");
            o->attachScript(new ScriptLUA(filename, typesJSON));
        }
    }
}

Object3D * Object3DSerializer::ObjectByJson(cJSON *json)
{
    std::cout << "[Object3DSerializer LoadJSONObject] Alive here..." << std::endl;
    auto obj = new Object3D();
    ApplyJsonToObject(json, obj);
    return obj;
}

void Object3DSerializer::LoadFileIntoScene(const std::string &file)
{
    auto o = new Object3D();
    Brakeza::get()->addObject3D(o, Brakeza::uniqueObjectLabel("Object3D"));

}

#endif //BRAKEZA3D_OBJECT3DSERIALIZER_CPP_H