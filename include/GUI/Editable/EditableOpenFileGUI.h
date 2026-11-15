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
// Created by Eduardo on 04/01/2026.
//

#ifndef BRAKEZA3D_EDITABLEOPENFILEGUI_H
#define BRAKEZA3D_EDITABLEOPENFILEGUI_H

#include "EditableOpenFile.h"

class EditableOpenFile;

class EditableOpenFileGUI
{
public:
    static void DrawCodeEditorTab(EditableOpenFile &file, int tabIndex);
};


#endif //BRAKEZA3D_EDITABLEOPENFILEGUI_H