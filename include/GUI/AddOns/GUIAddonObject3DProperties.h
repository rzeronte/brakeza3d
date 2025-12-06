//
// Created by edu on 29/12/23.
//

#ifndef BRAKEZA3D_GUIWIDGETOBJECT3DPROPERTIES_H
#define BRAKEZA3D_GUIWIDGETOBJECT3DPROPERTIES_H

#include "../GUIManager.h"
#include "../../3D/Object3D.h"

class GUIManager;

class GUIAddonObject3DProperties
{
    std::vector<Object3D *> &gameObjects;
    GUIType::ScriptEditableManager &scriptEditableManager;
public:
    GUIAddonObject3DProperties(std::vector<Object3D *> &gameObjects, GUIType::ScriptEditableManager &scriptEditableManager);
    void DrawPropertiesBySelectedObject(GUIManager *gui);
};

#endif //BRAKEZA3D_GUIWIDGETOBJECT3DPROPERTIES_H
