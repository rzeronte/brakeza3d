//
// Created by eduardo on 10/10/23.
//

#include "../../include/Misc/SceneLoader.h"
#include "../../include/EngineSetup.h"
#include "../../include/Misc/Tools.h"
#include "../../include/Render/Logging.h"
#include "../../include/Objects/Object3D.h"
#include "../../include/Objects/Mesh3D.h"
#include "../../include/Brakeza3D.h"
#include "../../include/Objects/ParticleEmitter.h"
#include "../../include/2D/Sprite2D.h"

SceneLoader::SceneLoader()
= default;

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
            case SceneObjectLoaderMapping::SpotLight3D : {
                SpotLight3D::createFromJSON(currentObject);
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
            case SceneObjectLoaderMapping::ParticleEmitter : {
                ParticleEmitter::createFromJSON(currentObject);
                break;
            }
        }
    }

    cJSON *currentShaderJSON;
    cJSON_ArrayForEach(currentShaderJSON, cJSON_GetObjectItemCaseSensitive(contentJSON, "shaders")) {
        std::string typeObject = cJSON_GetObjectItemCaseSensitive(currentShaderJSON, "type")->valuestring;
        auto sceneShaderTypes = ComponentsManager::get()->getComponentRender()->getSceneLoader().getSceneShaderTypes();

        Logging::Message("Shader ID: %s", typeObject.c_str());

        switch(sceneShaderTypes[typeObject.c_str()]) {
            case SceneShaderLoaderMapping::FXColorTint : {
                Logging::Message("Setup FXColorTint shader...");
                auto shader = dynamic_cast<FXColorTint*> (ComponentsManager::get()->getComponentRender()->getSceneShaderByIndex(0));
                shader->setColor(parseColorJSON(cJSON_GetObjectItemCaseSensitive(currentShaderJSON, "color")));
                shader->setProgress((float) cJSON_GetObjectItemCaseSensitive(currentShaderJSON, "alpha")->valuedouble);
                shader->setEnabled(cJSON_GetObjectItemCaseSensitive(currentShaderJSON, "enabled")->valueint);
                break;
            }
            /*case SceneShaderLoaderMapping::ShaderImage : {
                auto shader = dynamic_cast<FXImage*> (ComponentsManager::get()->getComponentRender()->getSceneShaderByIndex(0));
                shader->getImage().setImage(cJSON_GetObjectItemCaseSensitive(currentShaderJSON, "image")->valuestring);
                shader->setOffsets(
                    (float) cJSON_GetObjectItemCaseSensitive(currentShaderJSON, "offsetX")->valuedouble,
                    (float) cJSON_GetObjectItemCaseSensitive(currentShaderJSON, "offsetY")->valuedouble
                );
                break;
            }*/
            /*case SceneShaderLoaderMapping::ShaderDepthOfField : {
                auto shader = dynamic_cast<ShaderDepthOfField*> (ComponentsManager::get()->getComponentRender()->getSceneShaderByIndex(1));
                shader->setup(
                    (float) cJSON_GetObjectItemCaseSensitive(currentShaderJSON, "focusPlaneDepth")->valuedouble,
                    (float) cJSON_GetObjectItemCaseSensitive(currentShaderJSON, "focusRange")->valuedouble,
                    (float) cJSON_GetObjectItemCaseSensitive(currentShaderJSON, "blurSize")->valuedouble,
                    (float) cJSON_GetObjectItemCaseSensitive(currentShaderJSON, "intensity")->valuedouble
                );
                break;
            }*/
        }
    }

    if (cJSON_GetObjectItemCaseSensitive(contentJSON, "scripts") != nullptr) {
        cJSON *currentScript;
        cJSON_ArrayForEach(currentScript, cJSON_GetObjectItemCaseSensitive(contentJSON, "scripts")) {
            std::string fileName = (const char*) currentScript->valuestring;
            ComponentsManager::get()->getComponentRender()->addLUAScript(
                new ScriptLUA(fileName, ScriptLUA::dataTypesFileFor(fileName))
            );
        }
    }}

Vertex3D SceneLoader::parseVertex3DJSON(cJSON *vertex3DJSON)
{
    const auto x = (float) cJSON_GetObjectItemCaseSensitive(vertex3DJSON, "x")->valuedouble;
    const auto y = (float) cJSON_GetObjectItemCaseSensitive(vertex3DJSON, "y")->valuedouble;
    const auto z = (float) cJSON_GetObjectItemCaseSensitive(vertex3DJSON, "z")->valuedouble;

    return {x, y, z};
}

Color SceneLoader::parseColorJSON(cJSON *color)
{
    return Color(
        cJSON_GetObjectItemCaseSensitive(color, "r")->valueint,
        cJSON_GetObjectItemCaseSensitive(color, "g")->valueint,
        cJSON_GetObjectItemCaseSensitive(color, "b")->valueint
    );
}

void SceneLoader::saveScene(const std::string &filename)
{
    cJSON *root = cJSON_CreateObject();

    //scripts
    cJSON *scriptsArray = cJSON_CreateArray();
    for (auto object : Brakeza3D::get()->getSceneObjects()) {
        if (!object->isBelongToScene()) continue;
        auto objectJson = object->getJSON();
        cJSON_AddStringToObject(objectJson, "type", object->getTypeObject());
        cJSON_AddItemToArray(scriptsArray, objectJson);
    }
    cJSON_AddItemToObject(root, "objects", scriptsArray);

    //shaders
    cJSON *shadersArrayJSON = cJSON_CreateArray();
    for (auto shader : ComponentsManager::get()->getComponentRender()->getSceneShaders()) {
        auto objectJson = shader->getJSON();
        cJSON_AddItemToArray(shadersArrayJSON, objectJson);
    }
    cJSON_AddItemToObject(root, "shaders", shadersArrayJSON);

    //scripts
    cJSON *sceneScriptsArray = cJSON_CreateArray();
    for (auto script : ComponentsManager::get()->getComponentRender()->getLUAScripts()) {
        cJSON_AddItemToArray(sceneScriptsArray, cJSON_CreateString(script->scriptFilename.c_str()));
    }
    cJSON_AddItemToObject(root, "scripts", sceneScriptsArray);

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
        if (!object->isMultiScene()) {
            object->setRemoved(true);
        }
    }
}

void SceneLoader::createObjectInScene() {
    auto o = new Object3D();
    o->setBelongToScene(true);
    Brakeza3D::get()->addObject3D(o, Brakeza3D::uniqueObjectLabel("new_object"));
}

void SceneLoader::createPointLight3DInScene()
{
    auto o = LightPoint3D::base();
    o->setPosition(ComponentsManager::get()->getComponentCamera()->getCamera()->getPosition());
    o->setBelongToScene(true);
    Brakeza3D::get()->addObject3D(o, Brakeza3D::uniqueObjectLabel("new_object"));
}

void SceneLoader::createSpotLight3DInScene() {
    auto o = new SpotLight3D(
            glm::vec4(0.0f, 0.0f, 1.0f, 0),
            glm::vec4(0.05f, 0.05f, 0.05f, 0),
            glm::vec4(1.0f, 0.0f, 0.0f, 0),
            glm::vec4(1.0f, 1.0f, 1.0f, 0),
            1.0f,
            0.09f,
            0.032f,
            1,
            1
    );
    o->setPosition(ComponentsManager::get()->getComponentCamera()->getCamera()->getPosition());
    o->setBelongToScene(true);
    Brakeza3D::get()->addObject3D(o, Brakeza3D::uniqueObjectLabel("new_lightpoint"));
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

void SceneLoader::createSprite2DInScene()
{
    Vertex3D position = ComponentsManager::get()->getComponentCamera()->getCamera()->AxisForward().getScaled(2);

    auto *newObject = new Sprite2D(
        EngineSetup::get()->screenWidth/2,
        EngineSetup::get()->screenHeight/2,
        false,
        new TextureAnimated(std::string(EngineSetup::get()->SPRITES_FOLDER + "Smoke45Frames.png"), 128, 128, 45, 24)
    );
    newObject->setPosition(position);

    newObject->setBelongToScene(true);
    Logging::Message("Loading Sprite2D");

    Brakeza3D::get()->addObject3D(newObject, Brakeza3D::uniqueObjectLabel("new_sprite3D"));
}


void SceneLoader::createSprite3DInScene()
{
    Vertex3D position = ComponentsManager::get()->getComponentCamera()->getCamera()->AxisForward().getScaled(2);

    auto *newObject = new Sprite3D(1, 1);
    newObject->setPosition(position);
    newObject->addAnimation(std::string(EngineSetup::get()->SPRITES_FOLDER + "Smoke45Frames.png"), 128, 128, 45, 24);
    newObject->setAnimation(0);

    newObject->setBelongToScene(true);
    Logging::Message("Loading Sprite3D");

    Brakeza3D::get()->addObject3D(newObject, Brakeza3D::uniqueObjectLabel("new_sprite3D"));
}

void SceneLoader::createParticleEmitterInScene() {
    Vertex3D position = ComponentsManager::get()->getComponentCamera()->getCamera()->AxisForward().getScaled(2);

    auto *newObject = new ParticleEmitter(
        ParticleEmitterState::DEFAULT,
        nullptr,
        position,
        9999,
        Color::red(),
        Color::green(),
        OCParticlesContext(
            0.0f,
            0.5f,
            1.5f,
            25.0f,
            1,
            10,
            125.0f,
            255.0f,
            1,
            1,
            0.99f
        )
    );
    newObject->setBelongToScene(true);
    Logging::Message("Loading ParticleEmitter");

    Brakeza3D::get()->addObject3D(newObject, Brakeza3D::uniqueObjectLabel("new_particleEmitter"));
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

std::map<std::string, SceneObjectLoaderMapping> &SceneLoader::getSceneTypes() {
    return sceneTypes;
}

std::map<std::string, Mesh3DShaderLoaderMapping> &SceneLoader::getMesh3DShaderTypes() {
    return mesh3DShaderTypes;
}

std::map<std::string, SceneShaderLoaderMapping> &SceneLoader::getSceneShaderTypes() {
    return sceneShaderTypes;
}
