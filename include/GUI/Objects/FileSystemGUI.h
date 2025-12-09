//
// Created by Eduardo on 26/11/2025.
//

#ifndef BRAKEZA3D_FILESYSTEMGUI_H
#define BRAKEZA3D_FILESYSTEMGUI_H

#include "../GUI.h"
#include "../include/Misc/TexturePackage.h"

class GUIManager;

class FileSystemGUI
{
public:
    // Projects
    static void DrawProjectFiles(GUIManager *gui, GUIType::BrowserCache &browser);
    static void DrawProjectCreator(GUIManager *gui, GUIType::BrowserCache &browser);
    static void DrawProjectsTable(GUIManager *gui, GUIType::BrowserCache &browser);
    static void DrawProjectRow(GUIManager *gui, GUIType::BrowserCache &browser, const std::string &file, int index);
    static void DrawProjectRowActions(GUIManager *gui, GUIType::BrowserCache &browser, const std::string &file);
    static void DrawProjectCreatorDialog(GUIManager *gui, GUIType::BrowserCache &browser, std::string &title);

    // Scenes
    static void DrawSceneFiles(GUIManager *gui, GUIType::BrowserCache &browser);
    static void DrawSceneCreator(GUIManager * gui, GUIType::BrowserCache & browser);
    static void DrawScenesTable(GUIManager * gui, GUIType::BrowserCache & browser);
    static void DrawSceneRow(GUIManager *gui, GUIType::BrowserCache &browser, const std::string &file, int index);
    static void DrawSceneRowActions(GUIManager *gui, GUIType::BrowserCache &browser, const std::string &file);
    static void DrawSceneCreatorDialog(GUIManager *gui, GUIType::BrowserCache &browser, std::string &title);

    // Shader
    static void DrawShaderFiles(GUIManager *gui, GUIType::BrowserCache &browser);
    static void DrawShaderCreator(GUIManager *gui, int &selectedIndex, const std::vector<const char*> &items);
    static void DrawShaderTypeCombo(int &selectedIndex, const std::vector<const char *> &items);
    static void DrawShadersTable(GUIManager *gui, GUIType::BrowserCache &browser);
    static void DrawShaderRow(GUIManager *gui, GUIType::BrowserCache &browser, const std::string &file, int index);
    static void DrawShaderRowActions(GUIManager *gui, GUIType::BrowserCache &browser, const std::string &file);
    static void DrawShaderCreatorDialog(GUIManager *gui, GUIType::BrowserCache &browser, std::string &title);
    static std::vector<const char *> GetShaderTypeItems();

    // Scripts
    static void DrawScriptFiles(GUIManager *gui, GUIType::BrowserCache &browser);
    static void DrawScriptCreator(GUIManager *gui, GUIType::BrowserCache &browser);
    static void DrawScriptsTable(GUIManager *gui, GUIType::BrowserCache &browser);
    static void DrawScriptRow(GUIManager *gui, GUIType::BrowserCache &browser, const std::string &file, int index);
    static void DrawScriptRowActions(GUIManager *gui, GUIType::BrowserCache &browser, const std::string &file);
    static void DrawScriptCreatorDialog(GUIManager *gui, GUIType::BrowserCache &browser, std::string &title);

    static void LoadImagesFolder(GUIManager *gui);
    static void DrawBrowserFolders(
        GUIManager *gui,
        std::string &baseFolder,
        GUIType::BrowserCache &browser,
        std::string ext
    );
    static ImTextureID Icon(GUIType::Sheet coords);
};


#endif //BRAKEZA3D_FILESYSTEMGUI_H