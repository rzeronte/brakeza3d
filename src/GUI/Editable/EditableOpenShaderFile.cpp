//
// Created by Eduardo on 17/12/2025.
//

#include "../../../include/GUI/Editable/EditableOpenShaderFile.h"
#include "../../../include/Brakeza.h"
#include "../../../include/GUI/Objects/ShadersGUI.h"

EditableOpenShaderFile::EditableOpenShaderFile(const std::string &tabLabel, const std::string &path, ShaderBaseCustomOGLCode *shader)
:
    EditableOpenFile(tabLabel, path, TextEditor::LanguageDefinition::GLSL()),
    shader(shader)
{
}

[[nodiscard]] ShaderBaseCustomOGLCode * EditableOpenShaderFile::getShader() const
{
    return shader;
}

void EditableOpenShaderFile::setShader(ShaderBaseCustomOGLCode *value)
{
    shader = value;
}

void EditableOpenShaderFile::DrawEditableOpenFileConfig()
{
    ShadersGUI::DrawShaderConfig(*this);
}

void EditableOpenShaderFile::DrawCodeEditActionButtons()
{
    if (ImGui::BeginTable("ScriptFileButtons", 2, ImGuiTableFlags_None)) {
        ImGui::TableSetupColumn("Left", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Right", ImGuiTableColumnFlags_WidthFixed);

        ImGui::TableNextRow();

        ImGui::TableSetColumnIndex(0);
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

        ImGui::TableSetColumnIndex(1);
        GUI::DrawButton("Close file", IconGUI::CLEAR_SCENE, GUIType::Sizes::ICONS_CODE_EDITOR, false, [&] {
            Brakeza::get()->GUI()->CloseEditableFile(this);
        });

        ImGui::EndTable();
    }
}
