#pragma once

class GUIAddonProjectSetup {
public:
    static void DrawWinProjectSettings();

    static void DrawProjectSettingsNode();
    static void DrawProjectScriptsNode();
    static void DrawProjectScenesNode();
    static void DrawSceneScriptsNode();
    static void DrawSceneShadersNode();
    static void DrawSceneRenderSettingsNode();

    static void DrawProjectSettings();
    static void DrawProjectScripts();
    static void DrawProjectScenes();
    static void DrawSceneScripts();
    static void DrawSceneCustomShaders();

    // Project edit popup
    static void DrawProjectEditPopup();
    static bool openPopupEditProject;

    // Additive scene load popup
    static void DrawAdditiveScenePopup();
    static bool openPopupAdditiveScene;
    static bool additiveLoadScripts;
    static bool additiveLoadShaders;
    static bool additiveLoadCamera;
    static bool additiveLoadRenderSettings;
};