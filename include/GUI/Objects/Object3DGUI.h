//
// Created by Eduardo on 24/11/2025.
//

#ifndef BRAKEZA3D_OBJECT3DGUI_H
#define BRAKEZA3D_OBJECT3DGUI_H

#include "../../3D/Object3D.h"
#include "../../Brakeza.h"

class GUIManager;

class Object3DGUI
{
public:
    static void DrawPropertiesGUI(Object3D *o);
    static void DrawObjectSelectedGuizmoOperation();
};

#endif //BRAKEZA3D_OBJECT3DGUI_H