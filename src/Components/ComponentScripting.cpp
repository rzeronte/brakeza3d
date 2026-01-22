//
// Created by Eduardo on 30/01/2025.
//

#include "../../include/Components/ComponentScripting.h"
#include "../../include/Misc/Logging.h"
#include "../../include/Brakeza.h"
#include "../../include/GUI/Objects/ScriptLuaGUI.h"
#include "../../include/LUA/BrakezaLuaBridge.h"

void ComponentScripting::onStart()
{
    Component::onStart();
    InitLUATypes();
}

void ComponentScripting::preUpdate()
{
    Component::preUpdate();
}

void ComponentScripting::onUpdate()
{
    if (!isEnabled()) return;

    if (stateScripts == Config::LUA_PLAY) {
        RunScripts();
    }
}

void ComponentScripting::postUpdate()
{
    Component::postUpdate();
}

void ComponentScripting::onEnd()
{
}

void ComponentScripting::onSDLPollEvent(SDL_Event *event, bool &finish)
{
}

void ComponentScripting::PlayLUAScripts()
{
    LOG_MESSAGE("[Scripting] Start scripts execution...");

    Components::get()->Camera()->setEnabled(false);
    Components::get()->Scripting()->setEnabled(true);
    Components::get()->Input()->setEnabled(false);
    Components::get()->Render()->setSelectedObject(nullptr);
    Components::get()->Collisions()->setEnabled(true);

    onStartScripts();

    stateScripts = Config::LuaStateScripts::LUA_PLAY;
}

void ComponentScripting::StopLUAScripts()
{
    LOG_MESSAGE("LUA Scripts state changed to STOP");

    Components::get()->Camera()->setEnabled(true);
    Components::get()->Scripting()->setEnabled(false);
    Components::get()->Input()->setEnabled(true);
    Components::get()->Collisions()->setEnabled(false);
    Components::get()->Window()->setImGuiMouse();

    stateScripts = Config::LuaStateScripts::LUA_STOP;
}

void ComponentScripting::ReloadLUAScripts() const
{
    LOG_MESSAGE("Reloading LUA Scripts...");

    auto &lua = Components::get()->Scripting()->getLua();
    lua.collect_garbage();

    for (auto &s : projectScripts) {
        s->ReloadScriptCode();
    }

    for (auto &s : sceneScripts) {
        s->ReloadScriptCode();
    }

    auto &sceneObjects = Brakeza::get()->getSceneObjects();
    for (auto &object : sceneObjects) {
        object->ReloadScriptsCode();
        object->ReloadScriptsEnvironment();
    }

    ReloadScriptGlobals();
}

void ComponentScripting::AddProjectScene(const std::string &value)
{
    if (std::find(projectScenes.begin(), projectScenes.end(), value) == projectScenes.end()) {
        projectScenes.push_back(value);
    }
}

void ComponentScripting::AddSceneLUAScript(ScriptLUA *script)
{
    if (script->getScriptFilename().empty()) {
        delete script;
        LOG_ERROR("[Scripting] Script refused...Deleting!");
        return;
    }

    sceneScripts.push_back(script);
    ReloadScriptGlobals();
}

void ComponentScripting::AddProjectLUAScript(ScriptLUA *script)
{
    if (script->getScriptFilename().empty()) {
        delete script;
        LOG_ERROR("[Scripting] Script refused...Deleting!");
        return;
    }

    projectScripts.push_back(script);
    ReloadScriptGlobals();
}

void ComponentScripting::ReloadScriptGlobals() const
{
    for (auto &script : projectScripts) {
        script->ReloadGlobals();
    }

    for (auto &script : sceneScripts) {
        script->ReloadGlobals();
    }
}

void ComponentScripting::RemoveSceneScript(ScriptLUA *script)
{
    LOG_MESSAGE("[Scripting] Removing SceneScript '%s'...", script->scriptFilename.c_str());

    for (auto it = sceneScripts.begin(); it != sceneScripts.end(); ++it) {
        if ((*it)->scriptFilename == script->scriptFilename) {
            delete *it;
            sceneScripts.erase(it);
            return;
        }
    }
}

void ComponentScripting::RemoveProjectScene(const std::string &scenePath)
{
    LOG_MESSAGE("[Scripting] Removing project scene %s", scenePath.c_str());

    for (auto it = projectScenes.begin(); it != projectScenes.end(); ++it) {
        if (*it == scenePath) {
            projectScenes.erase(it);
            return;
        }
    }}

void ComponentScripting::RemoveProjectScript(ScriptLUA *script)
{
    LOG_MESSAGE("Removing PROJECT script %s", script->scriptFilename.c_str());

    for (auto it = projectScripts.begin(); it != projectScripts.end(); ++it) {
        if ((*it)->scriptFilename == script->scriptFilename) {
            delete *it;
            projectScripts.erase(it);
            return;
        }
    }
}

void ComponentScripting::onStartScripts() const
{
    LOG_MESSAGE("Executing OnStart for PROJECT scripts...");
    for (auto script : projectScripts) {
        script->RunGlobal("onStart");
    }

    LOG_MESSAGE("Executing OnStart for SCENE scripts...");
    for (auto script : sceneScripts) {
        script->RunGlobal("onStart");
    }

    LOG_MESSAGE("Executing OnStart for OBJECT scripts...");
    auto &sceneObjects = Brakeza::get()->getSceneObjects();
    for (auto object : sceneObjects) {
        object->RunStartScripts();
    }
}

void ComponentScripting::RunScripts() const
{
    for (auto &script : projectScripts) {
        script->RunGlobal("onUpdate");
    }

    for (auto &script : sceneScripts) {
        script->RunGlobal("onUpdate");
    }
}

void ComponentScripting::InitLUATypes()
{
    LOG_MESSAGE("[Scripting] Init LUA Global types");

    lua.open_libraries(sol::lib::base, sol::lib::math);

    LUAIntegration(lua);

    lua["Brakeza"] = Brakeza::get();
    lua["Components"] = Components::get();
    lua.set_function("print", static_cast<void(*)(const char*, ...)>(&Logging::Message));
}

sol::object ComponentScripting::getGlobalScriptVar(const std::string& scriptName, const char *varName) const
{
    for (auto &s : projectScripts) {
        if (s->getScriptFilename() == scriptName) continue;
        sol::state &lua = Components::get()->Scripting()->getLua();
        return lua[varName];
    }

    return sol::nil;
}

void ComponentScripting::CreateScriptLUAFile(const std::string& path)
{
    LOG_MESSAGE("[Scripting] Creating new Script in '%s'...", path.c_str());

    auto codeFile = std::string(path + ".lua");
    auto typesFile = std::string(path + ".json");
    auto name = Tools::getFilenameWithoutExtension(codeFile);

    auto content = Tools::ReadFile(Config::get()->TEMPLATE_SCRIPT);
    Tools::WriteToFile(codeFile, content);

    auto typesFileJson = CreateEmptyTypesFileJSON(name, codeFile, typesFile);
    Tools::WriteToFile(typesFile, cJSON_Print(typesFileJson));

    delete[] content;
}

cJSON* ComponentScripting::CreateEmptyTypesFileJSON(const std::string &name, const std::string &codeFile, const std::string &typesFile)
{
    cJSON* root = cJSON_CreateObject();

    cJSON_AddStringToObject(root, "codeFile", codeFile.c_str());
    cJSON_AddStringToObject(root, "typesFile", typesFile.c_str());
    cJSON_AddStringToObject(root, "name", name.c_str());

    cJSON_AddArrayToObject(root, "types");

    return root;
}

void ComponentScripting::RemoveScriptLUAFile(const std::string& path)
{
    auto meta = ScriptLuaGUI::ExtractScriptMetainfo(path);

    Tools::RemoveFile(meta.codeFile);
    Tools::RemoveFile(meta.typesFile);
}

void ComponentScripting::setCurrentScene(Scene *value)
{
    delete currentScene;
    currentScene = value;
}

void ComponentScripting::setCurrentProject(Project *value)
{
    delete currentProject;
    currentProject = value;
}