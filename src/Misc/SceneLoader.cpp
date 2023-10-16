//
// Created by eduardo on 10/10/23.
//

#include <utility>

#include "../../include/Misc/SceneLoader.h"
#include "../../include/EngineSetup.h"
#include "../../include/Misc/Tools.h"
#include "../../include/Render/Logging.h"
#include "../../include/Objects/Object3D.h"
#include "../../include/Objects/Mesh3D.h"
#include "../../include/Brakeza3D.h"

SceneLoader::SceneLoader()
{
}

void SceneLoader::loadScene(const std::string& filename)
{
    size_t file_size;
    auto contentFile = Tools::readFile(EngineSetup::get()->SCENES_FOLDER + filename, file_size);
    auto contentJSON = cJSON_Parse(contentFile);

    Logging::Message("Loading scene: %s", filename.c_str());

    auto camera = ComponentsManager::get()->getComponentCamera()->getCamera();

    cJSON *cameraJSON = cJSON_GetObjectItemCaseSensitive(contentJSON, "camera");

    auto cameraPositionJSON = cJSON_GetObjectItemCaseSensitive(cameraJSON, "position");
    auto cameraRotationJSON = cJSON_GetObjectItemCaseSensitive(cameraJSON, "rotation");

    camera->setPosition(parseVertex3DJSON(cameraPositionJSON));
    Logging::Message("Camera position (%f, %f, %f)", parseVertex3DJSON(cameraPositionJSON).x, parseVertex3DJSON(cameraPositionJSON).y, parseVertex3DJSON(cameraPositionJSON).z);
    camera->setRotationFromEulerAngles(
        (float) cJSON_GetObjectItemCaseSensitive(cameraRotationJSON, "x")->valuedouble,
        (float) cJSON_GetObjectItemCaseSensitive(cameraRotationJSON, "y")->valuedouble,
        (float) cJSON_GetObjectItemCaseSensitive(cameraRotationJSON, "z")->valuedouble
    );
    Logging::Message("Camera rotation (%f, %f, %f)", camera->pitch, camera->yaw, camera->roll);

    cJSON *currentObject;
    cJSON_ArrayForEach(currentObject, cJSON_GetObjectItemCaseSensitive(contentJSON, "objects")) {
        std::string typeObject = cJSON_GetObjectItemCaseSensitive(currentObject, "type")->valuestring;

        switch(sceneTypes[typeObject.c_str()]) {
            case SceneObjectLoaderMapping::Object3D : {
                Object3D::createFromJSON(currentObject);
                break;
            }
            case SceneObjectLoaderMapping::Mesh3D : {
                Mesh3D::createFromJSON(currentObject);
                break;
            }
            case SceneObjectLoaderMapping::LightPoint3D : {
                LightPoint3D::createFromJSON(currentObject);
                break;
            }
            case SceneObjectLoaderMapping::Mesh3DGhost : {
                Mesh3DGhost::createFromJSON(currentObject);
                break;
            }
            case SceneObjectLoaderMapping::Mesh3DBody : {
                Mesh3DBody::createFromJSON(currentObject);
                break;
            }
        }
    }
}

Vertex3D SceneLoader::parseVertex3DJSON(cJSON *vertex3DJSON)
{
    const auto x = (float) cJSON_GetObjectItemCaseSensitive(vertex3DJSON, "x")->valuedouble;
    const auto y = (float) cJSON_GetObjectItemCaseSensitive(vertex3DJSON, "y")->valuedouble;
    const auto z = (float) cJSON_GetObjectItemCaseSensitive(vertex3DJSON, "z")->valuedouble;

    return {x, y, z};
}

void SceneLoader::saveScene(const std::string &filename)
{
    cJSON *root = cJSON_CreateObject();

    cJSON *scriptsArray = cJSON_CreateArray();
    for (auto object : Brakeza3D::get()->getSceneObjects()) {
        if (!object->isBelongToScene()) continue;
        auto objectJson = object->getJSON();
        cJSON_AddStringToObject(objectJson, "type", object->getTypeObject());
        cJSON_AddItemToArray(scriptsArray, objectJson );
    }
    cJSON_AddItemToObject(root, "objects", scriptsArray);

    auto camera = ComponentsManager::get()->getComponentCamera()->getCamera();

    cJSON *cameraJSON = cJSON_CreateObject();
    cJSON *positionJSON = cJSON_CreateObject();
    cJSON_AddNumberToObject(positionJSON, "x", camera->getPosition().x);
    cJSON_AddNumberToObject(positionJSON, "y", camera->getPosition().y);
    cJSON_AddNumberToObject(positionJSON, "z", camera->getPosition().z);
    cJSON_AddItemToObject(cameraJSON, "position", positionJSON);

    cJSON *rotationJSON = cJSON_CreateObject();
    cJSON_AddNumberToObject(rotationJSON, "x", camera->getPitch());
    cJSON_AddNumberToObject(rotationJSON, "y", camera->getYaw());
    cJSON_AddNumberToObject(rotationJSON, "z", camera->getRoll());
    cJSON_AddItemToObject(cameraJSON, "rotation", rotationJSON);

    cJSON_AddItemToObject(root, "camera", cameraJSON);

    Logging::Message(cJSON_Print(root));
    Tools::writeToFile(EngineSetup::get()->SCENES_FOLDER + filename, cJSON_Print(root));
}

void SceneLoader::clearScene()
{
    for (auto object: Brakeza3D::get()->getSceneObjects()) {
        object->setRemoved(true);
    }
}


void SceneLoader::createObjectInScene() {
    auto o = new Object3D();
    o->setBelongToScene(true);
    Brakeza3D::get()->addObject3D(o, Brakeza3D::uniqueObjectLabel("new_object"));
}

void SceneLoader::createLightPointInScene() {
    auto o = new LightPoint3D(11, 1, 0, 0, 0, Color::red(), Color::green());
    o->setBelongToScene(true);
    Brakeza3D::get()->addObject3D(o, Brakeza3D::uniqueObjectLabel("new_object"));
}

void SceneLoader::createMesh3DBodyToScene(const std::string& filename, const char *name) {
    Vertex3D position = ComponentsManager::get()->getComponentCamera()->getCamera()->AxisForward().getScaled(10000);

    auto *newObject = new Mesh3DBody();
    newObject->setBelongToScene(true);
    newObject->setPosition(position);
    newObject->setScale(1);
    newObject->AssimpLoadGeometryFromFile(std::string(EngineSetup::get()->MODELS_FOLDER + filename));
    newObject->makeRigidBody(
        1,
        Brakeza3D::get()->getComponentsManager()->getComponentCollisions()->getDynamicsWorld(),
        btBroadphaseProxy::DefaultFilter,
         btBroadphaseProxy::DefaultFilter
     );

    Logging::Message("Loading Mesh3DBody from file: %s", std::string(EngineSetup::get()->MODELS_FOLDER + filename).c_str());

    Brakeza3D::get()->addObject3D(newObject, Brakeza3D::uniqueObjectLabel(name));
}


void SceneLoader::createGhostBody3DToScene(const std::string& filename, const char *name) {
    Vertex3D position = ComponentsManager::get()->getComponentCamera()->getCamera()->AxisForward().getScaled(10000);

    auto *newObject = new Mesh3DGhost();
    newObject->setBelongToScene(true);
    newObject->setPosition(position);
    newObject->setScale(1);
    newObject->AssimpLoadGeometryFromFile(std::string(EngineSetup::get()->MODELS_FOLDER + filename));
    newObject->updateBoundingBox();
    newObject->makeSimpleGhostBody(
            newObject->getAabb().size(),
            Brakeza3D::get()->getComponentsManager()->getComponentCollisions()->getDynamicsWorld(),
            btBroadphaseProxy::DefaultFilter,
            btBroadphaseProxy::DefaultFilter
    );
    Logging::Message("Loading GhostBody from file: %s", std::string(EngineSetup::get()->MODELS_FOLDER + filename).c_str());

    Brakeza3D::get()->addObject3D(newObject, Brakeza3D::uniqueObjectLabel(name));
}
