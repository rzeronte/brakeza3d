//
// Created by Eduardo on 05/12/2025.
//

#ifndef BRAKEZA3D_ICONSGUI_H
#define BRAKEZA3D_ICONSGUI_H

#include "FileSystemGUI.h"
#include "../../Config.h"
#include "../../Misc/cJSON.h"
#include "../../Misc/Logging.h"
#include "../../Misc/Tools.h"
#include "../../../include/GUI/GUIManager.h"

class IconsGUI {
public:
    static void ExportIconsToJSON(const std::string& filename)
    {
        cJSON* root = cJSON_CreateObject();

        // ===== Exportar GUI Icons =====
        cJSON* guiIcons = cJSON_CreateArray();
        for (size_t i = 0; i < IconGUI::ICON_COUNT; i++) {
            cJSON* iconObj = cJSON_CreateObject();
            cJSON_AddStringToObject(iconObj, "name", IconGUI::ALL_ICONS_EDITOR[i].name);
            cJSON_AddNumberToObject(iconObj, "x", IconGUI::ALL_ICONS_EDITOR[i].icon->x);
            cJSON_AddNumberToObject(iconObj, "y", IconGUI::ALL_ICONS_EDITOR[i].icon->y);
            cJSON_AddItemToArray(guiIcons, iconObj);
        }
        cJSON_AddItemToObject(root, "gui_icons", guiIcons);

        // ===== Exportar Object Icons =====
        cJSON* objectIcons = cJSON_CreateArray();
        for (size_t i = 0; i < IconObject::ICON_COUNT; i++) {
            cJSON* iconObj = cJSON_CreateObject();
            cJSON_AddStringToObject(iconObj, "name", IconObject::ALL_ICONS_EDITOR[i].name);
            cJSON_AddNumberToObject(iconObj, "x", IconObject::ALL_ICONS_EDITOR[i].icon->x);
            cJSON_AddNumberToObject(iconObj, "y", IconObject::ALL_ICONS_EDITOR[i].icon->y);
            cJSON_AddItemToArray(objectIcons, iconObj);
        }
        cJSON_AddItemToObject(root, "object_icons", objectIcons);

        char* jsonString = cJSON_Print(root);
        Tools::WriteToFile(filename, cJSON_Print(root));

        cJSON_free(jsonString);
        cJSON_Delete(root);
    }

    static void DrawDebugIconsWindow(GUIManager *gui)
    {
        auto windowStatus = gui->getWindowStatus(GUIType::DEBUG_ICONS);
        if (!windowStatus->isOpen) return;

        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "WARNING: Changes affect the entire application in REAL-TIME");
        ImGui::Separator();

        constexpr int COLUMNS_PER_ROW = 6;

        // Helper para centrar imagen
        auto CenterImage = [](ImTextureID texture, const ImVec2& size) {
            float availWidth = ImGui::GetContentRegionAvail().x;
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (availWidth - size.x) * 0.5f);
            ImGui::Image(texture, size);
        };

        // Helper para centrar texto
        auto CenterText = [](const char* text) {
            float availWidth = ImGui::GetContentRegionAvail().x;
            float textWidth = ImGui::CalcTextSize(text).x;
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (availWidth - textWidth) * 0.5f);
            ImGui::Text("%s", text);
        };

        // ===== ICONOS GUI =====
        if (ImGui::CollapsingHeader("GUI Icons")) {
            ImGui::Text("Total: %zu icons", IconGUI::ICON_COUNT);

            if (ImGui::BeginTable("IconGUITable", COLUMNS_PER_ROW, ImGuiTableFlags_Borders | ImGuiTableFlags_SizingStretchSame)) {
                size_t iconIndex = 0;
                while (iconIndex < IconGUI::ICON_COUNT) {
                    ImGui::TableNextRow();

                    for (int col = 0; col < COLUMNS_PER_ROW && iconIndex < IconGUI::ICON_COUNT; col++, iconIndex++) {
                        ImGui::TableNextColumn();
                        ImGui::PushID(static_cast<int>(iconIndex + 1000));
                        ImGui::Dummy(ImVec2(0.0f, 6.0f));

                        auto& icon = *IconGUI::ALL_ICONS_EDITOR[iconIndex].icon;

                        // Centrar imagen
                        CenterImage(FileSystemGUI::Icon(icon), ImVec2(32, 32));

                        // Centrar nombre
                        CenterText(IconGUI::ALL_ICONS_EDITOR[iconIndex].name);

                        ImGui::Separator();

                        // Fila X: centrar todo el grupo (texto + botones)
                        char xLabel[32];
                        snprintf(xLabel, sizeof(xLabel), "X %d", icon.x);
                        float xLabelWidth = ImGui::CalcTextSize(xLabel).x;
                        float buttonWidth = ImGui::CalcTextSize("-").x + ImGui::GetStyle().FramePadding.x * 2;
                        float totalWidth = xLabelWidth + buttonWidth * 2 + ImGui::GetStyle().ItemSpacing.x * 2;
                        float availWidth = ImGui::GetContentRegionAvail().x;

                        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (availWidth - totalWidth) * 0.5f);
                        int idx = gui->getTextureAtlas()->getIndexByXY(icon.x, icon.y);
                        if (ImGui::InputInt("Index", &idx, 1,  100)) {
                            if (idx > 0 && idx < gui->getTextureAtlas()->getTotalImages())
                                gui->getTextureAtlas()->setXYByIndex(idx, icon.x, icon.y);
                        }
                        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (availWidth - totalWidth) * 0.5f);
                        ImGui::Text("%s", xLabel);
                        ImGui::SameLine();
                        if (ImGui::SmallButton("<##X")) {
                            if (icon.x > 0) icon.x--;
                        }
                        ImGui::SameLine();
                        if (ImGui::SmallButton(">##X")) {
                            if (icon.x < gui->getTextureAtlas()->getNumColumns()) icon.x++;
                        }

                        char yLabel[32];
                        snprintf(yLabel, sizeof(yLabel), "Y %d", icon.y);
                        float yLabelWidth = ImGui::CalcTextSize(yLabel).x;
                        totalWidth = yLabelWidth + buttonWidth * 2 + ImGui::GetStyle().ItemSpacing.x * 2;

                        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (availWidth - totalWidth) * 0.5f);
                        ImGui::Text("%s", yLabel);
                        ImGui::SameLine();
                        if (ImGui::SmallButton("<##Y")) {
                            if (icon.y > 0) icon.y--;
                        }
                        ImGui::SameLine();
                        if (ImGui::SmallButton(">##Y")) {
                            if (icon.y < gui->getTextureAtlas()->getNumRows()) icon.y++;
                        }
                        ImGui::Dummy(ImVec2(0.0f, 6.0f));
                        ImGui::PopID();
                    }
                }
                ImGui::EndTable();
            }
            if (ImGui::Button("Reset GUI Icons", ImVec2(200, 0))) {
                IconGUI::ResetToDefault();
            }
        }
        ImGui::Separator();
        // ===== ICONOS OBJECT =====
        if (ImGui::CollapsingHeader("Object Icons")) {
            ImGui::Text("Total: %zu icons", IconObject::ICON_COUNT);

            if (ImGui::BeginTable("IconObjectTable", COLUMNS_PER_ROW, ImGuiTableFlags_Borders | ImGuiTableFlags_SizingStretchSame)) {
                size_t iconIndex = 0;
                while (iconIndex < IconObject::ICON_COUNT) {
                    ImGui::TableNextRow();

                    for (int col = 0; col < COLUMNS_PER_ROW && iconIndex < IconObject::ICON_COUNT; col++, iconIndex++) {
                        ImGui::Dummy(ImVec2(0.0f, 6.0f));
                        ImGui::TableNextColumn();
                        ImGui::PushID(static_cast<int>(iconIndex + 2000));

                        auto& icon = *IconObject::ALL_ICONS_EDITOR[iconIndex].icon;

                        // Centrar imagen
                        CenterImage(FileSystemGUI::Icon(icon), ImVec2(32, 32));

                        // Centrar nombre
                        CenterText(IconObject::ALL_ICONS_EDITOR[iconIndex].name);

                        ImGui::Separator();

                        // Fila X
                        char xLabel[32];
                        snprintf(xLabel, sizeof(xLabel), "X %d", icon.x);
                        float xLabelWidth = ImGui::CalcTextSize(xLabel).x;
                        float buttonWidth = ImGui::CalcTextSize("-").x + ImGui::GetStyle().FramePadding.x * 2;
                        float totalWidth = xLabelWidth + buttonWidth * 2 + ImGui::GetStyle().ItemSpacing.x * 2;
                        float availWidth = ImGui::GetContentRegionAvail().x;

                        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (availWidth - totalWidth) * 0.5f);
                        int idx = gui->getTextureAtlas()->getIndexByXY(icon.x, icon.y);
                        if (ImGui::InputInt("Index", &idx, 1,  100)) {
                            if (idx > 0 && idx < gui->getTextureAtlas()->getTotalImages())
                            gui->getTextureAtlas()->setXYByIndex(idx, icon.x, icon.y);
                        }
                        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (availWidth - totalWidth) * 0.5f);
                        ImGui::Text("%s", xLabel);
                        ImGui::SameLine();
                        if (ImGui::SmallButton("<##X")) {
                            if (icon.x > 0) icon.x--;
                        }
                        ImGui::SameLine();
                        if (ImGui::SmallButton(">##X")) {
                            if (icon.x < gui->getTextureAtlas()->getNumColumns()) icon.x++;
                        }

                        // Fila Y
                        char yLabel[32];
                        snprintf(yLabel, sizeof(yLabel), "Y %d", icon.y);
                        float yLabelWidth = ImGui::CalcTextSize(yLabel).x;
                        totalWidth = yLabelWidth + buttonWidth * 2 + ImGui::GetStyle().ItemSpacing.x * 2;

                        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (availWidth - totalWidth) * 0.5f);
                        ImGui::Text("%s", yLabel);
                        ImGui::SameLine();
                        if (ImGui::SmallButton("-##Y")) {
                            if (icon.y > 0) icon.y--;
                        }
                        ImGui::SameLine();
                        if (ImGui::SmallButton("+##Y")) {
                            if (icon.y < gui->getTextureAtlas()->getNumRows()) icon.y++;
                        }
                        ImGui::Dummy(ImVec2(0.0f, 6.0f));
                        ImGui::PopID();
                    }
                }
                ImGui::EndTable();
            }

            if (ImGui::Button("Reset Object Icons")) {
                IconObject::ResetToDefault();
            }
        }
        ImGui::Separator();
        // ===== BOTONES FINALES =====
        if (ImGui::Button("RESET ALL ICONS")) {
            IconGUI::ResetToDefault();
            IconObject::ResetToDefault();
        }
        ImGui::SameLine();
        if (ImGui::Button("Export to JSON")) {
            ExportIconsToJSON(Config::get()->CONFIG_FOLDER + Config::get()->ICONS_CONFIG);
        }
        ImGui::SameLine();
        if (ImGui::Button("Import from JSON")) {
            ImportIconsFromJSON(Config::get()->CONFIG_FOLDER + Config::get()->ICONS_CONFIG);
        }
        ImGui::SameLine();
        if (ImGui::Button("Close")) {
            windowStatus->isOpen = false;
        }
    }

    static void ImportIconsFromJSON(const std::string& filename)
    {
        size_t file_size;
        std::string jsonContent = Tools::ReadFile(filename.c_str(), file_size);

        cJSON* root = cJSON_Parse(jsonContent.c_str());
        if (!root) {
            std::cerr << "Failed to parse JSON" << std::endl;
            return;
        }

        // Importar GUI Icons
        cJSON* guiIcons = cJSON_GetObjectItem(root, "gui_icons");
        if (guiIcons) {
            int arraySize = cJSON_GetArraySize(guiIcons);
            for (int i = 0; i < arraySize && i < IconGUI::ICON_COUNT; i++) {
                cJSON* iconObj = cJSON_GetArrayItem(guiIcons, i);
                cJSON* x = cJSON_GetObjectItem(iconObj, "x");
                cJSON* y = cJSON_GetObjectItem(iconObj, "y");

                if (x && y) {
                    IconGUI::ALL_ICONS_EDITOR[i].icon->x = x->valueint;
                    IconGUI::ALL_ICONS_EDITOR[i].icon->y = y->valueint;
                }
            }
        }

        // Importar Object Icons
        cJSON* objectIcons = cJSON_GetObjectItem(root, "object_icons");
        if (objectIcons) {
            int arraySize = cJSON_GetArraySize(objectIcons);
            for (int i = 0; i < arraySize && i < IconObject::ICON_COUNT; i++) {
                cJSON* iconObj = cJSON_GetArrayItem(objectIcons, i);
                cJSON* x = cJSON_GetObjectItem(iconObj, "x");
                cJSON* y = cJSON_GetObjectItem(iconObj, "y");

                if (x && y) {
                    IconObject::ALL_ICONS_EDITOR[i].icon->x = x->valueint;
                    IconObject::ALL_ICONS_EDITOR[i].icon->y = y->valueint;
                }
            }
        }

        cJSON_Delete(root);
        Logging::Message("Icons imported from: %s", filename.c_str());
    }
};

#endif //BRAKEZA3D_ICONSGUI_H