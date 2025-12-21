//
// Created by Eduardo on 26/11/2025.
//

#include "../../../include/Brakeza.h"
#include "../../../include/GUI/GUIManager.h"
#include "../../../include/GUI/Objects/ScriptLuaGUI.h"
#include "../../../include/GUI/Objects/FileSystemGUI.h"
#include "../../../include/GUI/TextEditor/EditableOpenScriptFile.h"
#include "../../../include/Components/Components.h"
#include "../../../include/Render/Drawable.h"

void ScriptLuaGUI::DrawPropertiesGUI(ScriptLUA *o)
{
    ImGui::Spacing();

    ImGui::SeparatorText("Script variables");

    if (o->dataTypes.empty()) {
        Drawable::WarningMessage("Script with no variables");
        return;
    }
    int i = 0;
    for (auto& type: o->dataTypes) {
        ImGui::PushID(i);
        DrawTypeImGuiControl(type, true, true);
        i++;
        ImGui::PopID();
    }
    ImGui::Spacing();
}

ScriptMetaInfo ScriptLuaGUI::ExtractScriptMetainfo(const std::string& pathFile)
{
    auto json = cJSON_Parse(Tools::ReadFile(pathFile));

    return {
        cJSON_GetObjectItemCaseSensitive(json, "name")->valuestring,
        cJSON_GetObjectItemCaseSensitive(json, "codeFile")->valuestring,
        cJSON_GetObjectItemCaseSensitive(json, "typesFile")->valuestring
    };
}

void ScriptLuaGUI::LoadScriptDialog(const std::string& pathFile)
{
    auto meta = ExtractScriptMetainfo(pathFile);

    if (!Brakeza::get()->GUI()->isEditableFileAlreadyOpen(meta.name)) {
        Brakeza::get()->GUI()->OpenEditableFile(
            new EditableOpenScriptFile(
                meta.name,
                meta.codeFile,
                new ScriptLUA(meta.name, meta.codeFile, meta.typesFile)
            )
        );
    }
    Brakeza::get()->GUI()->setIndexCodeEditorTab(meta.name);
}

void ScriptLuaGUI::DrawWinObjectScripts()
{
    auto windowStatus = Brakeza::get()->GUI()->getWindowStatus(GUIType::OBJECT_SCRIPTS);
    if (!windowStatus->isOpen) return;

    auto o = Components::get()->Render()->getSelectedObject();
    if (o == nullptr) {
        Drawable::WarningMessage("No object selected");
        return;
    }

    auto objectScripts = o->getScripts();

    if (objectScripts.empty()) {
        Drawable::WarningMessage("No scripts in selected object");
    }

    if (ImGui::BeginTable("SceneShadersTable", 2, ImGuiTableFlags_None | ImGuiTableFlags_RowBg)) {
        ImGui::TableSetupColumn("Shader");
        ImGui::TableSetupColumn("Actions", ImGuiTableColumnFlags_WidthFixed);

        for (unsigned int i = 0; i < objectScripts.size(); i++) {
            auto currentScript = objectScripts[i];
            ImGui::PushID(std::string("##object_scripts_" + std::to_string(i)).c_str());
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::Image(FileSystemGUI::Icon(IconGUI::SCRIPT_FILE), GUIType::Sizes::ICONS_OBJECTS_ALLOWED);
            ImGui::SameLine(0, 5.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 4));
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(2, 2));
            std::string name = std::to_string(i + 1) + ") " + currentScript->getName();
            bool isOpenCurrentScript = ImGui::TreeNodeEx(name.c_str(), ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_FramePadding);
            ImGui::PopStyleVar(2);
            if (isOpenCurrentScript) {
                currentScript->drawImGuiProperties();
                ImGui::TreePop();
            }

            // Buttons
            ImGui::TableNextColumn();
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 4));    // padding inner button
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 2));     // spacing between buttonms

            GUI::DrawButtonTransparent(
                currentScript->isPaused() ? "Unlock script object" : "Lock script object",
                currentScript->isPaused() ? IconGUI::LUA_LOCK : IconGUI::LUA_UNLOCK,
                GUIType::Sizes::ICONS_BROWSERS,
                false,
                [&] { currentScript->setPaused(!currentScript->isPaused()); }
            );
            ImGui::SameLine();
            GUI::DrawButtonTransparent("Edit script", IconGUI::SCRIPT_EDIT, GUIType::Sizes::ICONS_BROWSERS, false, [&] {
                LoadScriptDialog(currentScript->scriptFilename);
            });
            ImGui::SameLine();
            GUI::DrawButtonTransparent("Remove script object", IconGUI::LUA_REMOVE, GUIType::Sizes::ICONS_BROWSERS, false, [&] {
                o->RemoveScript(currentScript);
            });

            ImGui::PopStyleVar(2);
            ImGui::PopID();
        }
    }
    ImGui::EndTable();
}

void ScriptLuaGUI::DrawTypeImGuiControl(ScriptLUATypeData &type, bool showName, bool showIcon)
{
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
}

void ScriptLuaGUI::DrawScriptConfig(EditableOpenScriptFile &file)
{
    DrawScriptConfigHeader(file);
    DrawScriptConfigEditName(file);
    DrawScriptConfigVarCreator(file);
    DrawScriptConfigVarsTable(file);
    DrawScriptConfigEmptyStateWarning(file);
    ImGui::Separator();
    DrawScriptConfigActionButtons(file);
}

int ScriptLuaGUI::getNumAllowedVars(Object3D* o)
{
    auto sceneObjects = Brakeza::get()->getSceneObjects();

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
        }
    }

    return numVarFound;
}

void ScriptLuaGUI::DrawWinObjectVars(GUIManager *gui)
{
    auto windowStatus = gui->getWindowStatus(GUIType::OBJECT_VARS);
    if (!windowStatus->isOpen) return;

    auto sceneObjects = Brakeza::get()->getSceneObjects();

    auto o = Components::get()->Render()->getSelectedObject();

    if (o == nullptr) {
        Drawable::WarningMessage("No object selected");
        return;
    }

    if (getNumAllowedVars(o) > 0) {
        static ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg;
        if (ImGui::BeginTable("ObjectVariablesTable", 3, flags)) {
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

void ScriptLuaGUI::DrawScriptConfigHeader(EditableOpenScriptFile &file)
{
    ImGui::Image(FileSystemGUI::Icon(IconGUI::SCRIPT_FILE), GUIType::Sizes::ICONS_BROWSERS);
    ImGui::SameLine();
    ImGui::Text(std::string("File: " + file.getShader()->getScriptFilename()).c_str());
}

void ScriptLuaGUI::DrawScriptConfigEditName(EditableOpenScriptFile &file)
{
    ImGui::Separator();
    auto shader= file.getShader();

    auto label = shader->getName();
    static char name[256];
    strncpy(name, label.c_str(), sizeof(name));
    if (ImGui::InputText("Name##", name, IM_ARRAYSIZE(name), ImGuiInputTextFlags_AutoSelectAll)) {
        shader->setName(name);
    }
}

void ScriptLuaGUI::DrawScriptConfigVarCreator(EditableOpenScriptFile &file)
{
    ImGui::SeparatorText("Create new variable");
    static char localVarName[256] = "";
    ImGui::InputText("Variable name##", localVarName, IM_ARRAYSIZE(localVarName), ImGuiInputTextFlags_AutoSelectAll);

    std::vector<std::string> items;
    for (auto &t : LUADataTypesMapping) {
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
            if (localVarName[0] != '\0') {
                file.getShader()->addDataTypeEmpty(localVarName, itemsCStr[selectedItem]);
            }
        }
    );
}

void ScriptLuaGUI::DrawScriptConfigVarsTable(EditableOpenScriptFile &file)
{
    ImGui::SeparatorText("Script variables");

    auto shader = file.getShader();

    static ImGuiTableFlags flags = ImGuiTableFlags_RowBg;
    if (ImGui::BeginTable("ScriptProperties", 4, flags)) {
        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("Actions", ImGuiTableColumnFlags_WidthFixed);

        for (int i = 0; i < shader->dataTypes.size(); i++) {
            ImGui::PushID(std::string("script_table_var_" + std::to_string(i)).c_str());
            ImGui::TableNextRow();
            auto type = &shader->dataTypes[i];

            ImGui::TableSetColumnIndex(0);
            ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + 5.0f, ImGui::GetCursorPosY() + 5.0f));
            ImGui::Text("%d) %s", i + 1, type->name.c_str());

            ImGui::TableSetColumnIndex(1);
            ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + 5.0f, ImGui::GetCursorPosY() + 5.0f));
            ImGui::Text("%s", LUADataTypesMapping[type->type].label.c_str());

            ImGui::TableSetColumnIndex(2);
            ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + 5.0f, ImGui::GetCursorPosY() + 5.0f));
            DrawTypeImGuiControl(*type, true, true);

            ImGui::TableSetColumnIndex(3);
            GUI::DrawButton("Delete script variable", IconGUI::SCRIPT_REMOVE_VARIABLE, GUIType::Sizes::ICONS_BROWSERS, true, [&] {
                file.getShader()->removeDataType(*type);
                file.getShader()->updateFileTypes();
            });
            ImGui::PopID();
        }
        ImGui::EndTable();
    }
}

void ScriptLuaGUI::DrawScriptConfigEmptyStateWarning(EditableOpenScriptFile &file)
{
    if (file.getShader()->dataTypes.empty()) {
        Drawable::WarningMessage("No variables defined");
    }
}

void ScriptLuaGUI::DrawScriptConfigActionButtons(EditableOpenScriptFile &file)
{
    GUI::DrawButton("Save script to disk", IconGUI::SCRIPT_SAVE, GUIType::Sizes::ICONS_CODE_EDITOR, true, [&] {
        file.getShader()->updateFileTypes();
    });
}

