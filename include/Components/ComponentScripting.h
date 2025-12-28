//
// Created by Eduardo on 30/01/2025.
//

#ifndef BRAKEZA3D_COMPONENTSCRIPTING_H
#define BRAKEZA3D_COMPONENTSCRIPTING_H


#include "Component.h"

class ComponentScripting : public Component
{
    Config::LuaStateScripts stateScripts = Config::LuaStateScripts::LUA_STOP;
    std::vector<ScriptLUA*> scripts;
    std::vector<ScriptLUA*> projectScripts;
    sol::state lua;
public:
    ComponentScripting() = default;

    void onStart() override;
    void preUpdate() override;
    void onUpdate() override;
    void postUpdate() override;
    void onEnd() override;
    void onSDLPollEvent(SDL_Event *event, bool &finish) override;
    void PlayLUAScripts();
    void StopLUAScripts();
    void ReloadLUAScripts() const;
    void AddSceneLUAScript(ScriptLUA *script);
    void AddProjectLUAScript(ScriptLUA *script);
    void ReloadScriptGlobals() const;
    void RemoveSceneScript(ScriptLUA *script);
    void RemoveProjectScript(ScriptLUA *script);
    void onStartScripts() const;
    void RunScripts() const;
    void InitLUATypes();
    sol::object getGlobalScriptVar(const std::string& scriptName, const char *varName);
    sol::state &getLua()                                                    { return lua; }
    std::vector<ScriptLUA*> &getSceneLUAScripts()                           { return scripts; }
    std::vector<ScriptLUA*> &getProjectLUAScripts()                         { return projectScripts; }
    bool isExecuting() const                                                { return getStateLUAScripts() == Config::LUA_PLAY; }
    [[nodiscard]] Config::LuaStateScripts getStateLUAScripts() const        { return stateScripts; }
    static void CreateScriptLUAFile(const std::string& path);
    static cJSON *CreateEmptyTypesFileJSON(const std::string &name, const std::string &codeFile, const std::string &typesFile);

    static void RemoveScriptLUAFile(const std::string& path);
};


#endif //BRAKEZA3D_COMPONENTSCRIPTING_H
