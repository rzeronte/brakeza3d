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

#ifndef BRAKEZA3D_GUIWIDGETOBJECT3DPROPERTIES_H
#define BRAKEZA3D_GUIWIDGETOBJECT3DPROPERTIES_H

#include "../GUIManager.h"

class GUIManager;

class GUIAddonObject3DProperties
{
public:
    GUIAddonObject3DProperties() = default;
    static void DrawWinObjectProps(GUIManager *gui);
};

#endif //BRAKEZA3D_GUIWIDGETOBJECT3DPROPERTIES_H
