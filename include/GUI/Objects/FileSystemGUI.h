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
    static void UpdateFolderFiles(GUIManager *gui);
    static void DrawProjectFiles(GUIManager *gui, GUIType::FolderBrowserCache &browser);
    static void DrawScenesFolder(GUIManager *gui, GUIType::FolderBrowserCache &browser);
    static void LoadImagesFolder(GUIManager *gui);
    static void DrawBrowserFolders(
        GUIManager *gui,
        std::string &folderBase,
        GUIType::FolderBrowserCache &browser,
        std::string ext
    );
    static ImTextureID Icon(GUIType::Sheet coords);
};


#endif //BRAKEZA3D_FILESYSTEMGUI_H