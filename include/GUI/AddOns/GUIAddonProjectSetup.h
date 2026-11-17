/*
 * Brakeza3D - 3D/2D game engine
 * Author:  Eduardo Rodríguez Álvarez
 * GitHub:  https://github.com/rzeronte/brakeza3d
 * Web:     https://brakeza.com
 * Started: 2018-09-30
 * Copyright (C) 2018-2026 Eduardo Rodríguez Álvarez
 * License: GNU General Public License v3.0 (see LICENSE)
 */
#pragma once

#include <string>
#include "../../Loaders/SceneLoader.h"

class GUIAddonProjectSetup {
public:
    // Search filters for the script/shader lists (case-insensitive substring)
    static char projectScriptsFilter[128];
    static char sceneScriptsFilter[128];
    static char sceneShadersFilter[128];
    static char projectAssetsFilter[128];
    static void DrawFilterBar(const char *id, char *buffer, size_t size);
    static bool MatchesFilter(const std::string &text, const char *filter);

    static void DrawWinProjectSettings();

    static void DrawProjectSettingsNode();
    static void DrawProjectScriptsNode();
    static void DrawProjectScenesNode();
    // File browser over the project's own assets folder (AssetPaths::GetProjectRoot()): scenes, scripts, shaders, widgets
    static void DrawProjectAssetsNode();
    static void DrawProjectAssetsFiles(const std::string &root);
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

    // Special save popup: choose which parts of a scene file are overwritten
    static void DrawSpecialSavePopup();
    static bool openPopupSpecialSave;
    static SceneLoader::SaveParts specialSaveParts;

    // Additive scene load popup
    static void DrawAdditiveScenePopup();
    static bool openPopupAdditiveScene;
    static bool additiveLoadScripts;
    static bool additiveLoadShaders;
    static bool additiveLoadCamera;
    static bool additiveLoadRenderSettings;
};