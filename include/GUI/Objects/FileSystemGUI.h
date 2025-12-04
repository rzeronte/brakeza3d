//
// Created by Eduardo on 26/11/2025.
//

#ifndef BRAKEZA3D_FILESYSTEMGUI_H
#define BRAKEZA3D_FILESYSTEMGUI_H

#include "../GUITypes.h"
#include "../include/Misc/TexturePackage.h"

class GUIManager;

class FileSystemGUI
{
public:
    static void UpdateFolderFiles(GUIManager *gui);
    static void DrawProjectFiles(GUIManager *gui, std::string folder);
    static void DrawScenesFolder(GUIManager *gui, std::string folder);
    static void LoadImagesFolder(GUIManager *gui);
    static void DrawBrowserFolders(
        GUIManager *gui,
        std::string &folder,
        std::string &folderBase,
        std::string &destiny,
        std::vector<std::string> &folders,
        std::vector<std::string> &files,
        std::string ext
    );
    static ImTextureID Icon(GUISheet coords);
};


#endif //BRAKEZA3D_FILESYSTEMGUI_H