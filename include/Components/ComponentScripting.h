//
// Created by Eduardo on 30/01/2025.
//

#ifndef BRAKEZA3D_COMPONENTSCRIPTING_H
#define BRAKEZA3D_COMPONENTSCRIPTING_H


#include "Component.h"
#include "../Loaders/Project.h"
#include "../Loaders/Scene.h"
#include "../Misc/FilePaths.h"
#include <unordered_map>

class ComponentScripting : public Component
{
    Config::LuaStateScripts stateScripts = Config::LuaStateScripts::LUA_STOP;
    std::vector<ScriptLUA*> sceneScripts;
    std::vector<ScriptLUA*> projectScripts;
    std::vector<std::string> projectScenes;
    std::unordered_map<std::string, ScriptLUA*> scriptsByName;

    std::vector<Scene*> loadedScenes;
    Project *currentProject = nullptr;

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
    void ReloadLUAScripts();
    void AddSceneLUAScript(ScriptLUA *script);
    void AddProjectScene(const std::string &);
    void AddProjectLUAScript(ScriptLUA *script);
    void ReloadScriptGlobals();
    void RunProjectScriptsOnStart();
    void RemoveSceneScript(ScriptLUA *script);
    void RemoveProjectScene(const std::string &scenePath);
    void RemoveProjectScript(ScriptLUA *script);
    void onStartScripts() const;
    void RunScripts() const;
    void RunPostUpdateScripts() const;
    void InitLUATypes();

    bool hasProjectScene(const std::string &filePath);

    sol::object getGlobalScriptVar(const std::string& scriptName, const char *varName);
    void setGlobalScriptVar(const std::string& scriptName, const char *varName, sol::object value);

    sol::state &getLua()                                                    { return lua; }
    std::vector<ScriptLUA*> &getSceneScripts()                              { return sceneScripts; }
    std::vector<std::string> &getProjectScenes()                            { return projectScenes; }
    std::vector<ScriptLUA*> &getProjectScripts()                            { return projectScripts; }
    [[nodiscard]] bool isExecuting() const                                  { return getStateLUAScripts() == Config::LUA_PLAY; }
    [[nodiscard]] Config::LuaStateScripts getStateLUAScripts() const        { return stateScripts; }
    [[nodiscard]] Scene *getCurrentScene() const                            { return loadedScenes.empty() ? nullptr : loadedScenes.back(); }
    [[nodiscard]] std::vector<Scene*>& getLoadedScenes()                    { return loadedScenes; }
    [[nodiscard]] Project *getCurrentProject() const                        { return currentProject; }
    Scene *getSceneByName(const std::string &name) const;
    void addScene(Scene *scene);
    void removeScene(const std::string &name);

    static void CreateScriptLUAFile(const FilePath::ScriptFile& path, ScriptType scriptType = SCRIPT_GLOBAL);
    static cJSON *CreateEmptyTypesFileJSON(const std::string &name, const FilePath::ScriptFile &codeFile, const FilePath::TypesFile &typesFile, ScriptType scriptType = SCRIPT_GLOBAL);
    static void RemoveScriptLUAFile(const FilePath::ScriptFile& path);

    sol::table loadJSON(const std::string &path);
    void saveJSON(const std::string &path, sol::table data);

    void rebuildScriptIndex();
    void MoveSceneScriptUp(ScriptLUA* script);
    void MoveSceneScriptDown(ScriptLUA* script);

    void MoveProjectScriptUp(ScriptLUA* script);
    void MoveProjectScriptDown(ScriptLUA* script);

    void setCurrentScene(Scene *current_scene);
    void setCurrentProject(Project *current_project);
};


#endif //BRAKEZA3D_COMPONENTSCRIPTING_H
