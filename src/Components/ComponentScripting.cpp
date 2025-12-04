//
// Created by Eduardo on 30/01/2025.
//

#include "../../include/Components/ComponentScripting.h"
#include "../../include/Misc/Logging.h"
#include "../../include/Brakeza.h"
#include "../../include/BrakezaLuaBridge.h"
#include "../../include/Serializers/JSONSerializerRegistry.h"

ComponentScripting::ComponentScripting()
{
}

void ComponentScripting::onStart()
{
    Component::onStart();
}

void ComponentScripting::preUpdate()
{
    Component::preUpdate();
}

void ComponentScripting::onUpdate()
{
    Component::onUpdate();

    if (stateScripts == BrakezaSetup::LUA_PLAY) {
        runScripts();
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

BrakezaSetup::LuaStateScripts ComponentScripting::getStateLUAScripts()
{
    return stateScripts;
}

void ComponentScripting::PlayLUAScripts()
{
    Logging::Message("LUA Scripts state changed to PLAY");

    onStartScripts();

    ComponentsManager::get()->getComponentInput()->setEnabled(false);
    ComponentsManager::get()->getComponentRender()->setSelectedObject(nullptr);
    ComponentsManager::get()->getComponentCollisions()->setEnabled(true);

    stateScripts = BrakezaSetup::LuaStateScripts::LUA_PLAY;
}

void ComponentScripting::StopLUAScripts()
{
    Logging::Message("LUA Scripts state changed to STOP");

    ComponentsManager::get()->getComponentInput()->setEnabled(true);
    ComponentsManager::get()->getComponentCollisions()->setEnabled(false);

    stateScripts = BrakezaSetup::LuaStateScripts::LUA_STOP;
}

void ComponentScripting::ReloadLUAScripts()
{
    Logging::Message("Reloading LUA Scripts...");

    auto &lua = ComponentsManager::get()->getComponentScripting()->getLua();
    lua.collect_garbage();

    for (auto s : projectScripts) {
        s->reloadScriptCode();
    }

    for (auto s : scripts) {
        s->reloadScriptCode();
    }

    auto &sceneObjects = Brakeza::get()->getSceneObjects();
    for (auto object : sceneObjects) {
        object->ReloadScriptsCode();
        object->ReloadScriptsEnvironment();
    }

    reloadScriptGlobals();
}

std::vector<ScriptLUA*> &ComponentScripting::getSceneLUAScripts()
{
    return scripts;
}

std::vector<ScriptLUA*> &ComponentScripting::getProjectLUAScripts()
{
    return projectScripts;
}

sol::object ComponentScripting::getGlobalScriptVar(const std::string& scriptName, const char *varName)
{
    for (auto s : projectScripts) {
        if (s->getScriptFilename() == scriptName) continue;
        sol::state &lua = ComponentsManager::get()->getComponentScripting()->getLua();
        return lua[varName];
    }

    return sol::nil;
}

void ComponentScripting::addProjectLUAScript(ScriptLUA *script)
{
    projectScripts.push_back(script);
    reloadScriptGlobals();
}

void ComponentScripting::addSceneLUAScript(ScriptLUA *script)
{
    scripts.push_back(script);
    reloadScriptGlobals();
}

void ComponentScripting::runScripts()
{
    for (auto script : projectScripts) {
        script->runGlobal("onUpdate");
    }

    for (auto script : scripts) {
        script->runGlobal("onUpdate");
    }
}

void ComponentScripting::reloadScriptGlobals()
{
    for (auto script : projectScripts) {
        script->reloadGlobals();
    }

    for (auto script : scripts) {
        script->reloadGlobals();
    }
}

void ComponentScripting::removeSceneScript(ScriptLUA *script)
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

void ComponentScripting::removeProjectScript(ScriptLUA *script)
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

void ComponentScripting::onStartScripts()
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

void ComponentScripting::removeScriptLUAFile(const std::string& path)
{
    Logging::Message("Deleting script file: %s", path.c_str());
    Tools::RemoveFile(path);
}

void ComponentScripting::InitLUATypes()
{
    Logging::Message("[ComponentScripting] Init LUA Global types");

    lua.open_libraries(sol::lib::base, sol::lib::math);

    LUAIntegration(lua);

    lua["brakeza"] = Brakeza::get();
    lua["componentsManager"] = ComponentsManager::get();
    lua.set_function("print", &Logging::Message);
}

sol::state &ComponentScripting::getLua()
{
    return lua;
}
