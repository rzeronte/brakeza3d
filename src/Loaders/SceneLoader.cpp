//
// Created by eduardo on 10/10/23.
//

#include <filesystem>
#include "../../include/Loaders/SceneLoader.h"
#include "../../include/Config.h"
#include "../../include/GUI/Objects/FileSystemGUI.h"
#include "../../include/Misc/Tools.h"
#include "../../include/Misc/Logging.h"
#include "../../include/Components/Components.h"
#include "../../include/Brakeza.h"
#include "../../include/3D/ParticleEmitter.h"
#include "../../include/Misc/ToolsJSON.h"
#include "../../include/Serializers/Image2DAnimationSerializer.h"
#include "../../include/Threads/ThreadJobLoadObject.h"
#include "../../include/Render/JSONSerializerRegistry.h"
#include "../../include/Serializers/LightPointSerializer.h"
#include "../../include/Serializers/Object3DSerializer.h"
#include "../../include/Serializers/Mesh3DSerializer.h"
#include "../../include/Serializers/Mesh3DAnimationSerializer.h"
#include "../../include/Serializers/LightSpotSerializer.h"
#include "../../include/Serializers/ParticleEmmitterSerializer.h"
#include "../../include/Serializers/Image3DAnimation360Serializer.h"
#include "../../include/Serializers/Image2DSerializer.h"
#include "../../include/Serializers/Image3DSerializer.h"
#include "../../include/Serializers/Image3DAnimationSerializer.h"
#include "../../include/Serializers/SwarmSerializer.h"
#include "../../include/Threads/ThreadJobCleanWorld.h"
#include "../../include/Threads/ThreadJobClearWorld.h"
#include "../../include/Threads/ThreadJobReadFileScene.h"
#include "../../include/Render/EngineObserver.h"

bool SceneLoader::isLoading = false;
bool SceneLoader::isClearing = false;

SceneLoader::SceneLoader()
{
    InitSerializers();
}

void SceneLoader::LoadADSSettings(const cJSON *contentJSON)
{
    auto shaderRender = Components::get()->Render()->getShaders()->shaderOGLRender;

    cJSON *adsJSON = cJSON_GetObjectItemCaseSensitive(contentJSON, "ads");
    if (adsJSON == nullptr) return;

    auto direction = ToolsJSON::getVertex3DByJSON(cJSON_GetObjectItemCaseSensitive(adsJSON, "direction"));
    auto ambient   = ToolsJSON::getVertex3DByJSON(cJSON_GetObjectItemCaseSensitive(adsJSON, "ambient"));
    auto diffuse   = ToolsJSON::getVertex3DByJSON(cJSON_GetObjectItemCaseSensitive(adsJSON, "diffuse"));
    auto specular  = ToolsJSON::getVertex3DByJSON(cJSON_GetObjectItemCaseSensitive(adsJSON, "specular"));

    shaderRender->getDirectionalLight().direction = direction.toGLM();
    shaderRender->getDirectionalLight().ambient   = ambient.toGLM();
    shaderRender->getDirectionalLight().diffuse   = diffuse.toGLM();
    shaderRender->getDirectionalLight().specular  = specular.toGLM();
}

void SceneLoader::LoadCameraSettings(const cJSON *contentJSON)
{
    auto camera = Components::get()->Camera()->getCamera();

    cJSON *cameraJSON = cJSON_GetObjectItemCaseSensitive(contentJSON, "camera");
    if (cameraJSON == nullptr) return;

    auto cameraPositionJSON = cJSON_GetObjectItemCaseSensitive(cameraJSON, "position");
    auto cameraRotationJSON = cJSON_GetObjectItemCaseSensitive(cameraJSON, "rotation");

    auto cameraPosition = ToolsJSON::getVertex3DByJSON(cameraPositionJSON);
    camera->setPosition(cameraPosition);
    LOG_MESSAGE("[SceneLoader] Camera position set to (%f, %f, %f)", cameraPosition.x, cameraPosition.y, cameraPosition.z);

    auto pitch = static_cast<float>(cJSON_GetObjectItemCaseSensitive(cameraRotationJSON, "x")->valuedouble);
    auto yaw   = static_cast<float>(cJSON_GetObjectItemCaseSensitive(cameraRotationJSON, "y")->valuedouble);
    auto roll  = static_cast<float>(cJSON_GetObjectItemCaseSensitive(cameraRotationJSON, "z")->valuedouble);

    camera->getPitch() = pitch;
    camera->getYaw()   = yaw;
    camera->getRoll()  = roll;
    camera->setRotation(M3::getMatrixRotationForEulerAngles(pitch, yaw, roll));

    LOG_MESSAGE("[SceneLoader] Camera rotation set to (%f, %f, %f)", camera->getPitch(), camera->getYaw(), camera->getRoll());
}

void SceneLoader::LoadSceneSettings(const cJSON *contentJSON)
{
    LoadADSSettings(contentJSON);
    LoadCameraSettings(contentJSON);
}

void SceneLoader::LoadScene(const FilePath::SceneFile& filename)
{
    if (isLoading) {
        LOG_ERROR("[SceneLoader] Cannot do loading or cleaning while another same operation is working... Ignoring : '%s'", filename.c_str());
        return;
    }

    LOG_MESSAGE("[SceneLoader] Loading scene: '%s'", filename.c_str());
    if (Config::get()->OBSERVER_AI_ENABLED) EngineObserver::setScene(filename.str());

    isLoading = true;
    auto *scene = new Scene(filename.str());
    Brakeza::get()->PoolCompute().enqueueWithMainThreadCallback(
        std::make_shared<ThreadJobReadFileScene>(filename, scene)
    );
    FileSystemGUI::autoExpandScene = true;
}

void SceneLoader::LoadSceneAdditive(
    const FilePath::SceneFile& filename,
    bool loadScripts,
    bool loadShaders,
    bool loadCamera,
    bool loadRenderSettings
)
{
    if (isLoading) {
        LOG_ERROR("[SceneLoader] Cannot load while another operation is working... Ignoring: '%s'", filename.c_str());
        return;
    }

    LOG_MESSAGE("[SceneLoader] Additive scene load: '%s'", filename.c_str());
    isLoading = true;
    auto *scene = new Scene(filename.str());
    Brakeza::get()->PoolCompute().enqueueWithMainThreadCallback(
        std::make_shared<ThreadJobReadFileScene>(filename, scene, true, loadScripts, loadShaders, loadCamera, loadRenderSettings)
    );
}

void SceneLoader::setSceneActive(const std::string& name, bool active)
{
    auto *scene = Components::get()->Scripting()->getSceneByName(name);
    if (scene == nullptr) {
        LOG_ERROR("[SceneLoader] setSceneActive: scene '%s' not found", name.c_str());
        return;
    }
    scene->setActive(active);
}

void SceneLoader::setSceneHidden(const std::string& name, bool hidden)
{
    auto *scene = Components::get()->Scripting()->getSceneByName(name);
    if (scene == nullptr) {
        LOG_ERROR("[SceneLoader] setSceneHidden: scene '%s' not found", name.c_str());
        return;
    }
    scene->setHidden(hidden);
}

void SceneLoader::ReloadScene(const std::string& name)
{
    auto *scene = Components::get()->Scripting()->getSceneByName(name);
    if (scene == nullptr) {
        LOG_ERROR("[SceneLoader] ReloadScene: scene '%s' not found", name.c_str());
        return;
    }
    auto path = scene->getFilePath();
    UnloadScene(name);
    LoadSceneAdditive(FilePath::SceneFile(path), true, true, true, true);
}

void SceneLoader::UnloadScene(const std::string& name)
{
    auto *scripting = Components::get()->Scripting();
    auto *render    = Components::get()->Render();

    auto *scene = scripting->getSceneByName(name);
    if (scene == nullptr) {
        LOG_ERROR("[SceneLoader] UnloadScene: scene '%s' not found", name.c_str());
        return;
    }

    for (auto *obj : scene->getObjects())
        obj->setRemoved(true);

    auto scriptsCopy = scripting->getSceneScripts();
    for (auto *script : scriptsCopy) {
        if (script->getScene() == scene)
            scripting->RemoveSceneScript(script);
    }

    auto shadersCopy = render->getSceneShaders();
    for (auto *shader : shadersCopy) {
        if (shader->getScene() == scene)
            render->RemoveSceneShader(shader);
    }

    scripting->removeScene(name);
    LOG_MESSAGE("[SceneLoader] UnloadScene: '%s' unloaded", name.c_str());
}

void SceneLoader::SaveScene(const FilePath::SceneFile &filename)
{
    cJSON *root = cJSON_CreateObject();

    auto render = Components::get()->Render()->getShaders()->shaderOGLRender;

    auto sceneBaseName = std::filesystem::path(filename.str()).stem().string();
    auto screenshotPath = std::string(Config::get()->SCREENSHOTS_FOLDER + sceneBaseName + ".png");
    ComponentRender::MakeScreenShot(screenshotPath);
    cJSON_AddStringToObject(root, "screenshot", screenshotPath.c_str());

    // illumination ADS
    cJSON *adsJSON = cJSON_CreateObject();
    cJSON_AddItemToObject(adsJSON, "direction", ToolsJSON::Vertex3DToJSON(Vertex3D::fromGLM(render->getDirectionalLight().direction)));
    cJSON_AddItemToObject(adsJSON, "diffuse", ToolsJSON::Vertex3DToJSON(Vertex3D::fromGLM(render->getDirectionalLight().diffuse)));
    cJSON_AddItemToObject(adsJSON, "specular", ToolsJSON::Vertex3DToJSON(Vertex3D::fromGLM(render->getDirectionalLight().specular)));
    cJSON_AddItemToObject(adsJSON, "ambient", ToolsJSON::Vertex3DToJSON(Vertex3D::fromGLM(render->getDirectionalLight().ambient)));
    cJSON_AddItemToObject(root, "ads", adsJSON);

    //Objects
    cJSON *objectsArray = cJSON_CreateArray();
    for (auto &object : Brakeza::get()->getSceneObjects()) {
        auto objectJson = JSONSerializerRegistry::instance().serialize(object);

        cJSON_AddNumberToObject(objectJson, "type", (int) object->getTypeObject());
        cJSON_AddItemToArray(objectsArray, objectJson);
    }
    cJSON_AddItemToObject(root, "objects", objectsArray);

    //shaders
    cJSON *shadersArrayJSON = cJSON_CreateArray();
    for (auto &shader : Components::get()->Render()->getSceneShaders()) {
        auto shaderJson = shader->getTypesJSON();
        cJSON_AddBoolToObject(shaderJson, "enabled", shader->isEnabled());
        cJSON_AddItemToArray(shadersArrayJSON, shaderJson);
    }
    cJSON_AddItemToObject(root, "shaders", shadersArrayJSON);

    //sceneScripts
    cJSON *sceneScriptsArray = cJSON_CreateArray();
    for (auto &script : Components::get()->Scripting()->getSceneScripts()) {
        auto scriptJson = script->getTypesJSON();
        cJSON_AddItemToArray(sceneScriptsArray, scriptJson);
    }
    cJSON_AddItemToObject(root, "scripts", sceneScriptsArray);

    auto camera = Components::get()->Camera()->getCamera();

    cJSON *cameraJSON = cJSON_CreateObject();
    cJSON_AddItemToObject(cameraJSON, "position", ToolsJSON::Vertex3DToJSON(camera->getPosition()));
    cJSON_AddItemToObject(cameraJSON, "rotation", ToolsJSON::Vertex3DToJSON(Vertex3D(camera->getPitch(), camera->getYaw(), camera->getRoll())));
    cJSON_AddItemToObject(root, "camera", cameraJSON);

    Tools::WriteToFile(filename, cJSON_Print(root));
}

void SceneLoader::ClearWorld()
{
    if (isClearing) {
        LOG_ERROR("[SceneLoader] Cannot do a load or cleaning while another same operation is working... Ignoring");
    }

    isClearing = true;
    LOG_MESSAGE("[SceneLoader] ClearWorld");

    Brakeza::get()->PoolCompute().enqueueWithMainThreadCallback(std::make_shared<ThreadJobClearWorld>());
}

void SceneLoader::CleanWorld()
{
    LOG_MESSAGE("[SceneLoader] CleanWorld");
    Brakeza::get()->PoolCompute().enqueue(std::make_shared<ThreadJobCleanWorld>());
}

void SceneLoader::CreateScene(const FilePath::SceneFile &filename)
{
    auto sceneJsonFile = std::string(filename + ".json");

    LOG_MESSAGE("Creating new scene file: %s", sceneJsonFile.c_str());
    SaveScene(sceneJsonFile);
}

void SceneLoader::RemoveScene(const FilePath::SceneFile &filename)
{
    if (!Tools::FileExists(filename.c_str())) {
        LOG_MESSAGE("File %s not found", filename.c_str());
        return;
    }

    LOG_MESSAGE("Deleting scene: %s", filename.c_str());

    Tools::RemoveFile(filename);
}

void SceneLoader::InitSerializers()
{
    auto& registry = JSONSerializerRegistry::instance();

    registry.registerSerializer(ObjectType::Object3D, std::make_shared<Object3DSerializer>());
    registry.registerSerializer(ObjectType::Image2D, std::make_unique<Image2DSerializer>());
    registry.registerSerializer(ObjectType::Mesh3D, std::make_unique<Mesh3DSerializer>());
    registry.registerSerializer(ObjectType::Mesh3DAnimation, std::make_unique<Mesh3DAnimationSerializer>());
    registry.registerSerializer(ObjectType::Image3D, std::make_unique<Image3DSerializer>());
    registry.registerSerializer(ObjectType::Image3DAnimation, std::make_unique<Image3DAnimationSerializer>());
    registry.registerSerializer(ObjectType::Image3DAnimation360, std::make_unique<Image3DAnimation360Serializer>());
    registry.registerSerializer(ObjectType::LightPoint, std::make_unique<LightPointSerializer>());
    registry.registerSerializer(ObjectType::LightSpot, std::make_unique<LightSpotSerializer>());
    registry.registerSerializer(ObjectType::ParticleEmitter, std::make_unique<ParticleEmmitterSerializer>());
    registry.registerSerializer(ObjectType::Image2DAnimation, std::make_unique<Image2DAnimationSerializer>());
    registry.registerSerializer(ObjectType::Swarm, std::make_unique<SwarmSerializer>());
}

void SceneLoader::SceneLoaderCreateObject(cJSON *object)
{
    auto job = std::make_shared<ThreadJobLoadObject>(object);
    Brakeza::get()->PoolCompute().enqueue(job);
}
