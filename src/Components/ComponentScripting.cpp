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
    Component::onUpdate();

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
    Logging::Message("LUA Scripts state changed to PLAY");

    onStartScripts();

    Components::get()->Input()->setEnabled(false);
    Components::get()->Render()->setSelectedObject(nullptr);
    Components::get()->Collisions()->setEnabled(true);

    stateScripts = Config::LuaStateScripts::LUA_PLAY;
}

void ComponentScripting::StopLUAScripts()
{
    Logging::Message("LUA Scripts state changed to STOP");

    Components::get()->Input()->setEnabled(true);
    Components::get()->Collisions()->setEnabled(false);

    stateScripts = Config::LuaStateScripts::LUA_STOP;
}

void ComponentScripting::ReloadLUAScripts() const
{
    Logging::Message("Reloading LUA Scripts...");

    auto &lua = Components::get()->Scripting()->getLua();
    lua.collect_garbage();

    for (auto &s : projectScripts) {
        s->reloadScriptCode();
    }

    for (auto &s : scripts) {
        s->reloadScriptCode();
    }

    auto &sceneObjects = Brakeza::get()->getSceneObjects();
    for (auto &object : sceneObjects) {
        object->ReloadScriptsCode();
        object->ReloadScriptsEnvironment();
    }

    ReloadScriptGlobals();
}

void ComponentScripting::addSceneLUAScript(ScriptLUA *script)
{
    if (script->getScriptFilename().empty()) {
        delete script;
        Logging::Error("[Scripting] Script refused...Deleting!");
        return;
    }

    scripts.push_back(script);
    ReloadScriptGlobals();
}

void ComponentScripting::addProjectLUAScript(ScriptLUA *script)
{
    if (script->getScriptFilename().empty()) {
        delete script;
        Logging::Error("[Scripting] Script refused...Deleting!");
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

    for (auto &script : scripts) {
        script->ReloadGlobals();
    }
}

void ComponentScripting::RemoveSceneScript(ScriptLUA *script)
{
    Logging::Message("Removing SCENE script %s", script->scriptFilename.c_str());

    for (auto it = scripts.begin(); it != scripts.end(); ++it) {
        if ((*it)->scriptFilename == script->scriptFilename) {
            delete *it;
            scripts.erase(it);
            return;
        }
    }
}

void ComponentScripting::RemoveProjectScript(ScriptLUA *script)
{
    Logging::Message("Removing PROJECT script %s", script->scriptFilename.c_str());

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
    Logging::Message("Executing OnStart for PROJECT scripts...");
    for (auto script : projectScripts) {
        script->runGlobal("onStart");
    }

    Logging::Message("Executing OnStart for SCENE scripts...");
    for (auto script : scripts) {
        script->runGlobal("onStart");
    }

    Logging::Message("Executing OnStart for OBJECT scripts...");
    auto &sceneObjects = Brakeza::get()->getSceneObjects();
    for (auto object : sceneObjects) {
        object->RunStartScripts();
    }
}

void ComponentScripting::RunScripts() const
{
    for (auto &script : projectScripts) {
        script->runGlobal("onUpdate");
    }

    for (auto &script : scripts) {
        script->runGlobal("onUpdate");
    }
}

void ComponentScripting::InitLUATypes()
{
    Logging::Message("[Scripting] Init LUA Global types");

    lua.open_libraries(sol::lib::base, sol::lib::math);

    LUAIntegration(lua);

    lua["Brakeza"] = Brakeza::get();
    lua["Components"] = Components::get();
    lua.set_function("print", static_cast<void(*)(const char*, ...)>(&Logging::Message)
    );
}

sol::object ComponentScripting::getGlobalScriptVar(const std::string& scriptName, const char *varName)
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
    Logging::Message("[Scripting] Creating new Script in '%s'...", path.c_str());

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
