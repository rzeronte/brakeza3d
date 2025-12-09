//
// Created by Eduardo on 26/11/2025.
//

#include <functional>
#include <string>
#include <vector>
#include "../../../include/GUI/Objects/ShadersGUI.h"
#include "../../../include/GUI/Objects/FileSystemGUI.h"
#include "../../../include/GUI/GUIManager.h"
#include "../../../include/Components/Components.h"
#include "../../../include/Misc/Tools.h"
#include "../../../include/Brakeza.h"

void ShadersGUI::DrawWinShaderEdition(GUIManager *gui)
{
    DrawShaderHeader(gui);
    DrawShaderConfiguration(gui);
    DrawVariableCreator(gui);
    DrawVariablesTable(gui);
    DrawEmptyStateWarning(gui);
    DrawSaveButton(gui);
}

void ShadersGUI::DrawSourceCodeEdit(GUIManager *gui) {
    ImGui::SeparatorText("GLSL code");

    static ImGuiInputTextFlags flags = ImGuiInputTextFlags_AllowTabInput;

    if (ImGui::CollapsingHeader("Vertex shader code")) {
        ImGui::InputTextMultiline("##sourceVS", gui->shaderEditableManager.editableSourceVS, IM_ARRAYSIZE(gui->shaderEditableManager.editableSourceVS), ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 10), flags);
        if (ImGui::Button(std::string("Save vertex shader").c_str())) {
            gui->shaderEditableManager.shader->sourceVS = gui->shaderEditableManager.editableSourceVS;
            Tools::WriteToFile(gui->shaderEditableManager.shader->getVertexFilename(), gui->shaderEditableManager.shader->sourceVS.c_str());
        }
    }

    if (ImGui::CollapsingHeader("Fragment shader code")) {
        ImGui::InputTextMultiline("##sourceFS", gui->shaderEditableManager.editableSourceFS, IM_ARRAYSIZE(gui->shaderEditableManager.editableSourceFS), ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 10), flags);
        if (ImGui::Button(std::string("Save fragment shader").c_str())) {
            gui->shaderEditableManager.shader->sourceFS = gui->shaderEditableManager.editableSourceFS;
            Tools::WriteToFile(gui->shaderEditableManager.shader->getFragmentFilename(), gui->shaderEditableManager.shader->sourceFS.c_str());
        }
    }
}

void ShadersGUI::DrawEditShaderWindow(GUIManager *gui)
{
    if (!gui->showEditShaderWindow) return;

    GUIManager::SetNextWindowSize(GUIType::Levels::DEFAULT_WINDOW_WIDTH, GUIType::Levels::DEFAULT_WINDOW_HEIGHT);

    if (ImGui::Begin("Shader edition", &gui->showEditShaderWindow, ImGuiWindowFlags_NoDocking)) {
        DrawWinShaderEdition(gui);
        DrawSourceCodeEdit(gui);
    }
    ImGui::End();
}

void ShadersGUI::DrawShaderHeader(GUIManager *gui)
{
    auto type = gui->shaderEditableManager.shader->getType();
    auto label = gui->shaderEditableManager.shader->getLabel();
    auto typeName = ShaderOGLCustom::getShaderTypeString(type);

    ImGui::Image(FileSystemGUI::Icon(IconGUI::SHADER_FILE), GUIType::Sizes::ICONS_BROWSERS);
    ImGui::SameLine();
    ImGui::Text(std::string("File: " + label).c_str());

    ImGui::Image(FileSystemGUI::Icon(IconGUI::SHADER_TYPE_MESH3D), GUIType::Sizes::ICONS_BROWSERS);
    ImGui::SameLine();
    ImGui::Text(std::string("Type: " + typeName).c_str());
}

void ShadersGUI::DrawShaderConfiguration(GUIManager *gui)
{
    ImGui::SeparatorText("Shader information");

    auto label = gui->shaderEditableManager.shader->getLabel();
    static char name[256];
    strncpy(name, label.c_str(), sizeof(name));

    if (ImGui::InputText("Name##", name, IM_ARRAYSIZE(name), ImGuiInputTextFlags_AutoSelectAll)) {
        gui->shaderEditableManager.shader->setLabel(name);
    }
}

void ShadersGUI::DrawVariableCreator(GUIManager *gui)
{
    ImGui::SeparatorText("Variables");

    static char varName[256];
    strncpy(varName, gui->currentVariableToAddName.c_str(), sizeof(varName));

    if (ImGui::InputText("Variable name##", varName, IM_ARRAYSIZE(varName), ImGuiInputTextFlags_AutoSelectAll)) {
        gui->currentVariableToAddName = varName;
    }

    static int selectedItem = 0;
    std::vector<const char*> typeItems = GetDataTypeItems(gui);

    ImGui::Combo("Type##1", &selectedItem, typeItems.data(), typeItems.size());
    ImGui::SameLine();

    GUI::DrawButton(
        "Create variable",
        IconGUI::SHADER_CREATE_VARIABLE,
        GUIType::Sizes::ICONS_BROWSERS,
        false,
        [&] {
            if (!gui->currentVariableToAddName.empty()) {
                gui->shaderEditableManager.shader->AddDataTypeEmpty(
                    gui->currentVariableToAddName.c_str(),
                    typeItems[selectedItem]
                );
            }
        }
    );

    ImGui::Separator();
}

std::vector<const char*> ShadersGUI::GetDataTypeItems(GUIManager *gui)
{
    std::vector<std::string> items;
    for (auto t : GLSLTypeMapping) {
        items.push_back(t.second.label);
    }

    // Remove mesh3d variable options if postprocessing
    if (gui->shaderEditableManager.shader->getType() == SHADER_POSTPROCESSING) {
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

void ShadersGUI::DrawVariablesTable(GUIManager *gui)
{
    static ImGuiTableFlags flags = ImGuiTableFlags_RowBg;
    auto types = gui->shaderEditableManager.shader->getDataTypes();

    if (ImGui::BeginTable("ScriptProperties", 3, flags)) {
        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("Actions", ImGuiTableColumnFlags_WidthFixed);
        for (int i = 0; i < types.size(); i++) {
            DrawVariableRow(gui, types, i);
        }
        ImGui::EndTable();
    }
}

void ShadersGUI::DrawVariableRow(GUIManager *gui, const std::vector<ShaderOpenGLCustomType>& types, int index)
{
    ImGui::PushID(index);
    ImGui::TableNextRow();
    auto dataType = &types[index];

    // Name column
    ImGui::TableSetColumnIndex(0);
    ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + 5.0f, ImGui::GetCursorPosY() + 5.0f));
    ImGui::Text("%d) %s", index + 1, dataType->name.c_str());

    // Type column
    ImGui::TableSetColumnIndex(1);
    ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + 5.0f, ImGui::GetCursorPosY() + 5.0f));
    ImGui::Text("%s", GLSLTypeMapping[dataType->type].label.c_str());

    // Actions column
    ImGui::TableSetColumnIndex(2);
    GUI::DrawButton(
        "Remove shader variable",
        IconGUI::SHADER_REMOVE_VARIABLE,
        GUIType::Sizes::ICONS_BROWSERS,
        true,
        [&] {
            gui->shaderEditableManager.shader->removeDataType(*dataType);
        }
    );
    ImGui::PopID();
}

void ShadersGUI::DrawEmptyStateWarning(GUIManager *gui)
{
    auto types = gui->shaderEditableManager.shader->getDataTypes();

    if (types.empty()) {
        ImGui::Image(FileSystemGUI::Icon(IconGUI::WARNING), GUIType::Sizes::ICONS_BROWSERS);
        ImGui::SameLine();
        ImGui::Text("No types defined");
        ImGui::Spacing();
    }
}

void ShadersGUI::DrawSaveButton(GUIManager *gui)
{
    GUI::DrawButton(
        "Save shader",
        IconGUI::SHADER_SAVE,
        GUIType::Sizes::ICONS_BROWSERS,
        true,
        [&] {
            gui->shaderEditableManager.shader->UpdateFileTypes();
        }
    );
}

void ShadersGUI::DrawWinObjectShaders(GUIManager *gui)
{
    auto windowStatus = gui->getWindowStatus(GUIType::OBJECT_SHADERS);
    if (!windowStatus->isOpen) return;

    auto objects = Brakeza::get()->getSceneObjects();
    bool hasSelectedIndex = gui->selectedObjectIndex >= 0 && gui->selectedObjectIndex < objects.size();

    if (!hasSelectedIndex) {
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
            auto folder = s->getFolder();
            auto jsonFilename = s->getLabel() + ".json";
            LoadShaderDialog(gui, folder, jsonFilename);
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


void ShadersGUI::LoadShaderDialog(GUIManager *gui, const std::string &folder, const std::string &file)
{
    auto shader = Components::get()->Render()->getLoadedShader(folder, file);
    delete gui->shaderEditableManager.shader;
    gui->shaderEditableManager.shader = shader;
    gui->shaderEditableManager.loaded = true;
    strcpy(gui->shaderEditableManager.editableSourceVS, shader->sourceVS.c_str());
    strcpy(gui->shaderEditableManager.editableSourceFS, shader->sourceFS.c_str());
    gui->showEditShaderWindow = true;
}