//
// Created by Eduardo on 23/11/2025.
//

#ifndef BRAKEZA3D_OBJECT3DSERIALIZER_CPP_H
#define BRAKEZA3D_OBJECT3DSERIALIZER_CPP_H

#include "../../include/Persistence/Object3DSerializer.h"
#include "../../include/Persistence/JSONSerializerRegistry.h"

cJSON * Object3DSerializer::JsonByObject(Object3D* object)
{
    cJSON *root = cJSON_CreateObject();

    cJSON_AddStringToObject(root, "name", object->getLabel().c_str());
    cJSON_AddNumberToObject(root, "scale", object->getScale());

    cJSON *position = cJSON_CreateObject();
    cJSON_AddNumberToObject(position, "x", (float) object->getPosition().x);
    cJSON_AddNumberToObject(position, "y", (float) object->getPosition().y);
    cJSON_AddNumberToObject(position, "z", (float) object->getPosition().z);
    cJSON_AddItemToObject(root, "position", position);

    cJSON *rotation = cJSON_CreateObject();
    cJSON_AddNumberToObject(rotation, "x", (float) object->getRotation().getPitchDegree());
    cJSON_AddNumberToObject(rotation, "y", (float) object->getRotation().getYawDegree());
    cJSON_AddNumberToObject(rotation, "z", (float) object->getRotation().getRollDegree());
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

void Object3DSerializer::ApplyJsonToObject(cJSON *json, Object3D *obj)
{
    std::cout << "[Object3DSerializer ApplyJsonToObject] " << obj->getTypeObject() << std::endl;

    auto label = cJSON_GetObjectItem(json, "label");
    if (label) obj->setLabel(label->valuestring);

    auto enabled = cJSON_GetObjectItem(json, "enabled");
    if (enabled) obj->setEnabled(enabled->valueint);

    auto posJson = cJSON_GetObjectItem(json, "position");
    if (posJson) {
        Vertex3D pos(
            cJSON_GetObjectItem(posJson, "x")->valuedouble,
            cJSON_GetObjectItem(posJson, "y")->valuedouble,
            cJSON_GetObjectItem(posJson, "z")->valuedouble
        );
        obj->setPosition(pos);
    }

    auto scale = cJSON_GetObjectItem(json, "scale");
    if (scale) obj->setScale(scale->valuedouble);

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
}


#endif //BRAKEZA3D_OBJECT3DSERIALIZER_CPP_H