//
// Created by Eduardo on 26/11/2025.
//

#include <string>
#include <vector>
#include "../../../include/Brakeza.h"
#include "../../../include/GUI/Objects/ShadersGUI.h"
#include "../../../include/GUI/Objects/FileSystemGUI.h"
#include "../../../include/GUI/GUIManager.h"
#include "../../../include/GUI/TextEditor/EditableOpenShaderFile.h"
#include "../../../include/Components/Components.h"

void ShadersGUI::DrawShaderConfig(EditableOpenShaderFile &file)
{
    DrawShaderConfigHeader(file);
    DrawShaderConfigEditName(file);
    DrawShaderConfigVarsCreator(file);
    DrawShaderConfigVarsTable(file);
    DrawShaderConfigEmptyStateWarning(file);
    ImGui::Separator();
    DrawShaderConfigActionButtons(file);
}

void ShadersGUI::DrawShaderConfigHeader(EditableOpenShaderFile &file)
{
    auto vsFile = file.getShader()->getVertexFilename();
    auto fsFile = file.getShader()->getFragmentFilename();
    auto type = file.getShader()->getType();
    auto typeName = ShaderOGLCustom::getShaderTypeString(type);

    ImGui::Image(FileSystemGUI::Icon(IconGUI::SHADER_CODE_VS), GUIType::Sizes::ICONS_BROWSERS);
    ImGui::SameLine();
    ImGui::Text(std::string("VS file: " + vsFile).c_str());

    ImGui::Image(FileSystemGUI::Icon(IconGUI::SHADER_CODE_FS), GUIType::Sizes::ICONS_BROWSERS);
    ImGui::SameLine();
    ImGui::Text(std::string("FS file: " + fsFile).c_str());

    ImGui::Separator();
    ImGui::Image(FileSystemGUI::Icon(IconGUI::SHADER_TYPE_MESH3D), GUIType::Sizes::ICONS_BROWSERS);
    ImGui::SameLine();
    ImGui::Text(std::string("Shader type: " + typeName).c_str());
}

void ShadersGUI::DrawShaderConfigEditName(EditableOpenShaderFile &file)
{
    ImGui::Separator();

    auto label = file.getShader()->getLabel();
    static char name[256];
    strncpy(name, label.c_str(), sizeof(name));

    if (ImGui::InputText("Name##", name, IM_ARRAYSIZE(name), ImGuiInputTextFlags_AutoSelectAll)) {
        file.getShader()->setLabel(name);
    }
}

void ShadersGUI::DrawShaderConfigVarsCreator(EditableOpenShaderFile &file)
{
    auto gui = Brakeza::get()->GUI();

    auto fieldId = std::string("Variable name##") +  "_" + file.getShader()->getLabel();

    ImGui::SeparatorText("Create new variable");

    static char varName[256];
    strncpy(varName, gui->currentVariableToAddName.c_str(), sizeof(varName));

    if (ImGui::InputText(fieldId.c_str(), varName, IM_ARRAYSIZE(varName), ImGuiInputTextFlags_AutoSelectAll)) {
        gui->currentVariableToAddName = varName;
    }

    static int selectedItem = 0;
    std::vector<const char*> typeItems = GetDataTypeItems(file);

    auto comboId = std::string("Type##") +  "_" + file.getShader()->getLabel();
    ImGui::Combo(comboId.c_str(), &selectedItem, typeItems.data(), typeItems.size());
    ImGui::SameLine();

    auto buttonId = std::string("Create variable##") +  "_" + file.getShader()->getLabel();
    GUI::DrawButton(buttonId.c_str(), IconGUI::SHADER_CREATE_VARIABLE, GUIType::Sizes::ICONS_BROWSERS, false,[&] {
        if (!gui->currentVariableToAddName.empty()) {
            file.getShader()->AddDataTypeEmpty(gui->currentVariableToAddName.c_str(),typeItems[selectedItem]);
        }
    });

    ImGui::Separator();
}

std::vector<const char*> ShadersGUI::GetDataTypeItems(EditableOpenShaderFile &file)
{
    std::vector<std::string> items;
    for (auto t : GLSLTypeMapping) {
        items.push_back(t.second.label);
    }

    // Remove mesh3d variable options if postprocessing
    if (file.getShader()->getType() == SHADER_POSTPROCESSING) {
        items.erase(std::remove_if(items.begin(), items.end(), [](const std::string& item) {
            auto typeEnum = GLSLTypeMapping[item].type;
            return typeEnum == ShaderOpenGLCustomDataType::DIFFUSE ||
                   typeEnum == ShaderOpenGLCustomDataType::SPECULAR;
        }), items.end());
    }

    std::vector<const char*> itemsCStr;
    for (auto& item : items) {
        itemsCStr.push_back(item.c_str());
    }

    return itemsCStr;
}

void ShadersGUI::DrawShaderConfigVarsTable(EditableOpenShaderFile &file)
{
    ImGui::SeparatorText("Shader uniforms");

    auto shader = file.getShader();

    static ImGuiTableFlags flags = ImGuiTableFlags_RowBg;
    auto tableId = std::string("ShaderProperties_" + file.getShader()->getLabel());
    if (ImGui::BeginTable(tableId.c_str(), 4, flags)) {
        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("Actions", ImGuiTableColumnFlags_WidthFixed);
        for (int i = 0; i < file.getShader()->dataTypes.size(); i++) {
            auto type = &file.getShader()->dataTypes[i];
            auto fileId = std::string("shader_table_var_" + type->name + "_" + type->type);
            ImGui::PushID(fileId.c_str());
            ImGui::TableNextRow();

            ImGui::TableNextRow();
            // Name column
            ImGui::TableSetColumnIndex(0);
            ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + 5.0f, ImGui::GetCursorPosY() + 5.0f));
            ImGui::Text("%d) %s", i + 1, type->name.c_str());

            // Type column
            ImGui::TableSetColumnIndex(1);
            ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + 5.0f, ImGui::GetCursorPosY() + 5.0f));
            ImGui::Text("%s", GLSLTypeMapping[type->type].label.c_str());

            // Value ImGuiControl
            ImGui::TableSetColumnIndex(2);
            ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + 5.0f, ImGui::GetCursorPosY() + 5.0f));
            shader->DrawTypeImGuiControl(*type);

            // Actions column
            ImGui::TableSetColumnIndex(3);
            GUI::DrawButton("Remove shader variable", IconGUI::SHADER_REMOVE_VARIABLE, GUIType::Sizes::ICONS_BROWSERS, true, [&] {
                file.getShader()->removeDataType(*type);
            });
            ImGui::PopID();
        }
        ImGui::EndTable();
    }
}

void ShadersGUI::DrawShaderConfigEmptyStateWarning(EditableOpenShaderFile &file)
{
    auto types = file.getShader()->getDataTypes();

    if (types.empty()) {
        ImGui::Image(FileSystemGUI::Icon(IconGUI::WARNING), GUIType::Sizes::ICONS_BROWSERS);
        ImGui::SameLine();
        ImGui::Text("No variables defined");
        ImGui::Spacing();
    }
}

void ShadersGUI::DrawShaderConfigActionButtons(EditableOpenShaderFile &file)
{
    GUI::DrawButton("Save shader", IconGUI::SHADER_SAVE, GUIType::Sizes::ICONS_CODE_EDITOR, true,[&] {
        file.getShader()->UpdateFileTypes();
    });
}

void ShadersGUI::DrawWinObjectShaders(GUIManager *gui)
{
    auto windowStatus = gui->getWindowStatus(GUIType::OBJECT_SHADERS);
    if (!windowStatus->isOpen) return;

    auto objects = Brakeza::get()->getSceneObjects();
    bool hasSelectedIndex = gui->selectedObjectIndex >= 0 && gui->selectedObjectIndex < objects.size();

    if (!hasSelectedIndex) {
        ImGui::Image(FileSystemGUI::Icon(IconGUI::WARNING), GUIType::Sizes::ICONS_BROWSERS);
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", "No object selected");
        return;
    }

    auto mesh = dynamic_cast<Mesh3D*>(objects[gui->selectedObjectIndex]);

    if (mesh == nullptr) {
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", "No Mesh3D object");
        return;
    }

    auto customShaders = mesh->getCustomShaders();
    if (customShaders.empty()) {
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", "No shaders in selected object.");
    }

    for (unsigned int i = 0; i < customShaders.size(); i++) {
        ImGui::PushID(i);
        auto s = customShaders[i];

        GUI::DrawButtonTransparent(
            !s->isEnabled() ? "Unlock shader object" : "Lock shader object",
            !s->isEnabled() ? IconGUI::SHADER_LOCK : IconGUI::SHADER_UNLOCK,
            GUIType::Sizes::ICONS_BROWSERS,
            false,
            [&] { s->setEnabled(!s->isEnabled()); }
        );
        ImGui::SameLine();
        GUI::DrawButton("Reload shader in object", IconGUI::SHADER_RELOAD, GUIType::Sizes::ICONS_BROWSERS, false, [&] {
            s->Reload();
        });
        ImGui::SameLine();
        GUI::DrawButton("Edit shader", IconGUI::SHADER_EDIT, GUIType::Sizes::ICONS_BROWSERS, false, [&] {
            auto jsonFilename = s->getLabel() + ".json";
            LoadDialogShader(s->getTypesFile());
        });
        ImGui::SameLine();
        GUI::DrawButton("Remove shader in object", IconGUI::SHADER_REMOVE, GUIType::Sizes::ICONS_BROWSERS, false, [&] {
            mesh->RemoveShader(i);
        });
        ImGui::SameLine();
        if (ImGui::CollapsingHeader(s->getLabel().c_str())) {
            s->drawImGuiProperties(mesh->getModelTextures()[0], mesh->getModelTextures()[0]);
        }
        ImGui::PopID();
    }
}

void ShadersGUI::LoadDialogShader(const std::string &file)
{
    auto shader = ComponentRender::CreateCustomShaderFromDisk(ExtractShaderMetainfo(file));

    shader->PrepareBackground();
    shader->PrepareMainThread();

    if (!Brakeza::get()->GUI()->isEditableFileAlreadyOpen(shader->getLabel())) {
        Brakeza::get()->GUI()->OpenEditableFile(
            new EditableOpenShaderFile(
                shader->getLabel(),
                shader->getVertexFilename(),
                shader
            )
        );
    }
}

ShaderOGLMetaInfo ShadersGUI::ExtractShaderMetainfo(const std::string &pathFile)
{
    auto json = cJSON_Parse(Tools::ReadFile(pathFile));

    return {
        cJSON_GetObjectItemCaseSensitive(json, "name")->valuestring,
        cJSON_GetObjectItemCaseSensitive(json, "type")->valuestring,
        cJSON_GetObjectItemCaseSensitive(json, "vsFile")->valuestring,
        cJSON_GetObjectItemCaseSensitive(json, "fsFile")->valuestring,
        cJSON_GetObjectItemCaseSensitive(json, "typesFile")->valuestring
    };
}
