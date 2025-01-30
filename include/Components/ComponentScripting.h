//
// Created by Eduardo on 30/01/2025.
//

#ifndef BRAKEZA3D_COMPONENTSCRIPTING_H
#define BRAKEZA3D_COMPONENTSCRIPTING_H


#include "Component.h"

class ComponentScripting : public Component {
private:
    EngineSetup::LuaStateScripts stateScripts;
    std::vector<ScriptLUA*> scripts;
    std::vector<ScriptLUA*> projectScripts;
    sol::state lua;
public:
    ComponentScripting();

    sol::state &getLua();

    void onStart() override;

    void preUpdate() override;

    void onUpdate() override;

    void postUpdate() override;

    void onEnd() override;

    void onSDLPollEvent(SDL_Event *event, bool &finish) override;

    EngineSetup::LuaStateScripts getStateLUAScripts();

    void playLUAScripts();

    void stopLUAScripts();

    void reloadLUAScripts();

    std::vector<ScriptLUA*> &getSceneLUAScripts();

    std::vector<ScriptLUA*> &getProjectLUAScripts();

    void addSceneLUAScript(ScriptLUA *script);

    void addProjectLUAScript(ScriptLUA *script);

    sol::object getGlobalScriptVar(const std::string& scriptName, const char *varName);

    void reloadScriptGlobals();

    void removeSceneScript(ScriptLUA *script);

    void removeProjectScript(ScriptLUA *script);

    void onStartScripts();

    void runScripts();

    static void createScriptLUAFile(const std::string& path);

    static void removeScriptLUAFile(const std::string& path);

    void initLUATypes();
};


#endif //BRAKEZA3D_COMPONENTSCRIPTING_H
