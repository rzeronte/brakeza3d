//
// Created by edu on 29/12/23.
//

#ifndef BRAKEZA3D_GUIWIDGETOBJECT3DPROPERTIES_H
#define BRAKEZA3D_GUIWIDGETOBJECT3DPROPERTIES_H

#include "../GUIManager.h"

class GUIManager;

class GUIAddonObject3DProperties
{
public:
    GUIAddonObject3DProperties();
    static void DrawPropertiesBySelectedObject(GUIManager *gui);
};

#endif //BRAKEZA3D_GUIWIDGETOBJECT3DPROPERTIES_H
