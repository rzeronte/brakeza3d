#pragma once

class GUIAddonProjectSetup {
public:
    // Función principal
    static void DrawWinProjectSettings();
    
    static void DrawProjectSettingsNode();
    static void DrawProjectScriptsNode();
    static void DrawProjectScenesNode();
    static void DrawSceneScriptsNode();
    static void DrawSceneShadersNode();
    
    static void DrawProjectSettings();
    static void DrawProjectScripts();
    static void DrawProjectScenes();
    static void DrawSceneScripts();
    static void DrawSceneCustomShaders();
};