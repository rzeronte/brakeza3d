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

void EditableOpenShaderFile::DrawEditableOpenFileConfig()
{
    ShadersGUI::DrawShaderConfig(*this);
}

void EditableOpenShaderFile::DrawCodeEditActionButtons()
{
    GUI::DrawButton("Save file", IconGUI::SAVE, GUIType::Sizes::ICONS_CODE_EDITOR, false, [&] {
        Tools::WriteToFile(getPath(), getEditor().GetText().c_str());
    });
    ImGui::SameLine();
    GUI::DrawButton("Edit VS source", IconGUI::SHADER_CODE_VS, GUIType::Sizes::ICONS_CODE_EDITOR, false, [&] {
        setContentFromFile(shader->getVertexFilename());
    });
    ImGui::SameLine();
    GUI::DrawButton("Edit FS source", IconGUI::SHADER_CODE_FS, GUIType::Sizes::ICONS_CODE_EDITOR, false, [&] {
        setContentFromFile(shader->getFragmentFilename());
    });
    ImGui::SameLine();
    GUI::DrawButton("Close file", IconGUI::CLEAR_SCENE, GUIType::Sizes::ICONS_CODE_EDITOR, false, [&] {
        Brakeza::get()->GUI()->CloseEditableFile(this);
    });
}
