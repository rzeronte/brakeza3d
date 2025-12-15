//
// Created by Eduardo on 30/01/2025.
//

#include "../../include/Components/ComponentScripting.h"
#include "../../include/Misc/Logging.h"
#include "../../include/Brakeza.h"
#include "../../include/LUA/BrakezaLuaBridge.h"
#include "../../include/Render/JSONSerializerRegistry.h"

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

sol::object ComponentScripting::getGlobalScriptVar(const std::string& scriptName, const char *varName)
{
    for (auto &s : projectScripts) {
        if (s->getScriptFilename() == scriptName) continue;
        sol::state &lua = Components::get()->Scripting()->getLua();
        return lua[varName];
    }

    return sol::nil;
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

void ComponentScripting::addSceneLUAScript(ScriptLUA *script)
{
    if (script == nullptr) {
        delete script;
    }
    scripts.push_back(script);
    ReloadScriptGlobals();
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

void ComponentScripting::ReloadScriptGlobals() const
{
    for (auto &script : projectScripts) {
        script->reloadGlobals();
    }

    for (auto &script : scripts) {
        script->reloadGlobals();
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

void ComponentScripting::createScriptLUAFile(const std::string& path)
{
    Logging::Message("Creating new Script LUA file: %s", path.c_str());

    std::string projectJsonFile = std::string(path + ".lua");

    std::string content = R"(
        function onStart()
        end
        function onUpdate()
        end
        function onCollision(with)
        end
    )";

    Tools::WriteToFile(projectJsonFile, content.c_str());
}

void ComponentScripting::RemoveScriptLUAFile(const std::string& path)
{
    Logging::Message("[Scripting] Deleting script file: %s", path.c_str());
    Tools::RemoveFile(path);
}

void ComponentScripting::InitLUATypes()
{
    Logging::Message("[Scripting] Init LUA Global types");

    lua.open_libraries(sol::lib::base, sol::lib::math);

    LUAIntegration(lua);

    lua["brakeza"] = Brakeza::get();
    lua["componentsManager"] = Components::get();
    lua.set_function("print",
        static_cast<void(*)(const char*, ...)>(&Logging::Message)
    );
}
