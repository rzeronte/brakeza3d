//
// Created by Eduardo on 16/12/2025.
//

#ifndef BRAKEZA3D_EDITABLEOPENSHADERFILE_H
#define BRAKEZA3D_EDITABLEOPENSHADERFILE_H

#include "EditableOpenFile.h"
#include "../../OpenGL/ShaderOGLCustom.h"
#include "../Objects/ShadersGUI.h"

class EditableOpenShaderFile : public EditableOpenFile
{
    ShaderOGLCustom *shader;
public:
    EditableOpenShaderFile(const std::string &path, ShaderOGLCustom *shader);
    [[nodiscard]] ShaderOGLCustom * getShader() const ;
    void setShader(ShaderOGLCustom *shader) ;
    void DrawEditableOpenFileConfig() override;
    void DrawCodeEditActionButtons() override;
};

#endif //BRAKEZA3D_EDITABLEOPENSHADERFILE_H