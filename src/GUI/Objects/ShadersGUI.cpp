//
// Created by Eduardo on 26/11/2025.
//

#include "../../../include/GUI/Objects/ShadersGUI.h"
#include "../../../include/GUI/Objects/FileSystemGUI.h"
#include "../../../include/GUI/GUIManager.h"

#include <functional>
#include <string>
#include <vector>

void ShadersGUI::DrawEditShaderWindow(GUIManager *gui)
{
    if (!gui->showEditShaderWindow) return;

    GUIManager::SetNextWindowSize(GUIType::Levels::DEFAULT_WINDOW_WIDTH, GUIType::Levels::DEFAULT_WINDOW_HEIGHT);
    ImGui::SetNextWindowBgAlpha(GUIType::Levels::WINDOW_ALPHA);

    if (ImGui::Begin("Shader edition", &gui->showEditShaderWindow, ImGuiWindowFlags_NoDocking)) {
        DrawShaderVariables(gui);

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
    ImGui::End();
}

void ShadersGUI::DrawShaderVariables(GUIManager *gui)
{
    auto type = gui->shaderEditableManager.shader->getType();
    auto typeName = gui->shaderEditableManager.shader->getShaderTypeString(type);

    ImGui::SeparatorText((gui->shaderEditableManager.shader->getLabel() + ": " + typeName).c_str());

    static char name[256];
    strncpy(name, gui->currentVariableToAddName.c_str(), sizeof(name));

    if (ImGui::InputText("Variable name##", name, IM_ARRAYSIZE(name), ImGuiInputTextFlags_AutoSelectAll)) {
        gui->currentVariableToAddName = name;
    }

    std::vector<std::string> items;
    for (auto t : GLSLTypeMapping) {
        items.push_back(t.first);
    }

    if (gui->shaderEditableManager.shader->getType() == ShaderCustomTypes::SHADER_POSTPROCESSING) {
        items.erase(std::remove_if(items.begin(), items.end(), [](const std::string& item) {
            auto typeEnum = GLSLTypeMapping[item];
            return typeEnum == ShaderOpenGLCustomDataType::DIFFUSE || typeEnum == ShaderOpenGLCustomDataType::SPECULAR;
        }), items.end());
    }

    std::vector<const char*> itemsCStr;
    for (auto& item : items) itemsCStr.push_back(item.c_str());

    static int selectedItem = 0;
    ImGui::Combo("Type##1", &selectedItem, itemsCStr.data(), itemsCStr.size());
    ImGui::SameLine();
    if (ImGui::Button(std::string("Create variable").c_str())) {
        if (!gui->currentVariableToAddName.empty()) {
            gui->shaderEditableManager.shader->addDataTypeEmpty(gui->currentVariableToAddName.c_str(), itemsCStr[selectedItem]);
        }
    }

    ImGui::SeparatorText("Variables");

    static ImGuiTableFlags flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingStretchProp;
    auto types = gui->shaderEditableManager.shader->getDataTypes();
    if (ImGui::BeginTable("ScriptProperties", 3, flags)) {
        for (int i = 0; i < types.size(); i++) {
            ImGui::PushID(i);
            ImGui::TableNextRow();
            auto dataType = &types[i];

            ImGui::TableSetColumnIndex(0);
            ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + 5.0f, ImGui::GetCursorPosY() + 5.0f));
            ImGui::Text("%d) %s", i + 1, dataType->name.c_str());

            ImGui::TableSetColumnIndex(1);
            ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + 5.0f, ImGui::GetCursorPosY() + 5.0f));
            ImGui::Text("%s", dataType->type.c_str());

            ImGui::TableSetColumnIndex(2);
            GUI::DrawButton("Remove shader variable", IconGUI::REMOVE, GUIType::Sizes::ICONS_BROWSERS, true, [&] {
                gui->shaderEditableManager.shader->removeDataType(*dataType);
            });
            ImGui::PopID();
        }
        ImGui::EndTable();
    }

    if (types.empty()) {
        ImGui::Text("No types defined");
        ImGui::Spacing();
    }

    if (ImGui::Button(std::string("Save shader types").c_str())) {
        gui->shaderEditableManager.shader->updateFileTypes();
    }
}

void ShadersGUI::DrawCustomShadersFolder(GUIManager *gui, GUIType::FolderBrowserCache &browser)
{
    static char name[256];
    strncpy(name, gui->currentVariableToCreateCustomShader.c_str(), sizeof(name));
    if (ImGui::InputText("Shader name##", name, IM_ARRAYSIZE(name), ImGuiInputTextFlags_AutoSelectAll)) {
        gui->currentVariableToCreateCustomShader = name;
    }

    std::vector<const char*> items;
    for (const auto& pair : ComponentsManager::get()->getComponentRender()->getShaderTypesMapping()) {
        items.push_back(pair.first.c_str());
    }

    static int item_current_idx = 0;

    if (ImGui::Combo("Type", &item_current_idx, items.data(), items.size())) {
        std::cout << "Seleccionado: " << items[item_current_idx] << std::endl;
    }

    GUI::DrawButton("Create shader", IconGUI::OPEN, GUIType::Sizes::ICONS_BROWSERS, true, [&] {
        if (!gui->currentVariableToCreateCustomShader.empty()) {
            auto type = ShaderOGLCustom::getShaderTypeFromString(items[item_current_idx]);
            ShaderOGLCustom::createEmptyCustomShader(gui->currentVariableToCreateCustomShader, browser.currentFolder, type);
            FileSystemGUI::UpdateFolderFiles(gui);
        }
    });

    ImGui::Separator();
    FileSystemGUI::DrawBrowserFolders(gui, Config::get()->CUSTOM_SHADERS_FOLDER, browser,Config::get()->SHADERS_EXT);
    ImGui::Separator();

    auto files = browser.folderFiles;
    static ImGuiTableFlags flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingStretchProp;
    if (ImGui::BeginTable("ScriptsFolderTable", 3, flags)) {
        for (int i = 0; i < files.size(); i++) {
            ImGui::PushID(i);
            auto file = files[i];
            auto fullPath = browser.currentFolder + file;
            ImGui::TableNextRow();

            ImGui::TableSetColumnIndex(0);
            ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + 5.0f, ImGui::GetCursorPosY() + 5.0f));
            ImGui::Image(FileSystemGUI::Icon(IconGUI::SHADER), GUIType::Sizes::ICONS_BROWSERS);
            ImGui::SameLine();
            std::string optionText = std::to_string(i + 1) + ") " + file;
            if (ImGui::Selectable(optionText.c_str())) {
                LoadShaderDialog(gui, browser.currentFolder, file);
            }

            if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
                static std::string folderCopy, fileCopy;
                folderCopy = browser.currentFolder;
                fileCopy = file;
                Config::DragDropCustomShaderData data = {folderCopy.c_str(), fileCopy.c_str()};
                ImGui::SetDragDropPayload("CUSTOMSHADER_ITEM", &data,sizeof(data));
                ImGui::Text("%s", fullPath.c_str());
                ImGui::EndDragDropSource();
            }
            ImGui::TableSetColumnIndex(1);
            GUI::DrawButton("Load shader into scene", IconGUI::SCENE, GUIType::Sizes::ICONS_BROWSERS, true, [&] {
                ComponentsManager::get()->getComponentRender()->loadShaderIntoScene(browser.currentFolder, file);
            });
            ImGui::TableSetColumnIndex(2);
            if (ImGui::ImageButton(FileSystemGUI::Icon(IconGUI::REMOVE), ImVec2(14, 14))) {
                ImGui::OpenPopup("##Deleting shader");
            }
            ImGui::SetItemTooltip("Delete shader");
            GUIManager::ShowDeletePopup("##Deleting shader", "Are you sure to delete shader?", [&] () {
                ShaderOGLCustom::removeCustomShaderFiles(browser.currentFolder,  Tools::getFilenameWithoutExtension(file));
                FileSystemGUI::UpdateFolderFiles(gui);
            });
            ImGui::PopID();
        }
        ImGui::EndTable();
    }
}

void ShadersGUI::DrawShadersBySelectedObject(GUIManager *gui)
{
    bool hasSelectedIndex = gui->selectedObjectIndex >= 0 && gui->selectedObjectIndex < gui->gameObjects.size();

    if (!hasSelectedIndex) {
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", "No object selected");
        return;
    }

    auto mesh = dynamic_cast<Mesh3D*>(gui->gameObjects[gui->selectedObjectIndex]);

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

        GUI::DrawButton("Load shader in object", IconGUI::SHADER, GUIType::Sizes::ICONS_BROWSERS, true, [&] {
            auto folder = s->getFolder();
            auto jsonFilename = s->getLabel() + ".json";
            LoadShaderDialog(gui, folder, jsonFilename);
        });
        ImGui::SameLine();
        if (!s->isEnabled()) {
            GUI::DrawButton("Unlock shader in object", IconGUI::UNLOCK, GUIType::Sizes::ICONS_BROWSERS, true, [&] {
                s->setEnabled(true);
            });
        } else {
            GUI::DrawButton("Lock shader in object", IconGUI::LOCK, GUIType::Sizes::ICONS_BROWSERS, false, [&] {
                s->setEnabled(false);
            });
        }
        ImGui::SameLine();
        GUI::DrawButton("Reload shader in object", IconGUI::RELOAD, GUIType::Sizes::ICONS_BROWSERS, false, [&] {
            s->reload();
        });
        ImGui::SameLine();
        GUI::DrawButton("Remove shader in object", IconGUI::REMOVE, GUIType::Sizes::ICONS_BROWSERS, false, [&] {
            mesh->removeShader(i);
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
    auto shader = ComponentsManager::get()->getComponentRender()->getLoadedShader(folder, file);
    delete gui->shaderEditableManager.shader;
    gui->shaderEditableManager.shader = shader;
    gui->shaderEditableManager.loaded = true;
    strcpy(gui->shaderEditableManager.editableSourceVS, shader->sourceVS.c_str());
    strcpy(gui->shaderEditableManager.editableSourceFS, shader->sourceFS.c_str());
    gui->showEditShaderWindow = true;
}