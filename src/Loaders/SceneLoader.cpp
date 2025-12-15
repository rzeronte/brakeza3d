//
// Created by eduardo on 10/10/23.
//

#include "../../include/Loaders/SceneLoader.h"
#include "../../include/Config.h"
#include "../../include/Misc/Tools.h"
#include "../../include/Misc/Logging.h"
#include "../../include/Components/Components.h"
#include "../../include/Brakeza.h"
#include "../../include/3D/ParticleEmitter.h"
#include "../../include/Misc/ToolsJSON.h"
#include "../../include/OpenGL/ShaderOGLCustomPostprocessing.h"
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
#include "../../include/Threads/ThreadJobClearScene.h"
#include "../../include/Threads/ThreadJobReadFileScene.h"

bool SceneLoader::isLoading = false;
bool SceneLoader::isClearing = false;

SceneLoader::SceneLoader()
{
    InitSerializers();
}

void SceneLoader::LoadSceneSettings(const cJSON *contentJSON)
{
    auto camera = Components::get()->Camera()->getCamera();
    auto shaderRender = Components::get()->Render()->getShaders()->shaderOGLRender;

    // GRAVITY
    if (cJSON_GetObjectItemCaseSensitive(contentJSON, "gravity") != nullptr) {
        auto gravity = ToolsJSON::getVertex3DByJSON(cJSON_GetObjectItemCaseSensitive(contentJSON, "gravity"));
        Config::get()->gravity = gravity;
        Components::get()->Collisions()->setGravity(gravity);
    }

    // ADS ILLUMINATION
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

    // CAMERA POSITION/ROTATION
    cJSON *cameraJSON = cJSON_GetObjectItemCaseSensitive(contentJSON, "camera");
    auto cameraPositionJSON = cJSON_GetObjectItemCaseSensitive(cameraJSON, "position");
    auto cameraRotationJSON = cJSON_GetObjectItemCaseSensitive(cameraJSON, "rotation");

    auto cameraPosition = ToolsJSON::getVertex3DByJSON(cameraPositionJSON);
    camera->setPosition(cameraPosition);
    Logging::Message("[SceneLoader] Camera position set to (%f, %f, %f)", cameraPosition.x, cameraPosition.y, cameraPosition.z );

    auto pitch = static_cast<float>(cJSON_GetObjectItemCaseSensitive(cameraRotationJSON, "x")->valuedouble);
    auto yaw = static_cast<float>(cJSON_GetObjectItemCaseSensitive(cameraRotationJSON, "y")->valuedouble);
    auto roll = static_cast<float>(cJSON_GetObjectItemCaseSensitive(cameraRotationJSON, "z")->valuedouble);

    camera->getPitch() = pitch;
    camera->getYaw() = yaw;
    camera->getRoll() = roll;
    camera->setRotation(M3::getMatrixRotationForEulerAngles(pitch, yaw, roll));

    Logging::Message("[SceneLoader] Camera rotation set to (%f, %f, %f)", camera->getPitch(), camera->getYaw(), camera->getRoll());
}

void SceneLoader::LoadScene(const std::string& filename)
{
    if (isLoading) {
        Logging::Error("[SceneLoader] Cannot do loading or cleaning while another same operation is working... Ignoring : '%s'", filename.c_str());
        return;
    }

    Logging::Message("[SceneLoader] Loading scene: '%s'", filename.c_str());

    isLoading = true;
    auto job = std::make_shared<ThreadJobReadFileScene>(filename);
    Brakeza::get()->PoolCompute().enqueueWithMainThreadCallback(job);
}

void SceneLoader::SaveScene(const std::string &filename)
{
    cJSON *root = cJSON_CreateObject();

    auto render = Components::get()->Render()->getShaders()->shaderOGLRender;

    cJSON_AddItemToObject(root, "gravity", ToolsJSON::Vertex3DToJSON(Config::get()->gravity));

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
        auto objectJson = JSONSerializerRegistry::instance().serialize(object);

        cJSON_AddNumberToObject(objectJson, "type", (int) object->getTypeObject());
        cJSON_AddItemToArray(objectsArray, objectJson);
    }
    cJSON_AddItemToObject(root, "objects", objectsArray);

    //shaders
    cJSON *shadersArrayJSON = cJSON_CreateArray();
    for (auto shader : Components::get()->Render()->getSceneShaders()) {
        auto objectJson = shader->getTypesJSON();
        cJSON_AddItemToArray(shadersArrayJSON, objectJson);
    }
    cJSON_AddItemToObject(root, "shaders", shadersArrayJSON);

    //scripts
    cJSON *sceneScriptsArray = cJSON_CreateArray();
    for (auto script : Components::get()->Scripting()->getSceneLUAScripts()) {
        cJSON *scriptSceneSON = cJSON_CreateObject();
        cJSON_AddStringToObject(scriptSceneSON, "name", script->getScriptFilename().c_str());
        cJSON_AddItemToArray(sceneScriptsArray, scriptSceneSON);
    }
    cJSON_AddItemToObject(root, "scripts", sceneScriptsArray);

    auto camera = Components::get()->Camera()->getCamera();

    cJSON *cameraJSON = cJSON_CreateObject();
    cJSON_AddItemToObject(cameraJSON, "position", ToolsJSON::Vertex3DToJSON(camera->getPosition()));
    cJSON_AddItemToObject(cameraJSON, "rotation", ToolsJSON::Vertex3DToJSON(Vertex3D(camera->getPitch(), camera->getYaw(), camera->getRoll())));
    cJSON_AddItemToObject(root, "camera", cameraJSON);

    Tools::WriteToFile(filename, cJSON_Print(root));
}

void SceneLoader::ClearScene()
{
    if (isClearing) {
        Logging::Error("[SceneLoader] Cannot do a load or cleaning while another same operation is working... Ignoring");
    }

    isClearing = true;
    Logging::Message("[SceneLoader] ClearScene");

    Brakeza::get()->PoolCompute().enqueue(std::make_shared<ThreadJobClearScene>());
}

void SceneLoader::CreateScene(const std::string &filename)
{
    auto sceneJsonFile = std::string(filename + ".json");

    Logging::Message("Creating new scene file: %s", sceneJsonFile.c_str());
    SaveScene(sceneJsonFile);
}

void SceneLoader::RemoveScene(const std::string &filename)
{
    if (!Tools::FileExists(filename.c_str())) {
        Logging::Message("File %s not found", filename.c_str());
        return;
    }

    Logging::Message("Deleting scene: %s", filename.c_str());

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
}

void SceneLoader::SceneLoaderCreateObject(cJSON *object)
{
    auto job = std::make_shared<ThreadJobLoadObject>(object);
    Brakeza::get()->PoolCompute().enqueue(job);
}
