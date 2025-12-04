//
// Created by Eduardo on 04/12/2025.
//

#ifndef BRAKEZA3D_GUI_H
#define BRAKEZA3D_GUI_H
#include "GUITypes.h"

class GUI
{
public:
    static void DrawButton(const std::string &tooltip, GUISheet icon, bool active, const std::function<void()> &onClick);
    static void Toggle(bool &value);
    static GUITypes::FolderBrowserCache CreateBrowserCache(std::string folder, std::string &extension);

};
#endif //BRAKEZA3D_GUI_H