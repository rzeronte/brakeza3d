//
// Created by Eduardo on 17/12/2025.
//

#include "../../../include/GUI/TextEditor/EditableOpenShaderFile.h"
#include "../../../include/Brakeza.h"

EditableOpenShaderFile::EditableOpenShaderFile(const std::string &path, ShaderOGLCustom *shader)
:
    EditableOpenFile(path, TextEditor::LanguageDefinition::GLSL()),
    shader(shader)
{
}

[[nodiscard]] ShaderOGLCustom * EditableOpenShaderFile::getShader() const
{
    return shader;
}

void EditableOpenShaderFile::setShader(ShaderOGLCustom *shader)
{
    this->shader = shader;
}

void EditableOpenShaderFile::DrawEditableOpenFileSetup()
{
    ShadersGUI::DrawEditShaderWindow(*this);
}