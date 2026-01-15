//
// Created by Eduardo on 28/01/2025.
//

#include "../../include/Loaders/ProjectLoader.h"
#include "../../include/Misc/Tools.h"
#include "../../include/Misc/Logging.h"
#include "../../include/Components/Components.h"
#include "../../include/Misc/ToolsJSON.h"


void ProjectLoader::LoadProject(const std::string &filename)
{
    RemoveProjectScripts();

    auto contentFile = Tools::ReadFile(filename);
    auto contentJSON = cJSON_Parse(contentFile);

    LOG_MESSAGE("[ProjectLoader] Loading project file '%s'...", filename.c_str());

    if (cJSON_GetObjectItemCaseSensitive(contentJSON, "name") != nullptr) {
        auto sceneName = cJSON_GetObjectItemCaseSensitive(contentJSON, "name")->valuestring;
        Components::get()->Window()->setWindowTitle(sceneName);
        Config::get()->ENGINE_TITLE = sceneName;
    }

    // scripts
    if (cJSON_GetObjectItemCaseSensitive(contentJSON, "scripts") != nullptr) {
        cJSON *currentScript;
        cJSON_ArrayForEach(currentScript, cJSON_GetObjectItemCaseSensitive(contentJSON, "scripts")) {
            std::string name = cJSON_GetObjectItemCaseSensitive(currentScript, "name")->valuestring;
            std::string codeScript = cJSON_GetObjectItemCaseSensitive(currentScript, "codeFile")->valuestring;
            std::string typesFile = cJSON_GetObjectItemCaseSensitive(currentScript, "typesFile")->valuestring;
            Components::get()->Scripting()->AddProjectLUAScript(new ScriptLUA(name, codeScript, typesFile));
        }
    }

    // scenes
    if (cJSON_GetObjectItemCaseSensitive(contentJSON, "scenes") != nullptr) {
        cJSON *currentScene;
        cJSON_ArrayForEach(currentScene, cJSON_GetObjectItemCaseSensitive(contentJSON, "scenes")) {
            std::string path = cJSON_GetObjectItemCaseSensitive(currentScene, "path")->valuestring;
            Components::get()->Scripting()->AddProjectScene(path);
        }
    }

    // RESOLUTION
    if (cJSON_GetObjectItemCaseSensitive(contentJSON, "resolution") != nullptr) {
        auto resolutionJSON = cJSON_GetObjectItemCaseSensitive(contentJSON, "resolution");

        Components::get()->Window()->setRendererSize(
            cJSON_GetObjectItemCaseSensitive(resolutionJSON, "width")->valueint,
            cJSON_GetObjectItemCaseSensitive(resolutionJSON, "height")->valueint
        );
    }

    // GRAVITY
    if (cJSON_GetObjectItemCaseSensitive(contentJSON, "gravity") != nullptr) {
        auto gravity = ToolsJSON::getVertex3DByJSON(cJSON_GetObjectItemCaseSensitive(contentJSON, "gravity"));
        Config::get()->gravity = gravity;
        Components::get()->Collisions()->setGravity(gravity);
    }

    // volumes
    if (cJSON_GetObjectItemCaseSensitive(contentJSON, "sound") != nullptr) {
        auto soundJSON = cJSON_GetObjectItemCaseSensitive(contentJSON, "sound");
        Config::get()->SOUND_VOLUME_MUSIC = cJSON_GetObjectItemCaseSensitive(soundJSON, "volume_music")->valuedouble;;
        Config::get()->SOUND_VOLUME_FX = cJSON_GetObjectItemCaseSensitive(soundJSON, "volume_fx")->valuedouble;;
        Mix_Volume(Config::SoundChannels::SND_GLOBAL, Config::get()->SOUND_VOLUME_FX);
        Mix_VolumeMusic(Config::get()->SOUND_VOLUME_MUSIC);
    }
}

void ProjectLoader::SaveProject(const std::string &filename)
{
    cJSON *root = cJSON_CreateObject();

    cJSON_AddStringToObject(root, "name", Config::get()->ENGINE_TITLE.c_str());
    cJSON_AddItemToObject(root, "gravity", ToolsJSON::Vertex3DToJSON(Config::get()->gravity));

    // scripts
    cJSON *sceneScriptsArray = cJSON_CreateArray();
    for (auto &script : Components::get()->Scripting()->getProjectLUAScripts()) {
        cJSON *scriptSceneSON = cJSON_CreateObject();
        cJSON_AddStringToObject(scriptSceneSON, "name", script->getScriptFilename().c_str());
        cJSON_AddItemToArray(sceneScriptsArray, scriptSceneSON);
    }
    cJSON_AddItemToObject(root, "scripts", sceneScriptsArray);

    // scenes
    cJSON *scenesArray = cJSON_CreateArray();
    for (auto &scenePath : Components::get()->Scripting()->getProjectScenes()) {
        cJSON *sceneJSON = cJSON_CreateObject();
        cJSON_AddStringToObject(sceneJSON, "path", scenePath.c_str());
        cJSON_AddItemToArray(scenesArray, sceneJSON);
    }
    cJSON_AddItemToObject(root, "scenes", scenesArray);

    // sound
    cJSON *soundJSON = cJSON_CreateObject();
    cJSON_AddNumberToObject(soundJSON, "volume_fx", Config::get()->SOUND_VOLUME_FX);
    cJSON_AddNumberToObject(soundJSON, "volume_music", Config::get()->SOUND_VOLUME_MUSIC);
    cJSON_AddItemToObject(root, "sound", soundJSON);

    // render resolution
    auto window = Components::get()->Window();
    cJSON *resolutionJSON = cJSON_CreateObject();
    cJSON_AddNumberToObject(resolutionJSON, "width", window->getWidthRender());
    cJSON_AddNumberToObject(resolutionJSON, "height", window->getHeightRender());
    cJSON_AddItemToObject(root, "resolution", resolutionJSON);

    Tools::WriteToFile(filename, cJSON_Print(root));
}

void ProjectLoader::RemoveProjectScripts()
{
    auto scripting = Components::get()->Scripting();
    for (auto o: scripting->getProjectLUAScripts()) {
        scripting->RemoveProjectScript(o);
    }
}

void ProjectLoader::CreateProject(const std::string &filename)
{
    LOG_MESSAGE("Creating new project file: %s", filename.c_str());

    cJSON *root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "name", filename.c_str());

    auto projectJsonFile = std::string(filename + ".json");

    Tools::WriteToFile(projectJsonFile, cJSON_Print(root));
}

void ProjectLoader::RemoveProject(const std::string &filename)
{
    LOG_MESSAGE("Deleting project: %s", filename.c_str());

    Tools::RemoveFile(filename);
}


