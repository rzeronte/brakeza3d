//
// Created by Eduardo on 26/11/2025.
//

#ifndef BRAKEZA3D_FILESYSTEMGUI_H
#define BRAKEZA3D_FILESYSTEMGUI_H

#include "../include/Misc/TexturePackage.h"

class GUIManager;

class FileSystemGUI
{
public:
    static void updateFolderFiles(GUIManager *gui);
    static void drawProjectsFiles(GUIManager *gui, const std::string &folder);
    static void drawScenesFolder(GUIManager *gui, const std::string &folder);
    static void LoadImagesFolder(GUIManager *gui);
    static void LoadIcons(TexturePackage &icon);

    static void drawBrowserFolders(
        GUIManager *gui,
        const std::string &folder,
        const std::string &folderBase,
        std::string &destiny,
        std::vector<std::string> &folders,
        std::vector<std::string> &files,
        const std::string &ext
    );
};


#endif //BRAKEZA3D_FILESYSTEMGUI_H