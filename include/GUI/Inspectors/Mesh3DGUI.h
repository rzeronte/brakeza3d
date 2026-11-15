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
// Created by Eduardo on 24/11/2025.
//

#ifndef BRAKEZA3D_MESH3DGUI_H
#define BRAKEZA3D_MESH3DGUI_H

#include "../../3D/Mesh3D.h"

class GUIManager;

class Mesh3DGUI
{
public:
    static void DrawPropertiesGUI(Mesh3D *o);
};

#endif //BRAKEZA3D_MESH3DGUI_H