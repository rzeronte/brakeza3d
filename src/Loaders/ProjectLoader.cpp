//
// Created by Eduardo on 28/01/2025.
//

#include "../../include/Loaders/ProjectLoader.h"
#include "../../include/Misc/Tools.h"
#include "../../include/Misc/Logging.h"
#include "../../include/Components/Components.h"


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

    if (cJSON_GetObjectItemCaseSensitive(contentJSON, "scripts") != nullptr) {
        cJSON *currentScript;
        cJSON_ArrayForEach(currentScript, cJSON_GetObjectItemCaseSensitive(contentJSON, "scripts")) {
            std::string name = cJSON_GetObjectItemCaseSensitive(currentScript, "name")->valuestring;
            std::string codeScript = cJSON_GetObjectItemCaseSensitive(currentScript, "codeFile")->valuestring;
            std::string typesFile = cJSON_GetObjectItemCaseSensitive(currentScript, "typesFile")->valuestring;
            Components::get()->Scripting()->AddProjectLUAScript(new ScriptLUA(name, codeScript, typesFile));
        }
    }
}

void ProjectLoader::SaveProject(const std::string &filename)
{
    cJSON *root = cJSON_CreateObject();

    cJSON_AddStringToObject(root, "name", Config::get()->ENGINE_TITLE.c_str());

    cJSON *sceneScriptsArray = cJSON_CreateArray();
    for (auto script : Components::get()->Scripting()->getProjectLUAScripts()) {
        cJSON *scriptSceneSON = cJSON_CreateObject();
        cJSON_AddStringToObject(scriptSceneSON, "name", script->getScriptFilename().c_str());
        cJSON_AddItemToArray(sceneScriptsArray, scriptSceneSON);
    }
    cJSON_AddItemToObject(root, "scripts", sceneScriptsArray);

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
