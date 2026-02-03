//
// Created by Eduardo on 26/11/2025.
//

#include "../../../include/Brakeza.h"
#include "../../../include/GUI/GUIManager.h"
#include "../../../include/GUI/Objects/ScriptLuaGUI.h"
#include "../../../include/GUI/Objects/FileSystemGUI.h"
#include "../../../include/GUI/Editable/EditableOpenScriptFile.h"
#include "../../../include/GUI/AddOns/CustomTreeNode.h"
#include "../../../include/Components/Components.h"
#include "../../../include/Render/Drawable.h"

void ScriptLuaGUI::DrawPropertiesGUI(ScriptLUA *o)
{
    ImGui::Spacing();

    ImGui::SeparatorText("Script variables values");

    static ImGuiTableFlags flags = ImGuiTableFlags_SizingStretchProp |
                                ImGuiTableFlags_BordersOuter |
                                ImGuiTableFlags_BordersInnerV;

    ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(8, 6));
    if (ImGui::BeginTable("ScriptVariables", 2, flags)) {
        if (!o->dataTypes.empty()) {
            int i = 0;
            for (auto& type: o->dataTypes) {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::AlignTextToFramePadding();
                ImGui::Text("%s", type.name.c_str());

                ImGui::TableSetColumnIndex(1);
                ImGui::PushID(i);
                DrawTypeImGuiControl(type, false, true);
                ImGui::PopID();

                i++;
            }
        } else {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            Drawable::WarningMessage("Script with no variables");
        }
        ImGui::EndTable();
    }
    ImGui::PopStyleVar();

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
    auto GUI = Brakeza::get()->GUI();
    std::string uniqueId = "file:" + pathFile;

    if (!GUI->isEditableFileAlreadyOpen(uniqueId)) {
        GUI->OpenEditableFile(
            new EditableOpenScriptFile(
                uniqueId,
                meta.name,
                meta.codeFile,
                new ScriptLUA(meta.name, meta.codeFile, meta.typesFile)
            )
        );
    }
    GUI->setIndexCodeEditorTab(uniqueId);
    Components::get()->Window()->setImGuiConfig(Config::ImGUIConfigs::CODING);
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
        return;
    }

    for (unsigned int i = 0; i < objectScripts.size(); i++) {
        auto currentScript = objectScripts[i];

        // Configurar CustomTreeNode
        std::string name = currentScript->getName();
        CustomImGui::CustomTreeNodeConfig config(name.c_str());

        config.leftIcon = FileSystemGUI::Icon(IconGUI::SCRIPT_FILE);
        config.iconSize = GUIType::Sizes::ICONS_OBJECTS_ALLOWED;

        // 🎯 Añadir bullets
        config.bulletOpen = FileSystemGUI::Icon(IconGUI::TREE_BULLET_ON);
        config.bulletClosed = FileSystemGUI::Icon(IconGUI::TREE_BULLET_OFF);
        config.bulletSize = ImVec2(18, 18);

        config.itemPadding = 1.0f;
        config.indentSpacing = 20.0f;
        config.defaultOpen = false;

        // Lock/Unlock
        CustomImGui::TreeActionItem lockItem(
            currentScript->isPaused() ? FileSystemGUI::Icon(IconGUI::LUA_LOCK) : FileSystemGUI::Icon(IconGUI::LUA_UNLOCK),
            currentScript->isPaused() ? "Unlock script object" : "Lock script object",
            [currentScript]() { currentScript->setPaused(!currentScript->isPaused()); }
        );
        lockItem.size = GUIType::Sizes::ICONS_BROWSERS;
        config.actionItems.push_back(lockItem);

        // Reload
        CustomImGui::TreeActionItem editItem(
            FileSystemGUI::Icon(IconGUI::SCRIPT_RELOAD),
            "Reload script object",
            [currentScript]() { }
        );
        editItem.size = GUIType::Sizes::ICONS_BROWSERS;
        config.actionItems.push_back(editItem);

        // Remove
        CustomImGui::TreeActionItem removeItem(
            FileSystemGUI::Icon(IconGUI::LUA_REMOVE),
            "Remove script object",
            [o, currentScript]() { o->RemoveScript(currentScript); }
        );
        removeItem.size = GUIType::Sizes::ICONS_BROWSERS;
        config.actionItems.push_back(removeItem);

        // Dibujar el nodo
        bool isOpen = CustomImGui::CustomTreeNode(config);

        if (isOpen) {
            currentScript->DrawImGuiProperties();
            CustomImGui::CustomTreePop(config.indentSpacing);
        }
    }
}

void ScriptLuaGUI::DrawTypeImGuiControl(ScriptLUATypeData &type, bool showName, bool showIcon)
{
    // Preparar el label
    std::string label = showName ? type.name : ("##" + type.name);

    // Si no mostramos el nombre, forzar el ancho completo
    if (!showName) {
        ImGui::SetNextItemWidth(-FLT_MIN);  // Ocupa todo el ancho disponible
    }

    switch (LUADataTypesMapping[type.type].type) {
        case LUADataType::INT: {
            int valueInt = std::get<int>(type.value);
            if (ImGui::InputInt(label.c_str(), &valueInt)) {
                type.value = valueInt;
            }
            break;
        }
        case LUADataType::STRING: {
            std::string valueString = std::get<const char*>(type.value);
            static char name[256];
            strncpy(name, valueString.c_str(), sizeof(name));
            if (ImGui::InputText(label.c_str(), name, IM_ARRAYSIZE(name), ImGuiInputTextFlags_AlwaysOverwrite)) {
                type.value = name;
            }
            break;
        }
        case LUADataType::FLOAT: {
            float valueFloat = std::get<float>(type.value);
            if (ImGui::InputFloat(label.c_str(), &valueFloat, 0.01f, 1.0f, "%.3f")) {
                type.value = valueFloat;
            }
            break;
        }
        case LUADataType::VERTEX3D: {
            Vertex3D valueVertex = std::get<Vertex3D>(type.value);
            float vec4f[4];
            valueVertex.toFloat(vec4f);
            if (ImGui::DragFloat3(label.c_str(), vec4f, 0.01f, 0.0f, 1.0f)) {
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
    ImGui::Text(std::string("File: " + file.getScript()->getScriptFilename()).c_str());
}

void ScriptLuaGUI::DrawScriptConfigEditName(EditableOpenScriptFile &file)
{
    ImGui::Separator();
    auto shader= file.getScript();

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
    GUI::ImageButtonNormal(IconGUI::SCRIPT_CREATE_VARIABLE, "Create variable", [&] {
        if (localVarName[0] != '\0') {
            file.getScript()->AddDataTypeEmpty(localVarName, itemsCStr[selectedItem]);
        }
    });
}

void ScriptLuaGUI::DrawScriptConfigVarsTable(EditableOpenScriptFile &file)
{
    ImGui::SeparatorText("Script variables");

    auto shader = file.getScript();

    if (shader->dataTypes.empty()) {
        Drawable::WarningMessage("No variables defined");
        return;
    }

    // Calcular el ancho del botón
    float buttonWidth = GUIType::Sizes::ICONS_BROWSERS.x + ImGui::GetStyle().FramePadding.x * 2;

    // Calcular el ancho máximo de los labels
    float maxLabelWidth = 0.0f;
    for (int i = 0; i < shader->dataTypes.size(); i++) {
        auto type = &shader->dataTypes[i];
        std::string label = std::to_string(i + 1) + ") " + type->name + " (" + LUADataTypesMapping[type->type].label + ")";
        float labelWidth = ImGui::CalcTextSize(label.c_str()).x;
        maxLabelWidth = ImMax(maxLabelWidth, labelWidth);
    }

    // Ancho total de la parte izquierda: botón + spacing + texto + margen
    float leftSideWidth = buttonWidth + ImGui::GetStyle().ItemSpacing.x + maxLabelWidth + 30.0f;

    for (int i = 0; i < shader->dataTypes.size(); i++) {
        auto type = &shader->dataTypes[i];

        ImGui::PushID(i);

        ImGui::AlignTextToFramePadding();

        // Botón eliminar
        GUI::DrawButton("Delete script variable", IconGUI::SCRIPT_REMOVE_VARIABLE, GUIType::Sizes::ICONS_BROWSERS, true, [&] {
            file.getScript()->RemoveDataType(*type);
            file.getScript()->UpdateFileTypes();
        });

        ImGui::SameLine();

        // Número y nombre
        ImGui::Text("%d) %s", i + 1, type->name.c_str());
        ImGui::SameLine();

        // Tipo
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "(%s)", LUADataTypesMapping[type->type].label.c_str());

        // Posicionar el control en la posición calculada
        ImGui::SameLine(leftSideWidth);

        // Control de valor ocupa el resto del espacio disponible
        ImGui::SetNextItemWidth(-FLT_MIN);
        DrawTypeImGuiControl(*type, false, true);

        ImGui::PopID();
    }
}

void ScriptLuaGUI::DrawScriptConfigActionButtons(EditableOpenScriptFile &file)
{
    GUI::ImageButtonNormal(IconGUI::SCRIPT_SAVE, "Save variables config", [&] {
        file.getScript()->UpdateFileTypes();
    });
}