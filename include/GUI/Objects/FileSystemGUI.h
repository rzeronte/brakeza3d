//
// Created by Eduardo on 26/11/2025.
//

#ifndef BRAKEZA3D_FILESYSTEMGUI_H
#define BRAKEZA3D_FILESYSTEMGUI_H

#include "../GUI.h"
#include "../Editable/EditableOpenFile.h"
#include "../include/Misc/TexturePackage.h"

class GUIManager;
class EditableOpenFile;

class FileSystemGUI
{
public:
    static inline GUIType::BrowserFileType type;

    static void DrawMainBrowser();

    static bool openPopUpCreateProject;
    static bool openPopupCreateScene;
    static bool openPopupCreateShader;
    static bool openPopupCreateScript;
    static bool autoExpandProject;
    static bool autoExpandScene;

    // Projects
    static void DrawProjectFiles(GUIType::BrowserCache &browser);
    static void DrawProjectsTable(GUIType::BrowserCache &browser);
    static void DrawProjectCreatorDialog(GUIType::BrowserCache &browser, std::string &title);

    // Scenes
    static void DrawSceneFiles(GUIType::BrowserCache &browser);
    static void DrawScenesTable(GUIType::BrowserCache & browser);
    static void DrawSceneCreatorDialog(GUIType::BrowserCache &browser, std::string &title);

    // Shader
    static void DrawShaderFiles(GUIType::BrowserCache &browser);
    static void DrawShaderTypeCombo(int &selectedIndex, const std::vector<const char *> &items);
    static void DrawShadersTable(GUIType::BrowserCache &browser);
    static void DrawShaderCreatorDialog(GUIType::BrowserCache &browser, std::string &title);
    static std::vector<const char *> getShaderTypeItems();

    // Scripts
    static void DrawScriptFiles(GUIType::BrowserCache &browser);
    static void DrawScriptsTable(GUIType::BrowserCache &browser);
    static void DrawScriptCreatorDialog(GUIType::BrowserCache &browser, std::string &title);

    static void LoadImagesFolder(GUIType::BrowserCache &browser, TexturePackage &package);
    static void DrawBrowserFolders(const std::string &folder, GUIType::BrowserCache &browser);

    static ImTextureID Icon(GUIType::Sheet coords);

    static void DrawWinMediaBrowser(GUIType::BrowserCache &browser, TexturePackage &package);
};


#endif //BRAKEZA3D_FILESYSTEMGUI_H