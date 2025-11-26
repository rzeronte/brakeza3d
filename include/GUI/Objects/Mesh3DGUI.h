//
// Created by Eduardo on 24/11/2025.
//

#ifndef BRAKEZA3D_MESH3DGUI_H
#define BRAKEZA3D_MESH3DGUI_H

#include "../../3D/Mesh3D.h"

class GUIManager;

class Mesh3DGUI
{
public:
    static void drawImGuiProperties(Mesh3D *o);
    static void DrawEditBonesMappingWindow(GUIManager *gui);
};

#endif //BRAKEZA3D_MESH3DGUI_H