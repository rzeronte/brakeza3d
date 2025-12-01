//
// Created by edu on 29/12/23.
//

#ifndef BRAKEZA3D_GUIWIDGETMENU_H
#define BRAKEZA3D_GUIWIDGETMENU_H

#include "../GUITypes.h"

struct GUIAddonMenu
{
    std::vector<GUITypes::GUIWindowsStatus> &windowStatus;

    explicit GUIAddonMenu(std::vector<GUITypes::GUIWindowsStatus> &windowStatus);

    void Draw();
    void DrawMainMenuWindow();
    void DrawMainMenuBrakeza3D();
    void DrawMainMenuScriptControls();
    void DrawMainMenuAddObject();
    void DrawMainMenuVideo();
    void DrawMainMenuColliders();
    void DrawMainMenuIllumination();
    void DrawMainMenuCamera();
    void DrawMainMenuSound();
    void DrawMainMenuLogging();
    void DrawMainMenuLayout();
    void DrawAboutMeModal();
    void DrawImage2DMenuItem(const std::string& folder);
    void DrawImage2DAnimationMenuItem(const std::string& folder);
    void DrawImage3DMenuItem(const std::string& folder);
    void DrawBillboardAnimationMenuItem(const std::string& folder);
    void DrawMesh3DMenuItem(const std::string& folder);
    void DrawMesh3DAnimationMenuItem(const std::string& folder);
    static void drawItemsToLoad(
        const std::string &folder,
        const std::string &extension,
        const std::string &tagIcon,
        const std::function<void(const std::string &)> &loadFunction
    );
};

#endif //BRAKEZA3D_GUIWIDGETMENU_H
