//
// Created by Eduardo on 26/11/2025.
//

#include "../include/GUI/Objects/ScriptLuaGUI.h"

#include "../../../include/Brakeza.h"
#include "../../../include/Components/Components.h"
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
        switch (LUADataTypesMapping[type.type].type) {
            case LUADataType::INT: {
                int valueInt = std::get<int>(type.value);
                if (ImGui::InputInt(type.name.c_str(), &valueInt)) {
                    type.value = valueInt;
                }
                break;
            }
            case LUADataType::STRING: {
                std::string valueString = std::get<const char*>(type.value);
                static char name[256];
                strncpy(name, valueString.c_str(), sizeof(name));
                ImGui::InputText(type.name.c_str(), name, IM_ARRAYSIZE(name), ImGuiInputTextFlags_AlwaysOverwrite);
                if (ImGui::IsItemEdited()) {
                    type.value = name;
                }

                break;
            }
            case LUADataType::FLOAT: {
                float valueFloat = std::get<float>(type.value);
                if (ImGui::InputFloat(type.name.c_str(), &valueFloat, 0.01f, 1.0f, "%.3f")) {
                    type.value = valueFloat;
                }
                break;
            }
            case LUADataType::VERTEX3D: {
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

void ScriptLuaGUI::DrawWinObjectScripts(GUIManager *gui)
{
    auto windowStatus = gui->getWindowStatus(GUIType::OBJECT_SCRIPTS);
    if (!windowStatus->isOpen) return;

    auto sceneObjects = Brakeza::get()->getSceneObjects();
    bool hasSelectedIndex = gui->selectedObjectIndex >= 0 && gui->selectedObjectIndex < sceneObjects.size();

    if (!hasSelectedIndex) {
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", "No object selected");
        return;
    }

    auto o = sceneObjects[gui->selectedObjectIndex];
    auto objectScripts = o->getScripts();

    if (objectScripts.empty()) {
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", "No scripts in selected object.");
    }

    for (unsigned int i = 0; i < objectScripts.size(); i++) {
        auto currentScript = objectScripts[i];

        GUI::DrawButtonTransparent(
            currentScript->isPaused() ? "Unlock script object" : "Lock script object",
            currentScript->isPaused() ? IconGUI::LUA_LOCK : IconGUI::LUA_UNLOCK,
            GUIType::Sizes::ICONS_BROWSERS,
            false,
            [&] { currentScript->setPaused(!currentScript->isPaused()); }
        );
        ImGui::SameLine();
        GUI::DrawButton("Edit script", IconGUI::SCRIPT_EDIT, GUIType::Sizes::ICONS_BROWSERS, false, [&] {
            LoadScriptDialog(gui, currentScript->scriptFilename);
        });

        ImGui::SameLine();
        GUI::DrawButton("Remove script object", IconGUI::LUA_REMOVE, GUIType::Sizes::ICONS_BROWSERS, false, [&] {
            o->RemoveScript(currentScript);
        });
        ImGui::SameLine();
        std::string name = std::to_string(i + 1) + ") " + currentScript->getName();
        if (ImGui::CollapsingHeader(name.c_str())) {
            currentScript->drawImGuiProperties();
        }
    }
}

void ScriptLuaGUI::DrawEditScriptWindow(GUIManager *gui)
{
    if (!gui->showEditScriptWindow) return;

    GUIManager::SetNextWindowSize(600, 600);
    ImGui::SetNextWindowBgAlpha(GUIType::Levels::WINDOW_ALPHA);

    if (ImGui::Begin("Script edition", &gui->showEditScriptWindow, ImGuiWindowFlags_NoDocking)) {
        DrawWinScriptEdition(gui);

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

void ScriptLuaGUI::DrawWinObjectVars(GUIManager *gui)
{
    auto windowStatus = gui->getWindowStatus(GUIType::OBJECT_VARS);
    if (!windowStatus->isOpen) return;

    auto sceneObjects = Brakeza::get()->getSceneObjects();

    bool hasSelectedIndex = gui->selectedObjectIndex >= 0 && gui->selectedObjectIndex < sceneObjects.size();

    if (!hasSelectedIndex) {
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", "No object selected");
    }

    if (hasSelectedIndex) {
        static ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg;
        if (ImGui::BeginTable("ObjectVariablesTable", 3, flags)) {
            auto o = sceneObjects[gui->selectedObjectIndex];
            auto scripts = o->getScripts();
            auto &luaEnvironment = o->getLuaEnvironment();
            auto &lua = Components::get()->Scripting()->getLua();

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

void ScriptLuaGUI::DrawWinGlobalVars(GUIManager *gui)
{
    auto windowStatus = gui->getWindowStatus(GUIType::GLOBAL_VARS);
    if (!windowStatus->isOpen) return;

    static ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg;
    if (ImGui::BeginTable("GlobalVariablesTable", 3, flags)) {
        auto &lua = Components::get()->Scripting()->getLua();
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

void ScriptLuaGUI::DrawWinScriptEdition(GUIManager *gui)
{
    DrawScriptHeader(gui);
    DrawScriptConfiguration(gui);
    DrawVariableCreator(gui);
    DrawVariablesTable(gui);
    DrawEmptyStateWarning(gui);
    DrawSaveButton(gui);
}

void ScriptLuaGUI::DrawScriptHeader(GUIManager *gui)
{
    ImGui::Image(FileSystemGUI::Icon(IconGUI::SCRIPT_FILE), GUIType::Sizes::ICONS_BROWSERS);
    ImGui::SameLine();
    ImGui::Text(std::string("File: " + gui->scriptEditableManager.script->scriptFilename).c_str());
}

void ScriptLuaGUI::DrawScriptConfiguration(GUIManager *gui)
{
    auto label = gui->scriptEditableManager.script->getName();
    ImGui::SeparatorText("Script configuration");
    static char name[256];
    strncpy(name, label.c_str(), sizeof(name));
    if (ImGui::InputText("Name##", name, IM_ARRAYSIZE(name), ImGuiInputTextFlags_AutoSelectAll)) {
        gui->scriptEditableManager.script->setName(name);
    }
}

void ScriptLuaGUI::DrawVariableCreator(GUIManager *gui)
{
    ImGui::SeparatorText("Variables");

    static char varName[256];
    strncpy(varName, gui->currentVariableToAddName.c_str(), sizeof(varName));
    if (ImGui::InputText("Variable name##", varName, IM_ARRAYSIZE(varName), ImGuiInputTextFlags_AutoSelectAll)) {
        gui->currentVariableToAddName = varName;
    }
    std::vector<std::string> items;
    for (auto t : LUADataTypesMapping) {
        items.push_back(t.second.label);
    }

    std::vector<const char*> itemsCStr;
    for (auto& item : items) itemsCStr.push_back(item.c_str());

    static int selectedItem = 0;
    ImGui::Combo("Type", &selectedItem, itemsCStr.data(), itemsCStr.size());
    ImGui::SameLine();
    GUI::DrawButton(
        "Create variable",
        IconGUI::SCRIPT_CREATE_VARIABLE,
        GUIType::Sizes::ICONS_BROWSERS,
        false,
        [&] {
            if (!gui->currentVariableToAddName.empty()) {
                gui->scriptEditableManager.script->addDataTypeEmpty(gui->currentVariableToAddName.c_str(), itemsCStr[selectedItem]);
            }
        }
    );
}

void ScriptLuaGUI::DrawVariablesTable(GUIManager *gui)
{
    ImGui::SeparatorText("Variables");
    static ImGuiTableFlags flags = ImGuiTableFlags_RowBg;
    if (ImGui::BeginTable("ScriptProperties", 3, flags)) {
        // Configurar columnas - igual que en proyectos
        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("Actions", ImGuiTableColumnFlags_WidthFixed);

        for (int i = 0; i < gui->scriptEditableManager.script->dataTypes.size(); i++) {
            ImGui::TableNextRow();
            auto type = &gui->scriptEditableManager.script->dataTypes[i];

            ImGui::TableSetColumnIndex(0);
            ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + 5.0f, ImGui::GetCursorPosY() + 5.0f));
            ImGui::Text("%d) %s", i + 1, type->name.c_str());

            ImGui::TableSetColumnIndex(1);
            ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + 5.0f, ImGui::GetCursorPosY() + 5.0f));
            ImGui::Text("%s", LUADataTypesMapping[type->type].label.c_str());

            ImGui::TableSetColumnIndex(2);
            GUI::DrawButton("Delete script variable", IconGUI::SCRIPT_REMOVE_VARIABLE, GUIType::Sizes::ICONS_BROWSERS, true, [&] {
                gui->scriptEditableManager.script->removeDataType(*type);
                gui->scriptEditableManager.script->updateFileTypes();
            });
        }
        ImGui::EndTable();
    }
}

void ScriptLuaGUI::DrawEmptyStateWarning(GUIManager *gui)
{
    if (gui->scriptEditableManager.script->dataTypes.empty()) {
        ImGui::Image(FileSystemGUI::Icon(IconGUI::WARNING), GUIType::Sizes::ICONS_BROWSERS);
        ImGui::SameLine();
        ImGui::Text("No types defined");
        ImGui::Spacing();
    }
}

void ScriptLuaGUI::DrawSaveButton(GUIManager *gui)
{
    GUI::DrawButton("Save SCRIPT", IconGUI::SCRIPT_SAVE, GUIType::Sizes::ICONS_BROWSERS, true, [&] {
        gui->scriptEditableManager.script->updateFileTypes();
    });
}

