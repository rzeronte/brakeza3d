//
// GUIAddonFilePicker.h
// Popup reutilizable para seleccionar un fichero de un directorio.
// Uso:
//   static GUIFilePicker picker;
//   if (ImGui::Button("Browse")) picker.open(currentPath, baseFolder, relPrefix, {".png",".jpg"});
//   if (picker.draw("MyUniquePopupID")) { useResult(picker.result); }
//

#ifndef BRAKEZA3D_GUIADDONFILEPICKER_H
#define BRAKEZA3D_GUIADDONFILEPICKER_H

#include <imgui.h>
#include <filesystem>
#include <string>
#include <vector>
#include <algorithm>
#include "../Objects/FileSystemGUI.h"
#include "../GUI.h"

struct GUIFilePicker
{
    std::string result;      // path seleccionado (relativo) tras confirmar
    bool        accepted = false;

    void open(const std::string& current, const std::string& baseFolder,
              const std::string& relPrefix, std::vector<std::string> exts)
    {
        _current    = current;
        _baseFolder = baseFolder;
        _relPrefix  = relPrefix;
        _exts       = std::move(exts);
        _show       = true;
        accepted    = false;
        result      = "";
    }

    // Renderiza el botón de disparo (icono carpeta + texto). Devuelve true si se pulsó.
    bool drawTrigger(const char* id)
    {
        ImGui::PushID(id);
        bool clicked = GUI::AutoImageButton(IconGUI::FOLDER, "Browse", ImVec2(16, 16), 18, 6, [](){});
        ImGui::PopID();
        return clicked;
    }

    // Llama cada frame. Devuelve true el frame en que el usuario elige un fichero.
    bool draw(const char* popupId)
    {
        if (_show) { ImGui::OpenPopup(popupId); _show = false; }

        accepted = false;
        if (ImGui::BeginPopup(popupId)) {
            ImGui::TextDisabled("Select file:");
            ImGui::Separator();
            drawFolder(popupId, _baseFolder);
            if (ImGui::Button("Cancel")) ImGui::CloseCurrentPopup();
            ImGui::EndPopup();
        }
        return accepted;
    }

private:
    std::string              _current;
    std::string              _baseFolder;
    std::string              _relPrefix;
    std::vector<std::string> _exts;
    bool                     _show = false;

    bool matchExt(const std::filesystem::path& p) const
    {
        if (_exts.empty()) return true;
        auto ext = p.extension().string();
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        for (auto& e : _exts) {
            std::string le = e;
            std::transform(le.begin(), le.end(), le.begin(), ::tolower);
            if (ext == le) return true;
        }
        return false;
    }

    void drawFolder(const char* popupId, const std::string& folder)
    {
        namespace fs = std::filesystem;
        if (!fs::exists(folder)) { ImGui::TextDisabled("Not found: %s", folder.c_str()); return; }

        for (auto& entry : fs::directory_iterator(folder)) {
            if (entry.is_directory()) {
                if (ImGui::BeginMenu(entry.path().filename().string().c_str())) {
                    drawFolder(popupId, entry.path().string());
                    ImGui::EndMenu();
                }
            } else {
                if (!matchExt(entry.path())) continue;
                std::string rel = _relPrefix + entry.path()
                    .lexically_relative(fs::path(_baseFolder)).string();
                std::replace(rel.begin(), rel.end(), '\\', '/');
                if (ImGui::MenuItem(entry.path().filename().string().c_str())) {
                    result   = rel;
                    accepted = true;
                    ImGui::CloseCurrentPopup();
                }
            }
        }
    }
};

#endif //BRAKEZA3D_GUIADDONFILEPICKER_H
