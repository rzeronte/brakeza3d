//
// Created by Eduardo on 26/11/2025.
//

#include <string>
#include <vector>
#include "../../../include/Brakeza.h"
#include "../../../include/GUI/Objects/ShadersGUI.h"
#include "../../../include/GUI/Objects/FileSystemGUI.h"
#include "../../../include/GUI/GUIManager.h"
#include "../../../include/GUI/Editable/EditableOpenShaderFile.h"
#include "../../../include/Components/Components.h"
#include "../../../include/OpenGL/Code/ShaderCustomOGLCodeTypes.h"
#include "../../../include/OpenGL/Code/ShaderOGLCustomCodeMesh3D.h"
#include "../../../include/OpenGL/Code/ShaderOGLCustomCodePostprocessing.h"
#include "../../../include/OpenGL/Nodes/ShaderBaseNodes.h"
#include "../../../include/Render/Drawable.h"

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
    auto typeName = ShaderBaseCustomOGLCode::getShaderTypeString(type);

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

    // Listas precalculadas (solo se hacen una vez)
    static std::vector<std::string> allGlslTypes;
    static std::vector<std::string> allLabels;
    static std::vector<const char*> allLabelsCStr;

    static std::vector<std::string> postprocessingGlslTypes;
    static std::vector<std::string> postprocessingLabels;
    static std::vector<const char*> postprocessingLabelsCStr;

    static bool initialized = false;
    if (!initialized) {
        // PRIMERO: Llenar todos los strings
        for (const auto& [glslType, info] : GLSLTypeMapping) {
            allGlslTypes.push_back(glslType);
            allLabels.push_back(info.label);

            if (info.type != ShaderOpenGLCustomDataType::DIFFUSE &&
                info.type != ShaderOpenGLCustomDataType::SPECULAR) {
                postprocessingGlslTypes.push_back(glslType);
                postprocessingLabels.push_back(info.label);
            }
        }

        // DESPUÉS: Llenar los punteros (ahora los vectores ya no se realojarán)
        for (const auto& label : allLabels) {
            allLabelsCStr.push_back(label.c_str());
        }
        for (const auto& label : postprocessingLabels) {
            postprocessingLabelsCStr.push_back(label.c_str());
        }

        initialized = true;
    }

    // Elegir la lista según el tipo de shader
    bool isPostprocessing = (file.getShader()->getType() == SHADER_POSTPROCESSING);
    const auto& glslTypes = isPostprocessing ? postprocessingGlslTypes : allGlslTypes;
    const auto& labelsCStr = isPostprocessing ? postprocessingLabelsCStr : allLabelsCStr;

    ImGui::SeparatorText("Create new uniform");

    static char localVarName[256] = "";
    auto fieldId = std::string("Uniform name##") + "_" + file.getShader()->getLabel();
    ImGui::InputText(fieldId.c_str(), localVarName, IM_ARRAYSIZE(localVarName), ImGuiInputTextFlags_AutoSelectAll);

    static int selectedItem = 0;

    auto comboId = std::string("Type##") + "_" + file.getShader()->getLabel();
    ImGui::Combo(comboId.c_str(), &selectedItem, labelsCStr.data(), (int) labelsCStr.size());
    ImGui::SameLine();

    GUI::DrawButton("Create uniform", IconGUI::SHADER_CREATE_VARIABLE, GUIType::Sizes::ICONS_BROWSERS, false, [&] {
        if (localVarName[0] != '\0' && selectedItem < glslTypes.size()) {
            file.getShader()->AddDataTypeEmpty(localVarName, glslTypes[selectedItem].c_str());
            localVarName[0] = '\0';
        }
    });
}

std::vector<std::string> ShadersGUI::GetDataTypeItems(EditableOpenShaderFile &file)
{
    std::vector<std::string> items;

    for (const auto& t : GLSLTypeMapping) {
        if (file.getShader()->getType() == SHADER_POSTPROCESSING) {
            auto typeEnum = t.second.type;
            if (typeEnum == ShaderOpenGLCustomDataType::DIFFUSE ||
                typeEnum == ShaderOpenGLCustomDataType::SPECULAR) {
                continue;
                }
        }

        items.push_back(t.second.label);
    }

    return items;
}

void ShadersGUI::DrawShaderConfigVarsTable(EditableOpenShaderFile &file)
{
    ImGui::SeparatorText("Shader uniforms");

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
            ShaderBaseCustomOGLCode::DrawTypeImGuiControl(*type);

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
        Drawable::WarningMessage("No variables defined");
    }
}

void ShadersGUI::DrawShaderConfigActionButtons(EditableOpenShaderFile &file)
{
    GUI::DrawButton("Save shader", IconGUI::SHADER_SAVE, GUIType::Sizes::ICONS_CODE_EDITOR, true,[&] {
        file.getShader()->UpdateFileTypes();
    });
}

void ShadersGUI::DrawWinObjectShaders()
{
    auto windowStatus = Brakeza::get()->GUI()->getWindowStatus(GUIType::OBJECT_SHADERS);
    if (!windowStatus->isOpen) return;

    auto objects = Brakeza::get()->getSceneObjects();

    auto o = Components::get()->Render()->getSelectedObject();
    if (o == nullptr) {
        Drawable::WarningMessage("No object selected");
        return;
    }

    auto mesh = dynamic_cast<Mesh3D*>(o);
    if (mesh == nullptr) {
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", "No Mesh3D object");
        return;
    }

    auto customShaders = mesh->getCustomShaders();
    if (customShaders.empty()) {
        Drawable::WarningMessage("No shaders in selected object");
    }

    if (ImGui::BeginTable("SceneShadersTable", 2, ImGuiTableFlags_None | ImGuiTableFlags_RowBg)) {
        ImGui::TableSetupColumn("Shader");
        ImGui::TableSetupColumn("Actions", ImGuiTableColumnFlags_WidthFixed);

        for (unsigned int i = 0; i < customShaders.size(); i++) {
            ImGui::PushID(i);
            auto s = customShaders[i];
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::Image(FileSystemGUI::Icon(IconGUI::SHADER_FILE), GUIType::Sizes::ICONS_OBJECTS_ALLOWED);
            ImGui::SameLine(0, 5.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 4));
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(2, 2));
            bool isOpenCurrentShader = ImGui::TreeNodeEx(s->getLabel().c_str(), ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_FramePadding);
            ImGui::PopStyleVar(2);
            if (isOpenCurrentShader) {
                s->DrawImGuiProperties(mesh->getModelTextures()[0], mesh->getModelTextures()[0]);
            }
            // Buttons
            ImGui::TableNextColumn();
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 4));    // padding inner button
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 2));     // spacing between buttonms

            GUI::DrawButtonTransparent(
                !s->isEnabled() ? "Unlock shader object" : "Lock shader object",
                !s->isEnabled() ? IconGUI::SHADER_LOCK : IconGUI::SHADER_UNLOCK,
                GUIType::Sizes::ICONS_BROWSERS,
                false,
                [&] { s->setEnabled(!s->isEnabled()); }
            );
            ImGui::SameLine();
            GUI::DrawButtonTransparent("Reload shader in object", IconGUI::SHADER_RELOAD, GUIType::Sizes::ICONS_BROWSERS, false, [&] {
                s->Reload();
            });
            ImGui::SameLine();
            GUI::DrawButtonTransparent("Edit shader", IconGUI::SHADER_EDIT, GUIType::Sizes::ICONS_BROWSERS, false, [&] {
                auto jsonFilename = s->getLabel() + ".json";
                LoadDialogShader(s->getTypesFile());
            });
            ImGui::SameLine();
            GUI::DrawButtonTransparent("Remove shader in object", IconGUI::SHADER_REMOVE, GUIType::Sizes::ICONS_BROWSERS, false, [&] {
                mesh->RemoveShader(i);
            });

            ImGui::PopStyleVar(2);

            ImGui::PopID();
        }
        ImGui::EndTable();
    }
}

void ShadersGUI::LoadDialogShader(const std::string &file)
{
    auto metaInfo = ExtractShaderCustomCodeMetainfo(file);

    if (!Brakeza::get()->GUI()->isEditableFileAlreadyOpen(metaInfo.name)) {
        auto shader = ComponentRender::CreateCustomShaderFromDisk(metaInfo, nullptr);
        shader->PrepareSync();

        Brakeza::get()->GUI()->OpenEditableFile(
            new EditableOpenShaderFile(shader->getLabel(), shader->getVertexFilename(), shader)
        );
    }
    Brakeza::get()->GUI()->setIndexCodeEditorTab(metaInfo.name);
    Components::get()->Window()->setImGuiConfig(Config::ImGUIConfigs::CODING);
}

ShaderBaseCustomMetaInfo ShadersGUI::ExtractShaderCustomCodeMetainfo(const std::string &pathFile)
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

ShaderBaseCustom* ShadersGUI::CreateShaderBaseCustom(ShaderCustomType type, std::string folder, std::string file)
{
    auto name = Tools::getFilenameWithoutExtension(file);
    std::string typesFile = folder + std::string(name + ".json");

    switch(type) {
        case SHADER_POSTPROCESSING:
        case SHADER_OBJECT: {
            ShaderBaseCustomOGLCode::WriteEmptyCustomShaderToDisk(name, folder, type);
            auto metaInfo = ExtractShaderCustomCodeMetainfo(typesFile);
            auto shader = ComponentRender::CreateCustomShaderFromDisk(metaInfo, nullptr);
            return shader;
        }
        case SHADER_NODE_OBJECT:
        case SHADER_NODE_POSTPROCESSING: {
            ShaderBaseNodes::WriteEmptyCustomShaderToDisk(name, folder, type);
            auto metaInfo = ExtractShaderCustomCodeMetainfo(typesFile);
            auto shader = ComponentRender::CreateCustomShaderFromDisk(metaInfo, nullptr);
            return shader;

        }
        case SHADER_NONE:
        default:
            return nullptr;
    }
}