//
// Created by Eduardo on 28/01/2025.
//

#include "../../include/Misc/ProjectLoader.h"
#include "../../include/Misc/Tools.h"
#include "../../include/Render/Logging.h"
#include "../../include/ComponentsManager.h"

ProjectLoader::ProjectLoader() {

}

void ProjectLoader::loadProject(const std::string &filename)
{
    ProjectLoader::removeProjectScripts();

    size_t file_size;
    auto contentFile = Tools::readFile(filename, file_size);
    auto contentJSON = cJSON_Parse(contentFile);

    Logging::Message("Loading PROJECT: %s", filename.c_str());

    if (cJSON_GetObjectItemCaseSensitive(contentJSON, "name") != nullptr) {
        auto sceneName = cJSON_GetObjectItemCaseSensitive(contentJSON, "name")->valuestring;
        ComponentsManager::get()->getComponentWindow()->setWindowTitle(sceneName);
        EngineSetup::get()->ENGINE_TITLE = sceneName;
    }

    if (cJSON_GetObjectItemCaseSensitive(contentJSON, "scripts") != nullptr) {
        cJSON *currentScript;
        cJSON_ArrayForEach(currentScript, cJSON_GetObjectItemCaseSensitive(contentJSON, "scripts")) {
            std::string fileName = (const char*) cJSON_GetObjectItemCaseSensitive(currentScript, "name")->valuestring;
            ComponentsManager::get()->getComponentRender()->addProjectLUAScript(
                new ScriptLUA(fileName, ScriptLUA::dataTypesFileFor(fileName))
            );
        }
    }
}

void ProjectLoader::saveProject(const std::string &filename)
{
    cJSON *root = cJSON_CreateObject();

    cJSON_AddStringToObject(root, "name", EngineSetup::get()->ENGINE_TITLE.c_str());

    cJSON *sceneScriptsArray = cJSON_CreateArray();
    for (auto script : ComponentsManager::get()->getComponentRender()->getProjectLUAScripts()) {
        cJSON *scriptSceneSON = cJSON_CreateObject();
        cJSON_AddStringToObject(scriptSceneSON, "name", script->getScriptFilename().c_str());
        cJSON_AddItemToArray(sceneScriptsArray, scriptSceneSON);
    }
    cJSON_AddItemToObject(root, "scripts", sceneScriptsArray);

    Logging::Message(cJSON_Print(root));
    Tools::writeToFile(EngineSetup::get()->PROJECTS_FOLDER + filename, cJSON_Print(root));
}

void ProjectLoader::removeProjectScripts()
{
    auto render = ComponentsManager::get()->getComponentRender();
    for (auto o: render->getProjectLUAScripts()) {
        render->removeProjectScript(o);
    }
}
