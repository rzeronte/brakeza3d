#pragma once

#include <string>

class GUIAddonProjectSetup {
public:
    // Search filters for the script/shader lists (case-insensitive substring)
    static char projectScriptsFilter[128];
    static char sceneScriptsFilter[128];
    static char sceneShadersFilter[128];
    static void DrawFilterBar(const char *id, char *buffer, size_t size);
    static bool MatchesFilter(const std::string &text, const char *filter);

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