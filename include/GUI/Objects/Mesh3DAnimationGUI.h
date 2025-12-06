//
// Created by Eduardo on 24/11/2025.
//

#ifndef BRAKEZA3D_MESH_3D_ANIMATIONGUI_H
#define BRAKEZA3D_MESH_3D_ANIMATIONGUI_H

#include "../../3D/Mesh3DAnimation.h"
#include "../../Brakeza.h"

class Mesh3DAnimationDrawerGUI
{
public:
    static void DrawPropertiesGUI(Mesh3DAnimation *o);
    static void DrawEditBonesMappingWindow(GUIManager *gui);
};

#endif //BRAKEZA3D_MESH_3D_ANIMATIONGUI_H