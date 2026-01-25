#pragma once

class GUIAddonProjectSetup {
public:
    // Función principal
    static void DrawWinProjectSettings();
    
    // Nodos principales con CustomTreeNode
    static void DrawProjectSettingsNode();
    static void DrawProjectScriptsNode();
    static void DrawProjectScenesNode();
    static void DrawSceneScriptsNode();
    static void DrawSceneShadersNode();
    
    // Funciones de renderizado de contenido (sin cambios)
    static void DrawProjectSettings();
    static void DrawProjectScripts();
    static void DrawProjectScenes();
    static void DrawSceneScripts();
    static void DrawSceneCustomShaders();
    
    // Funciones legacy (mantener compatibilidad)
    static void TreeProjectSettings();
    static void TreeProjectScripts();
    static void TreeProjectScenes();
    static void TreeSceneScripts();
    static void TreeSceneShaders();
    static void SaveCurrentSceneButton();
    static void SaveCurrentProjectButton();
};