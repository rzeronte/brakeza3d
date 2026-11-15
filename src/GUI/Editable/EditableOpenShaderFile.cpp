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
// Created by Eduardo on 17/12/2025.
//

#include "../../../include/GUI/Editable/EditableOpenShaderFile.h"
#include "../../../include/Brakeza.h"
#include "../../../include/GUI/Windows/ShadersGUI.h"
#include "../../../include/GUI/Widgets/GUIHelpers.h"

EditableOpenShaderFile::EditableOpenShaderFile(const std::string &uniqueId, const std::string &tabLabel, const std::string &path, ShaderBaseCustomOGLCode *shader)
:
    EditableOpenFile(uniqueId, tabLabel, path, TextEditor::LanguageDefinition::GLSL()),
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
        GUIHelpers::SetupLeftRightColumns();

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
        ImGui::SameLine();
        DrawSwitchButton();

        ImGui::TableSetColumnIndex(1);
        GUI::DrawButton("Close file", IconGUI::CLEAR_SCENE, GUIType::Sizes::ICONS_CODE_EDITOR, false, [&] {
            Brakeza::get()->GUI()->CloseEditableFile(this);
        });

        ImGui::EndTable();
    }
}
