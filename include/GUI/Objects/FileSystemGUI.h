//
// Created by Eduardo on 26/11/2025.
//

#ifndef BRAKEZA3D_FILESYSTEMGUI_H
#define BRAKEZA3D_FILESYSTEMGUI_H

#include "../GUI.h"
#include "../Editable/EditableOpenFile.h"

class GUIManager;
class EditableOpenFile;

class FileSystemGUI
{
public:
    // Projects
    static void DrawProjectFiles(GUIType::BrowserCache &browser);
    static void DrawProjectCreator();
    static void DrawProjectsTable(GUIType::BrowserCache &browser);
    static void DrawProjectRow(GUIType::BrowserCache &browser, const std::string &file, int index);
    static void DrawProjectRowActions(GUIType::BrowserCache &browser, const std::string &file);
    static void DrawProjectCreatorDialog(GUIType::BrowserCache &browser, std::string &title);

    // Scenes
    static void DrawSceneFiles(GUIType::BrowserCache &browser);
    static void DrawScenesTable(GUIType::BrowserCache & browser);
    static void DrawSceneRow(GUIType::BrowserCache &browser, const std::string &file, int index);
    static void DrawSceneRowActions(GUIType::BrowserCache &browser, const std::string &file);
    static void DrawSceneCreatorDialog(GUIType::BrowserCache &browser, std::string &title);

    // Shader
    static void DrawShaderFiles(GUIType::BrowserCache &browser);
    static void DrawShaderCreator(int &selectedIndex, const std::vector<const char*> &items);
    static void DrawShaderTypeCombo(int &selectedIndex, const std::vector<const char *> &items);
    static void DrawShadersTable(GUIType::BrowserCache &browser);
    static void DrawShaderRow(GUIType::BrowserCache &browser, const std::string &file, int index);
    static void DrawShaderRowActions(GUIType::BrowserCache &browser, const std::string &file);
    static void DrawShaderCreatorDialog(GUIType::BrowserCache &browser, std::string &title);
    static std::vector<const char *> getShaderTypeItems();

    // Scripts
    static void DrawScriptFiles(GUIType::BrowserCache &browser);
    static void DrawScriptsTable(GUIType::BrowserCache &browser);
    static void DrawScriptRow(GUIType::BrowserCache &browser, const std::string &file, int index);
    static void DrawScriptRowActions(GUIType::BrowserCache &browser, const std::string &file);
    static void DrawScriptCreatorDialog(GUIType::BrowserCache &browser, std::string &title);

    static void LoadImagesFolder(GUIType::BrowserCache &browser, TexturePackage &package);
    static void DrawBrowserFolders(const std::string &folder, GUIType::BrowserCache &browser);

    static ImTextureID Icon(GUIType::Sheet coords);

    static void DrawWinImages(GUIType::BrowserCache &browser, TexturePackage &package);
};


#endif //BRAKEZA3D_FILESYSTEMGUI_H