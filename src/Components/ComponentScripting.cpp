//
// Created by Eduardo on 30/01/2025.
//

#include "../../include/Components/ComponentScripting.h"
#include "../../include/Misc/Logging.h"
#include "../../include/Brakeza.h"
#include "../../include/Render/Profiler.h"
#include "../../include/GUI/Objects/ScriptLuaGUI.h"
#include "../../include/LUA/BrakezaLuaBridge.h"
#include "../../include/LUA/ObjectFactory.h"

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
    if (stateScripts == Config::LUA_PLAY) {
        RunPostUpdateScripts();
    }
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
    Components::get()->Input()->setMouseEnabled(true);
    Components::get()->Input()->setKeyboardEnabled(true);
    Components::get()->Collisions()->setEnabled(false);
    Components::get()->Window()->setImGuiMouse();

    for (auto &script : projectScripts) {
        script->setGlobalLoaded(false);
    }
    for (auto &script : sceneScripts) {
        script->setGlobalLoaded(false);
    }

    stateScripts = Config::LuaStateScripts::LUA_STOP;
}

void ComponentScripting::ReloadLUAScripts()
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

void ComponentScripting::ReloadScriptEnvironment(const std::string &name)
{
    auto it = scriptsByName.find(name);
    if (it == scriptsByName.end()) {
        LOG_ERROR("[Scripting] ReloadScriptEnvironment: script '%s' not found", name.c_str());
        return;
    }

    ScriptLUA *script = it->second;
    LOG_MESSAGE("[Scripting] ReloadScriptEnvironment: reloading '%s'", name.c_str());

    script->RunGlobal("onEnd");
    script->ReloadScriptCode();
    script->setGlobalLoaded(false);
    script->ReloadGlobals();
    script->RunGlobal("onStart");
}

void ComponentScripting::AddProjectScene(const std::string &value)
{
    LOG_MESSAGE("[Scripting] Adding scene '%s' to Project...", value.c_str());

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
    rebuildScriptIndex();
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
    rebuildScriptIndex();
    ReloadScriptGlobals();
}

void ComponentScripting::ReloadScriptGlobals()
{
    for (auto &script : projectScripts) {
        script->ReloadGlobals();
    }

    for (auto &script : sceneScripts) {
        script->ReloadGlobals();
    }
}

void ComponentScripting::RunProjectScriptsOnStart()
{
    for (auto &script : projectScripts) {
        script->RunGlobal("onStart");
    }
}

void ComponentScripting::RemoveSceneScript(ScriptLUA *script)
{
    LOG_MESSAGE("[Scripting] Removing SceneScript '%s'...", script->scriptFilename.c_str());

    for (auto it = sceneScripts.begin(); it != sceneScripts.end(); ++it) {
        if ((*it)->scriptFilename == script->scriptFilename) {
            delete *it;
            sceneScripts.erase(it);
            rebuildScriptIndex();
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
            rebuildScriptIndex();
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
    auto* profiler   = Profiler::get();
    bool  profileOn  = profiler->isScriptDetailEnabled();
    auto& sm         = profiler->getScriptMeasures();

    for (auto &script : projectScripts) {
        const std::string key = "[G] " + script->getName() + "_update";
        if (profileOn) Profiler::StartMeasure(sm, key);
        script->RunGlobal("onUpdate");
        if (profileOn) Profiler::EndMeasure(sm, key);
    }

    for (auto &script : sceneScripts) {
        const std::string key = "[S] " + script->getName() + "_update";
        if (profileOn) Profiler::StartMeasure(sm, key);
        script->RunGlobal("onUpdate");
        if (profileOn) Profiler::EndMeasure(sm, key);
    }
}

void ComponentScripting::RunPostUpdateScripts() const
{
    auto* profiler   = Profiler::get();
    bool  profileOn  = profiler->isScriptDetailEnabled();
    auto& sm         = profiler->getScriptMeasures();

    for (auto &script : projectScripts) {
        const std::string key = "[G] " + script->getName() + "_post";
        if (profileOn) Profiler::StartMeasure(sm, key);
        script->RunGlobal("postUpdate");
        if (profileOn) Profiler::EndMeasure(sm, key);
    }

    for (auto &script : sceneScripts) {
        const std::string key = "[S] " + script->getName() + "_post";
        if (profileOn) Profiler::StartMeasure(sm, key);
        script->RunGlobal("postUpdate");
        if (profileOn) Profiler::EndMeasure(sm, key);
    }
}

void ComponentScripting::InitLUATypes()
{
    LOG_MESSAGE("[Scripting] Init LUA Global types");

    lua.open_libraries(sol::lib::base, sol::lib::math, sol::lib::table, sol::lib::string, sol::lib::package, sol::lib::io);
    lua["package"]["path"] = "../assets/scripts/?.lua;../assets/scripts/?/init.lua";

    LUAIntegration(lua);

    lua["Brakeza"] = Brakeza::get();
    lua["Components"] = Components::get();
    lua.set_function("print", static_cast<void(*)(const char*, ...)>(&Logging::Message));
}

void ComponentScripting::rebuildScriptIndex()
{
    scriptsByName.clear();
    for (auto &s : sceneScripts) {
        scriptsByName[s->getName()] = s;
    }
    // project scripts take priority over scene scripts on same name
    for (auto &s : projectScripts) {
        scriptsByName[s->getName()] = s;
    }
}

sol::object ComponentScripting::getGlobalScriptVar(const std::string& scriptName, const char *varName)
{
    auto it = scriptsByName.find(scriptName);
    if (it != scriptsByName.end()) {
        return it->second->getGlobalEnvironment()[varName];
    }
    return sol::nil;
}

void ComponentScripting::setGlobalScriptVar(const std::string& scriptName, const char *varName, sol::object value)
{
    auto it = scriptsByName.find(scriptName);
    if (it != scriptsByName.end()) {
        it->second->getGlobalEnvironment()[varName] = value;
    }
}

void ComponentScripting::CreateScriptLUAFile(const FilePath::ScriptFile& path, ScriptType scriptType)
{
    LOG_MESSAGE("[Scripting] Creating new Script in '%s'...", path.c_str());

    FilePath::ScriptFile codeFile(path.str() + ".lua");
    FilePath::TypesFile typesFile(path.str() + ".json");
    auto name = Tools::getFilenameWithoutExtension(codeFile);

    auto content = Tools::ReadFile(Config::get()->TEMPLATE_SCRIPT);
    Tools::WriteToFile(codeFile, content);

    auto typesFileJson = CreateEmptyTypesFileJSON(name, codeFile, typesFile, scriptType);
    Tools::WriteToFile(typesFile, cJSON_Print(typesFileJson));

    delete[] content;
}

static sol::table cjsonToLua(sol::state &lua, cJSON *node)
{
    sol::table t = lua.create_table();
    cJSON *child = node->child;
    int arrayIdx = 1;
    while (child) {
        sol::object val;
        if (cJSON_IsObject(child) || cJSON_IsArray(child)) {
            val = cjsonToLua(lua, child);
        } else if (cJSON_IsString(child)) {
            val = sol::make_object(lua, std::string(child->valuestring));
        } else if (cJSON_IsBool(child)) {
            val = sol::make_object(lua, (bool)child->valueint);
        } else if (cJSON_IsNumber(child)) {
            val = sol::make_object(lua, child->valuedouble);
        } else {
            val = sol::make_object(lua, sol::nil);
        }

        if (cJSON_IsArray(node)) {
            t[arrayIdx++] = val;
        } else {
            t[child->string] = val;
        }
        child = child->next;
    }
    return t;
}

static cJSON* luaTableToJSON(const sol::table& t) {
    cJSON* obj = cJSON_CreateObject();
    for (auto& pair : t) {
        sol::object key = pair.first;
        sol::object val = pair.second;
        if (key.get_type() != sol::type::string) continue;
        std::string k = key.as<std::string>();
        switch (val.get_type()) {
            case sol::type::boolean:
                cJSON_AddBoolToObject(obj, k.c_str(), val.as<bool>());
                break;
            case sol::type::number:
                cJSON_AddNumberToObject(obj, k.c_str(), val.as<double>());
                break;
            case sol::type::string:
                cJSON_AddStringToObject(obj, k.c_str(), val.as<std::string>().c_str());
                break;
            case sol::type::table:
                cJSON_AddItemToObject(obj, k.c_str(), luaTableToJSON(val.as<sol::table>()));
                break;
            default:
                break;
        }
    }
    return obj;
}

void ComponentScripting::saveJSON(const std::string& path, sol::table data) {
    cJSON* root = luaTableToJSON(data);
    char* str = cJSON_Print(root);
    if (str) {
        Tools::WriteToFile(path, str);
        free(str);
    }
    cJSON_Delete(root);
    LOG_MESSAGE("[ComponentScripting] saveJSON: wrote %s", path.c_str());
}

sol::table ComponentScripting::loadJSON(const std::string &path)
{
    auto *raw = Tools::ReadFile(path.c_str());
    if (!raw) {
        LOG_ERROR("[ComponentScripting] loadJSON: cannot read %s",  path.c_str());
        return lua.create_table();
    }
    cJSON *root = cJSON_Parse(raw);
    delete[] raw;
    if (!root) {
        LOG_ERROR("[ComponentScripting] loadJSON: parse error in ", path.c_str());
        return lua.create_table();
    }
    sol::table result = cjsonToLua(lua, root);
    cJSON_Delete(root);
    return result;
}

cJSON* ComponentScripting::CreateEmptyTypesFileJSON(const std::string &name, const FilePath::ScriptFile &codeFile, const FilePath::TypesFile &typesFile, ScriptType scriptType)
{
    cJSON* root = cJSON_CreateObject();

    cJSON_AddStringToObject(root, "codeFile", codeFile.c_str());
    cJSON_AddStringToObject(root, "typesFile", typesFile.c_str());
    cJSON_AddStringToObject(root, "name", name.c_str());
    cJSON_AddStringToObject(root, "type", scriptType == SCRIPT_GLOBAL ? "Global" : "Object");

    cJSON_AddArrayToObject(root, "types");

    return root;
}

void ComponentScripting::RemoveScriptLUAFile(const FilePath::ScriptFile& path)
{
    auto meta = ScriptLuaGUI::ExtractScriptMetainfo(path);

    Tools::RemoveFile(meta.codeFile);
    Tools::RemoveFile(meta.typesFile);
}

void ComponentScripting::MoveSceneScriptUp(ScriptLUA* script)
{
    if (!script || sceneScripts.size() < 2) return;
    auto it = std::find(sceneScripts.begin(), sceneScripts.end(), script);
    if (it == sceneScripts.end() || it == sceneScripts.begin()) return;     // No encontrado o ya está arrib
    std::iter_swap(it, it - 1);                                         // Intercambia con el anterior
}

void ComponentScripting::MoveSceneScriptDown(ScriptLUA* script)
{
    if (!script || sceneScripts.size() < 2) return;
    auto it = std::find(sceneScripts.begin(), sceneScripts.end(), script);
    if (it == sceneScripts.end() || it == sceneScripts.end() - 1) return;   // No encontrado o ya está abajo
    std::iter_swap(it, it + 1);                                         // Intercambia con el siguiente
}

void ComponentScripting::MoveProjectScriptUp(ScriptLUA* script)
{
    if (!script || projectScripts.size() < 2) return;
    auto it = std::find(projectScripts.begin(), projectScripts.end(), script);
    if (it == projectScripts.end() || it == projectScripts.begin()) return;     // No encontrado o ya está arrib
    std::iter_swap(it, it - 1);                                         // Intercambia con el anterior
}

void ComponentScripting::MoveProjectScriptDown(ScriptLUA* script)
{
    if (!script || projectScripts.size() < 2) return;
    auto it = std::find(projectScripts.begin(), projectScripts.end(), script);
    if (it == projectScripts.end() || it == projectScripts.end() - 1) return;   // No encontrado o ya está abajo
    std::iter_swap(it, it + 1);                                         // Intercambia con el siguiente
}

bool ComponentScripting::hasProjectScene(const std::string& filePath)
{
    return std::find(projectScenes.begin(), projectScenes.end(), filePath) != projectScenes.end();
}

void ComponentScripting::setCurrentScene(Scene *value)
{
    for (auto *s : loadedScenes) delete s;
    loadedScenes.clear();
    if (value != nullptr)
        loadedScenes.push_back(value);
}

void ComponentScripting::addScene(Scene *scene)
{
    loadedScenes.push_back(scene);
}

void ComponentScripting::removeScene(const std::string &name)
{
    for (auto it = loadedScenes.begin(); it != loadedScenes.end(); ++it) {
        if ((*it)->getName() == name) {
            delete *it;
            loadedScenes.erase(it);
            return;
        }
    }
}

Scene *ComponentScripting::getSceneByName(const std::string &name) const
{
    for (auto *s : loadedScenes) {
        if (s->getName() == name) return s;
    }
    return nullptr;
}

void ComponentScripting::setCurrentProject(Project *value)
{
    delete currentProject;
    currentProject = value;
}