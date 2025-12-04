//
// Created by Eduardo on 26/11/2025.
//

#include "../include/GUI/Objects/ScriptLuaGUI.h"
#include "../include/GUI/Objects/FileSystemGUI.h"
#include "../../../include/GUI/GUIManager.h"

void ScriptLuaGUI::DrawPropertiesGUI(ScriptLUA *o)
{
    ImGui::SeparatorText("LUA variables");

    if (o->dataTypes.empty()) {
        ImGui::Text("No variables found");
        return;
    }

    int i = 0;
    for (auto&  type: o->dataTypes) {
        ImGui::PushID(i);
        switch (BrakezaSetup::get()->LUADataTypesMapping[type.type]) {
            case BrakezaSetup::LUADataType::INT: {
                int valueInt = std::get<int>(type.value);
                if (ImGui::InputInt(type.name.c_str(), &valueInt)) {
                    type.value = valueInt;
                }
                break;
            }
            case BrakezaSetup::LUADataType::STRING: {
                std::string valueString = std::get<const char*>(type.value);
                static char name[256];
                strncpy(name, valueString.c_str(), sizeof(name));
                ImGui::InputText(type.name.c_str(), name, IM_ARRAYSIZE(name), ImGuiInputTextFlags_AlwaysOverwrite);
                if (ImGui::IsItemEdited()) {
                    type.value = name;
                }

                break;
            }
            case BrakezaSetup::LUADataType::FLOAT: {
                float valueFloat = std::get<float>(type.value);
                if (ImGui::InputFloat(type.name.c_str(), &valueFloat, 0.01f, 1.0f, "%.3f")) {
                    type.value = valueFloat;
                }
                break;
            }
            case BrakezaSetup::LUADataType::VERTEX3D: {
                Vertex3D valueVertex = std::get<Vertex3D>(type.value);
                float vec4f[4];
                valueVertex.toFloat(vec4f);
                if (ImGui::DragFloat3(type.name.c_str(), vec4f, 0.01f, 0.0f, 1.0f)) {
                    valueVertex.x = vec4f[0];
                    valueVertex.y = vec4f[1];
                    valueVertex.z = vec4f[2];
                    type.value = valueVertex;
                }
                break;
            }
            default:
                std::cerr << "Unknown data type." << std::endl;
        }
        i++;
        ImGui::PopID();
    }
}

void ScriptLuaGUI::LoadScriptDialog(GUIManager *gui, const std::string& filename)
{
    delete gui->scriptEditableManager.script;
    gui->scriptEditableManager.selectedScriptFilename = filename;
    gui->scriptEditableManager.script = new ScriptLUA(filename,ScriptLUA::dataTypesFileFor(filename));
    strcpy(gui->scriptEditableManager.editableSource, gui->scriptEditableManager.script->content.c_str());
    gui->showEditScriptWindow = true;
}

void ScriptLuaGUI::DrawScriptsBySelectedObject(GUIManager *gui)
{
    bool hasSelectedIndex = gui->selectedObjectIndex >= 0 && gui->selectedObjectIndex < gui->gameObjects.size();

    if (!hasSelectedIndex) {
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", "No object selected");
        return;
    }

    auto o = gui->gameObjects[gui->selectedObjectIndex];
    auto objectScripts = o->getScripts();

    if (objectScripts.empty()) {
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", "No scripts in selected object.");
    }

    for (unsigned int i = 0; i < objectScripts.size(); i++) {
        auto currentScript = objectScripts[i];

        if (ImGui::ImageButton(FileSystemGUI::Icon(IconGUI::SCRIPT), ImVec2(14, 14))) {
            LoadScriptDialog(gui, currentScript->scriptFilename);
        }

        ImGui::SameLine();
        std::string optionText = std::to_string(i + 1) + ") " + Tools::removeSubstring(currentScript->scriptFilename, BrakezaSetup::get()->SCRIPTS_FOLDER);
        if (currentScript->isPaused()) {
            if (ImGui::ImageButton(FileSystemGUI::Icon(IconGUI::UNLOCK), ImVec2(14, 14))) {
                currentScript->setPaused(false);
            }
        } else {
            if (ImGui::ImageButton(FileSystemGUI::Icon(IconGUI::LOCK), ImVec2(14, 14))) {
                currentScript->setPaused(true);
            }
        }
        ImGui::SameLine();
        if (ImGui::ImageButton(FileSystemGUI::Icon(IconGUI::REMOVE), ImVec2(14, 14))) {
            o->RemoveScript(currentScript);
        }
        ImGui::SameLine();
        if (ImGui::CollapsingHeader(optionText.c_str())) {
            currentScript->drawImGuiProperties();
        }
    }
}

void ScriptLuaGUI::DrawScriptsLuaFolderFiles(GUIManager *gui, std::string& folder)
{
    static char name[256];
    strncpy(name, gui->currentVariableToAddName.c_str(), sizeof(name));
    if (ImGui::InputText("Script name##", name, IM_ARRAYSIZE(name), ImGuiInputTextFlags_AutoSelectAll)) {
        gui->currentVariableToAddName = name;
    }
    if (ImGui::ImageButton(FileSystemGUI::Icon(IconGUI::OPEN), ImVec2(14, 14))) {
        if (!gui->currentVariableToAddName.empty()) {
            auto browser = gui->getBrowserScripts();
            ComponentScripting::createScriptLUAFile(browser.currentFolder + gui->currentVariableToAddName);
            browser.folderFiles = Tools::getFolderFiles(browser.currentFolder, "lua");
        }
    }

    ImGui::Separator();
    auto browser = gui->getBrowserScripts();
    FileSystemGUI::DrawBrowserFolders(
        gui,
        folder,
        BrakezaSetup::get()->SCRIPTS_FOLDER,
        browser.currentFolder,
        browser.folderFolders,
        browser.folderFiles,
        "lua"
    );

    ImGui::Separator();

    auto files = browser.folderFiles;
    static ImGuiTableFlags flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingStretchProp;
    if (ImGui::BeginTable("ScriptsFolderTable", 2, flags)) {
        for (int i = 0; i < files.size(); i++) {
            ImGui::PushID(i);
            const auto& file = files[i];
            auto fullPath = folder + file;
            ImGui::TableNextRow();

            ImGui::TableSetColumnIndex(0);
            ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + 5.0f, ImGui::GetCursorPosY() + 5.0f));
            ImGui::Image(FileSystemGUI::Icon(IconGUI::SCRIPT), ImVec2(16, 16));
            ImGui::SameLine();
            std::string optionText = std::to_string(i + 1) + ") " + file;
            if (ImGui::Selectable(optionText.c_str())) {
                ScriptLuaGUI::LoadScriptDialog(gui, fullPath);
            }

            if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
                ImGui::SetDragDropPayload("SCRIPT_ITEM",fullPath.c_str(),fullPath.size() + 1);
                ImGui::Text("%s", fullPath.c_str());
                ImGui::EndDragDropSource();
            }
            ImGui::TableSetColumnIndex(1);
            if (ImGui::ImageButton(FileSystemGUI::Icon(IconGUI::REMOVE), ImVec2(14, 14))) {
                ImGui::OpenPopup("Deleting script");
            }
            GUIManager::ShowDeletePopup("Deleting script", "Are you sure to delete?", [folder, file, gui] () {
                ComponentScripting::removeScriptLUAFile(folder + file);
                FileSystemGUI::UpdateFolderFiles(gui);
            });
            ImGui::PopID();
        }
        ImGui::EndTable();
    }
}

void ScriptLuaGUI::DrawEditScriptWindow(GUIManager *gui)
{
    if (!gui->showEditScriptWindow) return;

    GUIManager::SetNextWindowSize(600, 600);
    ImGui::SetNextWindowBgAlpha(GUITypes::Levels::WINDOW_ALPHA);

    if (ImGui::Begin("Script edition", &gui->showEditScriptWindow, ImGuiWindowFlags_NoDocking)) {
        drawScriptVariables(gui);

        ImGui::Separator();

        if (ImGui::CollapsingHeader("Script code")) {
            static ImGuiInputTextFlags flags = ImGuiInputTextFlags_AllowTabInput;
            ImGui::InputTextMultiline("##source", gui->scriptEditableManager.editableSource, IM_ARRAYSIZE(gui->scriptEditableManager.editableSource), ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 25), flags);
            if (ImGui::Button(std::string("Save").c_str())) {
                gui->scriptEditableManager.script->updateScriptCodeWith(gui->scriptEditableManager.editableSource);
                gui->scriptEditableManager.script->reloadScriptCode();
            }
        }
    }
    ImGui::End();
}

void ScriptLuaGUI::DrawObjectVariables(const GUIManager *gui)
{
    bool hasSelectedIndex = gui->selectedObjectIndex >= 0 && gui->selectedObjectIndex < gui->gameObjects.size();

    if (!hasSelectedIndex) {
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", "No object selected");
    }

    if (hasSelectedIndex) {
        static ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg;
        if (ImGui::BeginTable("ObjectVariablesTable", 3, flags)) {
            auto o = gui->gameObjects[gui->selectedObjectIndex];
            auto scripts = o->getScripts();
            auto &luaEnvironment = o->getLuaEnvironment();
            auto &lua = ComponentsManager::get()->getComponentScripting()->getLua();

            int numVarFound = 0;
            for (auto &pair : luaEnvironment) {
                std::string key = pair.first.as<std::string>(); // Nombre de la variable
                sol::type valueType = pair.second.get_type();   // Tipo de la variable

                auto type = std::string(sol::type_name(lua, valueType));

                if (type == "number" || type == "string" || type == "boolean") {
                    numVarFound++;
                    ImGui::TableNextRow();

                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("%s", std::string(key).c_str());

                    ImGui::TableSetColumnIndex(1);
                    ImGui::Text("%s", std::string(sol::type_name(lua, valueType)).c_str());

                    ImGui::TableSetColumnIndex(2);
                    if (type == "number") {
                        ImGui::Text("%f", static_cast<float>(luaEnvironment[key]));
                    } else if (type == "string") {
                        ImGui::Text("%s", std::string(luaEnvironment[key]).c_str());
                    } else if (type == "boolean") {
                        ImGui::Text("%d", static_cast<bool>(luaEnvironment[key]));
                    }
                }
            }
            ImGui::EndTable();
            if (numVarFound <= 0) {
                ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", "The selected object has defined variables");
            }
        }
    }
}

void ScriptLuaGUI::DrawGlobalVariables()
{
    static ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg;
    if (ImGui::BeginTable("GlobalVariablesTable", 3, flags)) {
        auto &lua = ComponentsManager::get()->getComponentScripting()->getLua();

        sol::table globalTable = lua["_G"];  // Acceder a la tabla global

        int numVarFound = 0;
        for (auto &pair : globalTable) {
            std::string key = pair.first.as<std::string>(); // Nombre de la variable
            sol::type valueType = pair.second.get_type();   // Tipo de la variable
            //std::cout << "Variable: " << key << " | Tipo: " << sol::type_name(lua, valueType) << std::endl;

            auto type = std::string(sol::type_name(lua, valueType));

            if (type == "number" || type == "string" || type == "boolean") {
                numVarFound++;
                ImGui::TableNextRow();

                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%s", std::string(key).c_str());

                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%s", std::string(sol::type_name(lua, valueType)).c_str());

                ImGui::TableSetColumnIndex(2);
                ImGui::Text("%s", std::string(lua[key]).c_str());
            }
        }

        if (numVarFound <= 0) {
            ImGui::Text("%s", "There are not variables yet");
        }

        ImGui::EndTable();
    }
}

void ScriptLuaGUI::drawScriptVariables(GUIManager *gui)
{
    ImGui::SeparatorText(std::string("File: " + gui->scriptEditableManager.script->scriptFilename).c_str());

    static char name[256];

    strncpy(name, gui->currentVariableToAddName.c_str(), sizeof(name));

    if (ImGui::InputText("Variable name##", name, IM_ARRAYSIZE(name), ImGuiInputTextFlags_AutoSelectAll)) {
        gui->currentVariableToAddName = name;
    }

    const char* items[] = { "int", "float", "string", "Vertex3D" };
    static int selectedItem = 0;
    ImGui::Combo("Type", &selectedItem, items, IM_ARRAYSIZE(items));
    ImGui::SameLine();
    if (ImGui::Button(std::string("Create variable").c_str())) {
        if (!gui->currentVariableToAddName.empty()) {
            gui->scriptEditableManager.script->addDataTypeEmpty(gui->currentVariableToAddName.c_str(), items[selectedItem]);
        }
    }

    ImGui::SeparatorText("Variables");

    static ImGuiTableFlags flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingStretchProp;
    if (ImGui::BeginTable("ScriptProperties", 3, flags)) {
        for (int i = 0; i < gui->scriptEditableManager.script->dataTypes.size(); i++) {
            ImGui::TableNextRow();
            auto type = &gui->scriptEditableManager.script->dataTypes[i];

            ImGui::TableSetColumnIndex(0);
            ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + 5.0f, ImGui::GetCursorPosY() + 5.0f));
            ImGui::Text("%d) %s", i + 1, type->name.c_str());

            ImGui::TableSetColumnIndex(1);
            ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + 5.0f, ImGui::GetCursorPosY() + 5.0f));
            ImGui::Text("%s", type->type.c_str());

            ImGui::TableSetColumnIndex(2);
            if (ImGui::ImageButton(FileSystemGUI::Icon(IconGUI::REMOVE), ImVec2(14, 14))) {
                gui->scriptEditableManager.script->removeDataType(*type);
                gui->scriptEditableManager.script->updateFileTypes();
            }
        }
        ImGui::EndTable();
    }

    if (gui->scriptEditableManager.script->dataTypes.empty()) {
        ImGui::Text("No types defined");
        ImGui::Spacing();
    } else {
        if (ImGui::Button(std::string("Save script types").c_str())) {
            gui->scriptEditableManager.script->updateFileTypes();
        }
    }
}
