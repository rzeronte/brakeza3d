//
// Created by Eduardo on 30/01/2025.
//

#ifndef BRAKEZA3D_COMPONENTSCRIPTING_H
#define BRAKEZA3D_COMPONENTSCRIPTING_H


#include "Component.h"

class ComponentScripting : public Component
{
    BrakezaSetup::LuaStateScripts stateScripts = BrakezaSetup::LuaStateScripts::LUA_STOP;
    std::vector<ScriptLUA*> scripts;
    std::vector<ScriptLUA*> projectScripts;
    sol::state lua;
public:
    ComponentScripting();

    void onStart() override;
    void preUpdate() override;
    void onUpdate() override;
    void postUpdate() override;
    void onEnd() override;
    void onSDLPollEvent(SDL_Event *event, bool &finish) override;
    void playLUAScripts();
    void stopLUAScripts();
    void reloadLUAScripts();
    void addSceneLUAScript(ScriptLUA *script);
    void addProjectLUAScript(ScriptLUA *script);
    void reloadScriptGlobals();
    void removeSceneScript(ScriptLUA *script);
    void removeProjectScript(ScriptLUA *script);
    void onStartScripts();
    void runScripts();
    void initLUATypes();
    sol::state &getLua();
    std::vector<ScriptLUA*> &getSceneLUAScripts();
    std::vector<ScriptLUA*> &getProjectLUAScripts();
    BrakezaSetup::LuaStateScripts getStateLUAScripts();
    sol::object getGlobalScriptVar(const std::string& scriptName, const char *varName);
    static void createScriptLUAFile(const std::string& path);
    static void removeScriptLUAFile(const std::string& path);
};


#endif //BRAKEZA3D_COMPONENTSCRIPTING_H
