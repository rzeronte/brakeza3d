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
// Created by Eduardo on 16/12/2025.
//

#ifndef BRAKEZA3D_EDITABLEOPENSHADERFILE_H
#define BRAKEZA3D_EDITABLEOPENSHADERFILE_H

#include "EditableOpenFile.h"
#include "../../OpenGL/Code/ShaderBaseCustomOGLCode.h"

class EditableOpenShaderFile : public EditableOpenFile
{
    ShaderBaseCustomOGLCode *shader;
public:
    EditableOpenShaderFile(const std::string &uniqueId, const std::string &tabLabel, const std::string &path, ShaderBaseCustomOGLCode *shader);
    [[nodiscard]] ShaderBaseCustomOGLCode * getShader() const ;
    void setShader(ShaderBaseCustomOGLCode *shader) ;
    void DrawEditableOpenFileConfig() override;
    void DrawCodeEditActionButtons() override;
};

#endif //BRAKEZA3D_EDITABLEOPENSHADERFILE_H