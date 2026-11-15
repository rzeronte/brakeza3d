/*
 * Brakeza3D - 3D/2D game engine
 * Author:  Eduardo Rodríguez Álvarez
 * GitHub:  https://github.com/rzeronte/brakeza3d
 * Web:     https://brakeza.com
 * Started: 2018-09-30
 * Copyright (C) 2018-2026 Eduardo Rodríguez Álvarez
 * License: GNU General Public License v3.0 (see LICENSE)
 */

#ifndef BRAKEZA3D_SHADERSGUI_H
#define BRAKEZA3D_SHADERSGUI_H

#include <string>
#include "../GUI.h"
#include "../../3D/Mesh3D.h"
#include "../Editable/EditableOpenShaderFile.h"

class GUIManager;
class EditableOpenShaderFile;

class ShadersGUI
{
public:
    static void LoadDialogShader(const std::string &file);
    static void LoadDialogShader(ShaderBaseCustom *shader);

    static void DrawShaderConfig(EditableOpenShaderFile &file);
    static void DrawShaderConfigHeader(EditableOpenShaderFile &file);
    static void DrawShaderConfigEditName(EditableOpenShaderFile &file);
    static void DrawShaderConfigVarsCreator(EditableOpenShaderFile &file);
    static void DrawShaderConfigVarsTable(EditableOpenShaderFile &file);
    static void DrawShaderConfigEmptyStateWarning(EditableOpenShaderFile &file);
    static void DrawShaderConfigActionButtons(EditableOpenShaderFile &file);
    static ShaderBaseCustomMetaInfo ExtractShaderCustomCodeMetainfo(const std::string &pathFile);
    static ShaderBaseCustom *CreateShaderBaseCustom(ShaderCustomType type, std::string folder, std::string file);
    static std::vector<std::string> GetDataTypeItems(EditableOpenShaderFile &file);
    static void DrawWinObjectShaders();
};


#endif //BRAKEZA3D_SHADERSGUI_H