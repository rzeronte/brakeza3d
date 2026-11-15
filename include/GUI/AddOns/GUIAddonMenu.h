/*
 * Brakeza3D - 3D/2D game engine
 * Author:  Eduardo Rodríguez Álvarez
 * GitHub:  https://github.com/rzeronte/brakeza3d
 * Web:     https://brakeza.com
 * Started: 2018-09-30
 * Copyright (C) 2018-2026 Eduardo Rodríguez Álvarez
 * License: GNU General Public License v3.0 (see LICENSE)
 */
//
// Created by edu on 29/12/23.
//

#ifndef BRAKEZA3D_GUIWIDGETMENU_H
#define BRAKEZA3D_GUIWIDGETMENU_H

#include "../GUI.h"
#include "../GUIManager.h"

class GUIManager;

class GUIAddonMenu
{
public:
    explicit GUIAddonMenu() = default;

    static inline float snapRadius = 10.0f;

    static void Draw(GUIManager *gui);
    static void MenuBrakeza3D();
    static void MenuEdit();
    static void MenuScriptControls();
    static void MenuAddObject();
    static void MenuVideo();
    static void MenuColliders();
    static void MenuIllumination();
    static void MenuCamera();
    static void MenuSound();
    static void MenuLogging();
    static void MenuLayout();
    static void MenuInterface();

    static void MenuWindow(GUIManager *gui);
    static void AboutMeModal();

    static void DrawItemsToLoad(std::string &folder, std::string &ext, GUIType::Sheet icon, const std::function<void(const std::string &)> &cb );

    static void MenuFile(GUIManager *gui);

    struct FileMenuAction {
        GUIType::Sheet icon;
        const char* label;
        std::function<void(const std::string&)> callback;
    };
    static void DrawFileMenuTree(std::string& folder, std::string& ext, GUIType::Sheet fileIcon, const std::vector<FileMenuAction>& actions);

    static void MenuResourcesHub();
};

#endif //BRAKEZA3D_GUIWIDGETMENU_H
