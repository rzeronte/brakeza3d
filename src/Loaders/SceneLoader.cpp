//
// Created by eduardo on 10/10/23.
//

#include "../../include/Loaders/SceneLoader.h"
#include "../../include/BrakezaSetup.h"
#include "../../include/Misc/Tools.h"
#include "../../include/Misc/Logging.h"
#include "../../include/3D/Mesh3D.h"
#include "../../include/Brakeza.h"
#include "../../include/3D/ParticleEmitter.h"
#include "../../include/3D/Image3DAnimation.h"
#include "../../include/Misc/ToolsJSON.h"
#include "../../include/OpenGL/ShaderOGLCustomPostprocessing.h"
#include "../../include/Serializers/JSONSerializerRegistry.h"
#include "../../include/Serializers/LightPointSerializer.h"
#include "../../include/Serializers/Object3DSerializer.h"
#include "../../include/Serializers/Mesh3DSerializer.h"
#include "../../include/Serializers/Mesh3DAnimationSerializer.h"
#include "../../include/Serializers/LightSpotSerializer.h"
#include "../../include/Serializers/ParticleEmmitterSerializer.h"

SceneLoader::SceneLoader()
{
    InitSerializers();
}

void SceneLoader::LoadScene(const std::string& filename)
{
    std::cout <<"Loading '%s' scene" << filename.c_str() << std::endl;
    Logging::Message("Loading '%s' scene", filename.c_str());

    size_t file_size;
    auto contentFile = Tools::readFile(filename, file_size);
    auto contentJSON = cJSON_Parse(contentFile);

    auto camera = ComponentsManager::get()->getComponentCamera()->getCamera();
    auto shaderRender = ComponentsManager::get()->getComponentRender()->getShaderOGLRenderForward();

    if (cJSON_GetObjectItemCaseSensitive(contentJSON, "gravity") != nullptr) {
        auto gravity = ToolsJSON::getVertex3DByJSON(cJSON_GetObjectItemCaseSensitive(contentJSON, "gravity"));
        BrakezaSetup::get()->gravity = gravity;
        ComponentsManager::get()->getComponentCollisions()->setGravity(gravity);
    }

    cJSON *adsJSON = cJSON_GetObjectItemCaseSensitive(contentJSON, "ads");
    if (adsJSON != nullptr) {
        auto direction = ToolsJSON::getVertex3DByJSON(cJSON_GetObjectItemCaseSensitive(adsJSON, "direction"));
        auto ambient = ToolsJSON::getVertex3DByJSON(cJSON_GetObjectItemCaseSensitive(adsJSON, "ambient"));
        auto diffuse = ToolsJSON::getVertex3DByJSON(cJSON_GetObjectItemCaseSensitive(adsJSON, "diffuse"));
        auto specular = ToolsJSON::getVertex3DByJSON(cJSON_GetObjectItemCaseSensitive(adsJSON, "specular"));

        shaderRender->getDirectionalLight().direction = direction.toGLM();
        shaderRender->getDirectionalLight().ambient = ambient.toGLM();
        shaderRender->getDirectionalLight().diffuse = diffuse.toGLM();
        shaderRender->getDirectionalLight().specular = specular.toGLM();
    }

    cJSON *cameraJSON = cJSON_GetObjectItemCaseSensitive(contentJSON, "camera");

    auto cameraPositionJSON = cJSON_GetObjectItemCaseSensitive(cameraJSON, "position");
    auto cameraRotationJSON = cJSON_GetObjectItemCaseSensitive(cameraJSON, "rotation");

    camera->setPosition(ToolsJSON::getVertex3DByJSON(cameraPositionJSON));

    Logging::Message("Camera position (%f, %f, %f)",
        ToolsJSON::getVertex3DByJSON(cameraPositionJSON).x,
        ToolsJSON::getVertex3DByJSON(cameraPositionJSON).y,
        ToolsJSON::getVertex3DByJSON(cameraPositionJSON).z
    );

    auto pitch = static_cast<float>(cJSON_GetObjectItemCaseSensitive(cameraRotationJSON, "x")->valuedouble);
    auto yaw = static_cast<float>(cJSON_GetObjectItemCaseSensitive(cameraRotationJSON, "y")->valuedouble);
    auto roll = static_cast<float>(cJSON_GetObjectItemCaseSensitive(cameraRotationJSON, "z")->valuedouble);

    camera->getPitch() = pitch;
    camera->getYaw() = yaw;
    camera->getRoll() = roll;
    camera->setRotation(M3::getMatrixRotationForEulerAngles(pitch, yaw, roll));

    Logging::Message("Camera rotation (%f, %f, %f)", camera->getPitch(), camera->getYaw(), camera->getRoll());

    cJSON *currentObject;
    cJSON_ArrayForEach(currentObject, cJSON_GetObjectItemCaseSensitive(contentJSON, "objects")) {
        SceneLoaderCreateObject(currentObject);
    }

    cJSON *currentShaderJSON;
    cJSON_ArrayForEach(currentShaderJSON, cJSON_GetObjectItemCaseSensitive(contentJSON, "shaders")) {
        auto name = cJSON_GetObjectItemCaseSensitive(currentShaderJSON, "name")->valuestring;
        auto vertex = cJSON_GetObjectItemCaseSensitive(currentShaderJSON, "vertexshader")->valuestring;
        auto fragment = cJSON_GetObjectItemCaseSensitive(currentShaderJSON, "fragmentshader")->valuestring;
        auto types = cJSON_GetObjectItemCaseSensitive(currentShaderJSON, "types");

        auto shader = new ShaderOGLCustomPostprocessing(name, vertex, fragment, types);
        ComponentsManager::get()->getComponentRender()->addShaderToScene(shader);
    }

    if (cJSON_GetObjectItemCaseSensitive(contentJSON, "scripts") != nullptr) {
        cJSON *currentScript;
        cJSON_ArrayForEach(currentScript, cJSON_GetObjectItemCaseSensitive(contentJSON, "scripts")) {
            std::string fileName = cJSON_GetObjectItemCaseSensitive(currentScript, "name")->valuestring;
            ComponentsManager::get()->getComponentScripting()->addSceneLUAScript(
                new ScriptLUA(fileName, ScriptLUA::dataTypesFileFor(fileName))
            );
        }
    }
}

void SceneLoader::SaveScene(const std::string &filename)
{
    cJSON *root = cJSON_CreateObject();

    auto render = ComponentsManager::get()->getComponentRender()->getShaderOGLRenderForward();

    cJSON_AddItemToObject(root, "gravity", ToolsJSON::Vertex3DToJSON(BrakezaSetup::get()->gravity));

    // illumination ADS
    cJSON *adsJSON = cJSON_CreateObject();
    cJSON_AddItemToObject(adsJSON, "direction", ToolsJSON::Vertex3DToJSON(Vertex3D::fromGLM(render->getDirectionalLight().direction)));
    cJSON_AddItemToObject(adsJSON, "diffuse", ToolsJSON::Vertex3DToJSON(Vertex3D::fromGLM(render->getDirectionalLight().diffuse)));
    cJSON_AddItemToObject(adsJSON, "specular", ToolsJSON::Vertex3DToJSON(Vertex3D::fromGLM(render->getDirectionalLight().specular)));
    cJSON_AddItemToObject(adsJSON, "ambient", ToolsJSON::Vertex3DToJSON(Vertex3D::fromGLM(render->getDirectionalLight().ambient)));
    cJSON_AddItemToObject(root, "ads", adsJSON);

    //Objects
    cJSON *objectsArray = cJSON_CreateArray();
    for (auto object : Brakeza::get()->getSceneObjects()) {
        //auto objectJson = JSONSerializerRegistry::instance().serialize(object);
        auto objectJson = JSONSerializerRegistry::instance().serialize(object);

        cJSON_AddStringToObject(objectJson, "type", object->getTypeObject());
        cJSON_AddItemToArray(objectsArray, objectJson);
    }
    cJSON_AddItemToObject(root, "objects", objectsArray);

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
    cJSON_AddItemToObject(cameraJSON, "position", ToolsJSON::Vertex3DToJSON(camera->getPosition()));
    cJSON_AddItemToObject(cameraJSON, "rotation", ToolsJSON::Vertex3DToJSON(Vertex3D(camera->getPitch(), camera->getYaw(), camera->getRoll())));
    cJSON_AddItemToObject(root, "camera", cameraJSON);

    Tools::writeToFile(filename, cJSON_Print(root));
}

void SceneLoader::ClearScene()
{
    Logging::Message("[SceneLoader] ClearScene");

    auto scripting = ComponentsManager::get()->getComponentScripting();

    scripting->stopLUAScripts();

    for (auto o: scripting->getSceneLUAScripts()) {
        scripting->removeSceneScript(o);
    }

    for (auto object: Brakeza::get()->getSceneObjects()) {
        if (!object->isMultiScene()) {
            object->setRemoved(true);
        }
    }
    auto render = ComponentsManager::get()->getComponentRender();
    for (auto &s: render->getSceneShaders()) {
        render->removeSceneShader(s);
    }

    Brakeza::get()->getManagerGui()->setSelectedObject(nullptr);
    ComponentsManager::get()->getComponentRender()->setSelectedObject(nullptr);
}

void SceneLoader::CreateScene(const std::string &filename)
{
    auto sceneJsonFile = std::string(filename + ".json");

    Logging::Message("Creating new scene file: %s", sceneJsonFile.c_str());
    SaveScene(sceneJsonFile);
}

void SceneLoader::RemoveScene(const std::string &filename)
{
    if (!Tools::fileExists(filename.c_str())) {
        Logging::Message("File %s not found", filename.c_str());
        return;
    }

    Logging::Message("Deleting scene: %s", filename.c_str());

    Tools::removeFile(filename);
}

void SceneLoader::InitSerializers()
{
    auto& registry = JSONSerializerRegistry::instance();

    registry.registerSerializer(SceneObjectTypes::OBJECT_3D, std::make_shared<Object3DSerializer>());
    registry.registerSerializer(SceneObjectTypes::IMAGE_2D, std::make_unique<Image2DSerializer>());
    registry.registerSerializer(SceneObjectTypes::MESH_3D, std::make_unique<Mesh3DSerializer>());
    registry.registerSerializer(SceneObjectTypes::MESH_3D_ANIMATION, std::make_unique<Mesh3DAnimationSerializer>());
    registry.registerSerializer(SceneObjectTypes::IMAGE_3D, std::make_unique<Image3DSerializer>());
    registry.registerSerializer(SceneObjectTypes::IMAGE_3D_ANIMATION, std::make_unique<Image3DAnimationSerializer>());
    registry.registerSerializer(SceneObjectTypes::IMAGE_3D_ANIMATION_8DIR, std::make_unique<Image3DAnimation8DirectionsSerializer>());
    registry.registerSerializer(SceneObjectTypes::LIGHT_SPOT, std::make_unique<LightSpotSerializer>());
    registry.registerSerializer(SceneObjectTypes::LIGHT_POINT, std::make_unique<LightPointSerializer>());
    registry.registerSerializer(SceneObjectTypes::PARTICLE_EMITTER, std::make_unique<ParticleEmmitterSerializer>());
}

void SceneLoader::SceneLoaderCreateObject(cJSON *object)
{
    auto o = JSONSerializerRegistry::instance().deserialize(object);
    o->setBelongToScene(true);
    std::cout << "[SceneLoader SceneLoaderCreateObject] " << o->getLabel() << std::endl;

    Brakeza::get()->addObject3D(o, o->getLabel());
}