//
// Created by eduardo on 10/10/23.
//

#include "../../include/Misc/SceneLoader.h"
#include "../../include/EngineSetup.h"
#include "../../include/Misc/Tools.h"
#include "../../include/Render/Logging.h"
#include "../../include/Objects/Mesh3D.h"
#include "../../include/Brakeza3D.h"
#include "../../include/Objects/ParticleEmitter.h"
#include "../../include/2D/Image2DAnimation.h"
#include "../../include/Objects/BillboardAnimation.h"
#include "../../include/Objects/Image3D.h"
#include "../../include/2D/Image2D.h"
#include "../../include/Objects/Mesh3DAnimation.h"
#include "../../include/Objects/BillboardAnimation8Directions.h"
#include "../../include/Misc/ToolsJSON.h"

SceneLoader::SceneLoader() = default;

void SceneLoader::loadScene(const std::string& filename)
{
    size_t file_size;
    auto contentFile = Tools::readFile(filename, file_size);
    auto contentJSON = cJSON_Parse(contentFile);

    Logging::Message("Loading scene: %s", filename.c_str());

    auto camera = ComponentsManager::get()->getComponentCamera()->getCamera();
    auto shaderRender = ComponentsManager::get()->getComponentWindow()->getShaderOGLRender();

    cJSON *adsJSON = cJSON_GetObjectItemCaseSensitive(contentJSON, "ads");

    if (adsJSON != nullptr) {
        auto direction = ToolsJSON::parseVertex3DJSON(cJSON_GetObjectItemCaseSensitive(adsJSON, "direction"));
        auto ambient = ToolsJSON::parseVertex3DJSON(cJSON_GetObjectItemCaseSensitive(adsJSON, "ambient"));
        auto diffuse = ToolsJSON::parseVertex3DJSON(cJSON_GetObjectItemCaseSensitive(adsJSON, "diffuse"));
        auto specular = ToolsJSON::parseVertex3DJSON(cJSON_GetObjectItemCaseSensitive(adsJSON, "specular"));

        shaderRender->getDirectionalLight()->direction = direction.toGLM();
        shaderRender->getDirectionalLight()->ambient = ambient.toGLM();
        shaderRender->getDirectionalLight()->diffuse = diffuse.toGLM();
        shaderRender->getDirectionalLight()->specular = specular.toGLM();
    }

    cJSON *cameraJSON = cJSON_GetObjectItemCaseSensitive(contentJSON, "camera");

    auto cameraPositionJSON = cJSON_GetObjectItemCaseSensitive(cameraJSON, "position");
    auto cameraRotationJSON = cJSON_GetObjectItemCaseSensitive(cameraJSON, "rotation");

    camera->setPosition(ToolsJSON::parseVertex3DJSON(cameraPositionJSON));

    Logging::Message("Camera position (%f, %f, %f)",
        ToolsJSON::parseVertex3DJSON(cameraPositionJSON).x,
        ToolsJSON::parseVertex3DJSON(cameraPositionJSON).y,
        ToolsJSON::parseVertex3DJSON(cameraPositionJSON).z
    );

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
            case SceneObjectLoaderMapping::ParticleEmitter : {
                ParticleEmitter::createFromJSON(currentObject);
                break;
            }
            case SceneObjectLoaderMapping::BillboardAnimation : {
                BillboardAnimation::createFromJSON(currentObject);
                break;
            }
            case SceneObjectLoaderMapping::Image2DAnimation : {
                Image2DAnimation::createFromJSON(currentObject);
                break;
            }
            case SceneObjectLoaderMapping::Image2D : {
                Image2D::createFromJSON(currentObject);
                break;
            }
            case SceneObjectLoaderMapping::Image3D : {
                Image3D::createFromJSON(currentObject);
                break;
            }
            case SceneObjectLoaderMapping::BillboardAnimation8Directions : {
                BillboardAnimation8Directions::createFromJSON(currentObject);
                break;
            }
        }
    }

    cJSON *currentShaderJSON;
    cJSON_ArrayForEach(currentShaderJSON, cJSON_GetObjectItemCaseSensitive(contentJSON, "shaders")) {
        std::string nameFragmentCodeFile = cJSON_GetObjectItemCaseSensitive(currentShaderJSON, "file")->valuestring;
        std::string shaderCustomName = cJSON_GetObjectItemCaseSensitive(currentShaderJSON, "name")->valuestring;
        ComponentsManager::get()->getComponentRender()->addShaderToScene(
            new ShaderOpenGLCustom(shaderCustomName, nameFragmentCodeFile)
        );
    }

    if (cJSON_GetObjectItemCaseSensitive(contentJSON, "scripts") != nullptr) {
        cJSON *currentScript;
        cJSON_ArrayForEach(currentScript, cJSON_GetObjectItemCaseSensitive(contentJSON, "scripts")) {
            std::string fileName = (const char*) cJSON_GetObjectItemCaseSensitive(currentScript, "name")->valuestring;
            ComponentsManager::get()->getComponentRender()->addLUAScript(
                new ScriptLUA(fileName, ScriptLUA::dataTypesFileFor(fileName))
            );
        }
    }
}

void SceneLoader::saveScene(const std::string &filename)
{
    cJSON *root = cJSON_CreateObject();

    auto ads = ComponentsManager::get()->getComponentWindow()->getShaderOGLRender();
    // illumination ADS
    cJSON *adsJSON = cJSON_CreateObject();

    cJSON *adsDirectionJSON = cJSON_CreateObject();
    cJSON_AddNumberToObject(adsDirectionJSON, "x", ads->getDirectionalLight()->direction.x);
    cJSON_AddNumberToObject(adsDirectionJSON, "y", ads->getDirectionalLight()->direction.y);
    cJSON_AddNumberToObject(adsDirectionJSON, "z", ads->getDirectionalLight()->direction.z);
    cJSON_AddItemToObject(adsJSON, "direction", adsDirectionJSON);

    cJSON *adsDiffuseJSON = cJSON_CreateObject();
    cJSON_AddNumberToObject(adsDiffuseJSON, "x", ads->getDirectionalLight()->diffuse.x);
    cJSON_AddNumberToObject(adsDiffuseJSON, "y", ads->getDirectionalLight()->diffuse.y);
    cJSON_AddNumberToObject(adsDiffuseJSON, "z", ads->getDirectionalLight()->diffuse.z);
    cJSON_AddItemToObject(adsJSON, "diffuse", adsDiffuseJSON);

    cJSON *adsSpecularJSON = cJSON_CreateObject();
    cJSON_AddNumberToObject(adsSpecularJSON, "x", ads->getDirectionalLight()->specular.x);
    cJSON_AddNumberToObject(adsSpecularJSON, "y", ads->getDirectionalLight()->specular.y);
    cJSON_AddNumberToObject(adsSpecularJSON, "z", ads->getDirectionalLight()->specular.z);
    cJSON_AddItemToObject(adsJSON, "specular", adsSpecularJSON);

    cJSON *adsAmbientJSON = cJSON_CreateObject();
    cJSON_AddNumberToObject(adsAmbientJSON, "x", ads->getDirectionalLight()->ambient.x);
    cJSON_AddNumberToObject(adsAmbientJSON, "y", ads->getDirectionalLight()->ambient.y);
    cJSON_AddNumberToObject(adsAmbientJSON, "z", ads->getDirectionalLight()->ambient.z);
    cJSON_AddItemToObject(adsJSON, "ambient", adsAmbientJSON);

    cJSON_AddItemToObject(root, "ads", adsJSON);

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
        auto objectJson = shader->getTypesJSON();
        cJSON_AddItemToArray(shadersArrayJSON, objectJson);
    }
    cJSON_AddItemToObject(root, "shaders", shadersArrayJSON);

    //scripts
    cJSON *sceneScriptsArray = cJSON_CreateArray();
    for (auto script : ComponentsManager::get()->getComponentRender()->getLUAScripts()) {
        cJSON *scriptSceneSON = cJSON_CreateObject();
        cJSON_AddStringToObject(scriptSceneSON, "name", script->getScriptFilename().c_str());
        cJSON_AddItemToArray(sceneScriptsArray, scriptSceneSON);
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
    auto o = LightPoint3D::create(ComponentsManager::get()->getComponentCamera()->getCamera()->getPosition());
    o->setBelongToScene(true);
    Brakeza3D::get()->addObject3D(o, Brakeza3D::uniqueObjectLabel("new_object"));
}

void SceneLoader::createSpotLight3DInScene()
{
    auto o = SpotLight3D::create(
        ComponentsManager::get()->getComponentCamera()->getCamera()->getPosition(),
        Vertex3D(0, 0, 1)
    );
    o->setBelongToScene(true);
    Brakeza3D::get()->addObject3D(o, Brakeza3D::uniqueObjectLabel("SpotLight3D"));
}

void SceneLoader::createImage2DInScene(const std::string& filename)
{
    auto *newObject = new Image2D(
        EngineSetup::get()->screenWidth/2,
        EngineSetup::get()->screenHeight/2,
        new Image(filename)
    );

    newObject->setBelongToScene(true);
    Logging::Message("Loading Image2D");

    Brakeza3D::get()->addObject3D(newObject, Brakeza3D::uniqueObjectLabel("Image2D"));
}

void SceneLoader::createImage2DAnimatedInScene(const std::string& filename)
{
    auto *newObject = new Image2DAnimation(
        EngineSetup::get()->screenWidth/2,
        EngineSetup::get()->screenHeight/2,
        false,
        new TextureAnimated(filename,1,1,1,1)
    );
    newObject->setPosition(ComponentsManager::get()->getComponentCamera()->getCamera()->AxisForward().getScaled(2));
    newObject->setBelongToScene(true);

    Logging::Message("Loading Image2DAnimation");
    Brakeza3D::get()->addObject3D(newObject, Brakeza3D::uniqueObjectLabel("Image2DAnimation"));
}

void SceneLoader::createBillboardAnimationInScene(const std::string& filename)
{
    auto *newObject = new BillboardAnimation(1, 1);
    newObject->setPosition(ComponentsManager::get()->getComponentCamera()->getCamera()->AxisForward().getScaled(2));
    newObject->addAnimation(filename,1,1,1,1);
    newObject->setAnimation(0);

    newObject->setBelongToScene(true);
    Logging::Message("Loading BillboardAnimation");

    Brakeza3D::get()->addObject3D(newObject, Brakeza3D::uniqueObjectLabel("BillboardAnimation"));
}

void SceneLoader::createParticleEmitterInScene()
{
    auto *newObject = new ParticleEmitter(
            ParticleEmitterState::DEFAULT,
            nullptr,
            ComponentsManager::get()->getComponentCamera()->getCamera()->getPosition(),
            9999,
            Color::red(),
            Color::green(),
            ParticlesContext(
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
        ),
            nullptr
    );
    newObject->setBelongToScene(true);

    Logging::Message("Loading ParticleEmitter");
    Brakeza3D::get()->addObject3D(newObject, Brakeza3D::uniqueObjectLabel("new_particleEmitter"));
}

void SceneLoader::createImage3DToScene(const std::string &filename)
{
    auto *newObject = new Image3D(
        ComponentsManager::get()->getComponentCamera()->getCamera()->AxisForward().getScaled(10),
        10,
        10,
        new Image(filename)
    );
    newObject->setBelongToScene(true);

    Brakeza3D::get()->addObject3D(newObject, Brakeza3D::uniqueObjectLabel("Image3D"));
}

std::map<std::string, Mesh3DShaderLoaderMapping> &SceneLoader::getMesh3DShaderTypes() {
    return mesh3DShaderTypes;
}

void SceneLoader::createBillboardAnimation8Directions()
{
    auto *newObject = new BillboardAnimation8Directions(10, 10);
    newObject->setBelongToScene(true);
    newObject->setPosition(ComponentsManager::get()->getComponentCamera()->getCamera()->getPosition());

    Brakeza3D::get()->addObject3D(newObject, Brakeza3D::uniqueObjectLabel("Sprite3DDirectional"));
}

void SceneLoader::createMesh3D(const std::string& animationFile)
{
    auto *o = new Mesh3D();
    o->setBelongToScene(true);
    o->setPosition(ComponentsManager::get()->getComponentCamera()->getCamera()->getPosition());
    o->AssimpLoadGeometryFromFile(animationFile);

    Brakeza3D::get()->addObject3D(o, Brakeza3D::uniqueObjectLabel("Mesh3DAnimation"));
}

void SceneLoader::createMesh3DAnimationToScene(const std::string& animationFile)
{
    auto *o = new Mesh3DAnimation();
    o->setBelongToScene(true);
    o->setPosition(ComponentsManager::get()->getComponentCamera()->getCamera()->getPosition());
    o->AssimpLoadAnimation(animationFile);

    Brakeza3D::get()->addObject3D(o, Brakeza3D::uniqueObjectLabel("Mesh3DAnimation"));
}
