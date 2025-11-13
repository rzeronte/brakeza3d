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
#include "../../include/OpenGL/ShaderOpenGLCustomPostprocessing.h"

SceneLoader::SceneLoader() = default;

void SceneLoader::loadScene(const std::string& filename)
{
    size_t file_size;
    auto contentFile = Tools::readFile(filename, file_size);
    auto contentJSON = cJSON_Parse(contentFile);

    Logging::Message("Loading scene: %s", filename.c_str());

    auto camera = ComponentsManager::get()->getComponentCamera()->getCamera();
    auto shaderRender = ComponentsManager::get()->getComponentRender()->getShaderOGLRender();

    if (cJSON_GetObjectItemCaseSensitive(contentJSON, "gravity") != nullptr) {
        auto gravity = ToolsJSON::parseVertex3DJSON(cJSON_GetObjectItemCaseSensitive(contentJSON, "gravity"));
        EngineSetup::get()->gravity = gravity;
        ComponentsManager::get()->getComponentCollisions()->setGravity(gravity);
    }

    cJSON *adsJSON = cJSON_GetObjectItemCaseSensitive(contentJSON, "ads");

    if (adsJSON != nullptr) {
        auto direction = ToolsJSON::parseVertex3DJSON(cJSON_GetObjectItemCaseSensitive(adsJSON, "direction"));
        auto ambient = ToolsJSON::parseVertex3DJSON(cJSON_GetObjectItemCaseSensitive(adsJSON, "ambient"));
        auto diffuse = ToolsJSON::parseVertex3DJSON(cJSON_GetObjectItemCaseSensitive(adsJSON, "diffuse"));
        auto specular = ToolsJSON::parseVertex3DJSON(cJSON_GetObjectItemCaseSensitive(adsJSON, "specular"));

        shaderRender->getDirectionalLight().direction = direction.toGLM();
        shaderRender->getDirectionalLight().ambient = ambient.toGLM();
        shaderRender->getDirectionalLight().diffuse = diffuse.toGLM();
        shaderRender->getDirectionalLight().specular = specular.toGLM();
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

    auto pitch = (float) cJSON_GetObjectItemCaseSensitive(cameraRotationJSON, "x")->valuedouble;
    auto yaw = (float) cJSON_GetObjectItemCaseSensitive(cameraRotationJSON, "y")->valuedouble;
    auto roll = (float) cJSON_GetObjectItemCaseSensitive(cameraRotationJSON, "z")->valuedouble;

    camera->pitch = pitch;
    camera->yaw = yaw;
    camera->roll = roll;
    camera->setRotation(M3::getMatrixRotationForEulerAngles(pitch, yaw, roll));

    Logging::Message("Camera rotation (%f, %f, %f)", camera->pitch, camera->yaw, camera->roll);

    cJSON *currentObject;
    cJSON_ArrayForEach(currentObject, cJSON_GetObjectItemCaseSensitive(contentJSON, "objects")) {
        std::string typeObject = cJSON_GetObjectItemCaseSensitive(currentObject, "type")->valuestring;

        switch(SceneObjectTypesMapping[typeObject.c_str()]) {
            case SceneObjectLoaderMapping::Object3D : {
                Object3D::createFromJSON(currentObject);
                break;
            }
            case SceneObjectLoaderMapping::Mesh3D : {
                Mesh3D::createFromJSON(currentObject);
                break;
            }
            case SceneObjectLoaderMapping::Mesh3DAnimation : {
                Mesh3DAnimation::createFromJSON(currentObject);
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
        auto name = cJSON_GetObjectItemCaseSensitive(currentShaderJSON, "name")->valuestring;
        auto vertex = cJSON_GetObjectItemCaseSensitive(currentShaderJSON, "vertexshader")->valuestring;
        auto fragment = cJSON_GetObjectItemCaseSensitive(currentShaderJSON, "fragmentshader")->valuestring;
        auto types = cJSON_GetObjectItemCaseSensitive(currentShaderJSON, "types");

        auto shader = new ShaderOpenGLCustomPostprocessing(name, vertex, fragment, types);
        ComponentsManager::get()->getComponentRender()->addShaderToScene(shader);
    }

    if (cJSON_GetObjectItemCaseSensitive(contentJSON, "scripts") != nullptr) {
        cJSON *currentScript;
        cJSON_ArrayForEach(currentScript, cJSON_GetObjectItemCaseSensitive(contentJSON, "scripts")) {
            std::string fileName = (const char*) cJSON_GetObjectItemCaseSensitive(currentScript, "name")->valuestring;
            ComponentsManager::get()->getComponentScripting()->addSceneLUAScript(
                new ScriptLUA(fileName, ScriptLUA::dataTypesFileFor(fileName))
            );
        }
    }
}

void SceneLoader::saveScene(const std::string &filename)
{
    cJSON *root = cJSON_CreateObject();

    auto render = ComponentsManager::get()->getComponentRender()->getShaderOGLRender();

    // gravity
    cJSON *gravityJSON = cJSON_CreateObject();
    cJSON_AddNumberToObject(gravityJSON, "x", EngineSetup::get()->gravity.x);
    cJSON_AddNumberToObject(gravityJSON, "y", EngineSetup::get()->gravity.y);
    cJSON_AddNumberToObject(gravityJSON, "z", EngineSetup::get()->gravity.z);
    cJSON_AddItemToObject(root, "gravity", gravityJSON);

    // illumination ADS
    cJSON *adsJSON = cJSON_CreateObject();

    cJSON *adsDirectionJSON = cJSON_CreateObject();
    cJSON_AddNumberToObject(adsDirectionJSON, "x", render->getDirectionalLight().direction.x);
    cJSON_AddNumberToObject(adsDirectionJSON, "y", render->getDirectionalLight().direction.y);
    cJSON_AddNumberToObject(adsDirectionJSON, "z", render->getDirectionalLight().direction.z);
    cJSON_AddItemToObject(adsJSON, "direction", adsDirectionJSON);

    cJSON *adsDiffuseJSON = cJSON_CreateObject();
    cJSON_AddNumberToObject(adsDiffuseJSON, "x", render->getDirectionalLight().diffuse.x);
    cJSON_AddNumberToObject(adsDiffuseJSON, "y", render->getDirectionalLight().diffuse.y);
    cJSON_AddNumberToObject(adsDiffuseJSON, "z", render->getDirectionalLight().diffuse.z);
    cJSON_AddItemToObject(adsJSON, "diffuse", adsDiffuseJSON);

    cJSON *adsSpecularJSON = cJSON_CreateObject();
    cJSON_AddNumberToObject(adsSpecularJSON, "x", render->getDirectionalLight().specular.x);
    cJSON_AddNumberToObject(adsSpecularJSON, "y", render->getDirectionalLight().specular.y);
    cJSON_AddNumberToObject(adsSpecularJSON, "z", render->getDirectionalLight().specular.z);
    cJSON_AddItemToObject(adsJSON, "specular", adsSpecularJSON);

    cJSON *adsAmbientJSON = cJSON_CreateObject();
    cJSON_AddNumberToObject(adsAmbientJSON, "x", render->getDirectionalLight().ambient.x);
    cJSON_AddNumberToObject(adsAmbientJSON, "y", render->getDirectionalLight().ambient.y);
    cJSON_AddNumberToObject(adsAmbientJSON, "z", render->getDirectionalLight().ambient.z);
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
    for (auto script : ComponentsManager::get()->getComponentScripting()->getSceneLUAScripts()) {
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
    Tools::writeToFile(filename, cJSON_Print(root));
}

void SceneLoader::clearScene()
{
    Logging::Message("[SceneLoader] ClearScene");

    auto scripting = ComponentsManager::get()->getComponentScripting();

    scripting->stopLUAScripts();

    for (auto o: scripting->getSceneLUAScripts()) {
        scripting->removeSceneScript(o);
    }

    for (auto object: Brakeza3D::get()->getSceneObjects()) {
        if (!object->isMultiScene()) {
            object->setRemoved(true);
        }
    }
    auto render = ComponentsManager::get()->getComponentRender();
    for (auto &s: render->getSceneShaders()) {
        render->removeSceneShader(s);
    }

    Brakeza3D::get()->getManagerGui()->setSelectedObject(nullptr);
    ComponentsManager::get()->getComponentRender()->setSelectedObject(nullptr);
}

void SceneLoader::createObjectInScene()
{
    auto o = new Object3D();
    o->setBelongToScene(true);
    Brakeza3D::get()->addObject3D(o, Brakeza3D::uniqueObjectLabel("Object3D"));
}

void SceneLoader::createPointLight3DInScene()
{
    auto o = LightPoint3D::create(ComponentsManager::get()->getComponentCamera()->getCamera()->getPosition());
    o->setBelongToScene(true);
    Brakeza3D::get()->addObject3D(o, Brakeza3D::uniqueObjectLabel("LightPoint3D"));
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
    if (!Tools::fileExists(filename.c_str())) {
        Logging::Message("File %s not found", filename.c_str());
        return;
    }

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
    if (!Tools::fileExists(filename.c_str())) {
        Logging::Message("File %s not found", filename.c_str());
        return;
    }

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
    if (!Tools::fileExists(filename.c_str())) {
        Logging::Message("File %s not found", filename.c_str());
        return;
    }

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
    Brakeza3D::get()->addObject3D(newObject, Brakeza3D::uniqueObjectLabel("ParticleEmitter"));
}

void SceneLoader::createImage3DToScene(const std::string &filename)
{
    if (!Tools::fileExists(filename.c_str())) {
        Logging::Message("File %s not found", filename.c_str());
        return;
    }

    auto *newObject = new Image3D(
        ComponentsManager::get()->getComponentCamera()->getCamera()->getPosition(),
        10,
        10,
        new Image(filename)
    );
    newObject->setBelongToScene(true);

    Brakeza3D::get()->addObject3D(newObject, Brakeza3D::uniqueObjectLabel("Image3D"));
}

void SceneLoader::createBillboardAnimation8Directions()
{
    auto *newObject = new BillboardAnimation8Directions(10, 10);
    newObject->setBelongToScene(true);
    newObject->setPosition(ComponentsManager::get()->getComponentCamera()->getCamera()->getPosition());

    Brakeza3D::get()->addObject3D(newObject, Brakeza3D::uniqueObjectLabel("Billboard8D"));
}

void SceneLoader::createMesh3D(const std::string& animationFile)
{
    if (!Tools::fileExists(animationFile.c_str())) {
        Logging::Message("File %s not found", animationFile.c_str());
        return;
    }

    auto *o = new Mesh3D();
    o->setBelongToScene(true);
    o->setPosition(ComponentsManager::get()->getComponentCamera()->getCamera()->getPosition());
    o->AssimpLoadGeometryFromFile(animationFile);

    Brakeza3D::get()->addObject3D(o, Brakeza3D::uniqueObjectLabel("Mesh3D"));
}

void SceneLoader::createMesh3DAnimationToScene(const std::string& animationFile)
{
    if (!Tools::fileExists(animationFile.c_str())) {
        Logging::Message("File %s not found", animationFile.c_str());
        return;
    }

    auto *o = new Mesh3DAnimation();
    o->setBelongToScene(true);
    o->setPosition(ComponentsManager::get()->getComponentCamera()->getCamera()->getPosition());
    o->AssimpLoadAnimation(animationFile);

    Brakeza3D::get()->addObject3D(o, Brakeza3D::uniqueObjectLabel("Mesh3DAnimation"));
}

void SceneLoader::createScene(const std::string &filename)
{
    std::string sceneJsonFile = std::string(filename + ".json");

    Logging::Message("Creating new scene file: %s", sceneJsonFile.c_str());
    SceneLoader::saveScene(sceneJsonFile);
}

void SceneLoader::removeScene(const std::string &filename)
{
    if (!Tools::fileExists(filename.c_str())) {
        Logging::Message("File %s not found", filename.c_str());
        return;
    }

    Logging::Message("Deleting scene: %s", filename.c_str());

    Tools::removeFile(filename);
}
