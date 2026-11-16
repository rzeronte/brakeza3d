/*
 * Brakeza3D - 3D/2D game engine
 * Author:  Eduardo Rodríguez Álvarez
 * GitHub:  https://github.com/rzeronte/brakeza3d
 * Web:     https://brakeza.com
 * Started: 2018-09-30
 * Copyright (C) 2018-2026 Eduardo Rodríguez Álvarez
 * License: GNU General Public License v3.0 (see LICENSE)
 */
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
#include "../../Misc/AssetPaths.h"
#include "../Windows/FileSystemGUI.h"
#include "../GUI.h"
#include "../Widgets/GUIHelpers.h"

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
            if (GUIHelpers::CancelButton()) ImGui::CloseCurrentPopup();
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

    static std::string lowerName(const std::filesystem::path& p)
    {
        std::string s = p.filename().string();
        std::transform(s.begin(), s.end(), s.begin(), ::tolower);
        return s;
    }

    // Carpetas primero y luego ficheros, cada grupo en orden alfabético sin distinguir mayúsculas
    // (directory_iterator no garantiza ningún orden y los mezclaba).
    void drawFolder(const char* popupId, const std::string& folder)
    {
        namespace fs = std::filesystem;
        // Con un proyecto con assets propios (AssetPaths) se muestran juntas las carpetas del core y del proyecto; la ruta que se devuelve de un
        // fichero del proyecto es relativa a la raíz equivalente del proyecto (el motor la resuelve igual con "../assets/...").
        const auto folders = AssetPaths::ResolveDirs(folder);
        const auto bases   = AssetPaths::ResolveDirs(_baseFolder);
        const std::string projRoot = AssetPaths::GetProjectRoot();
        bool anyExists = false;
        for (auto& f : folders) if (fs::exists(f)) anyExists = true;
        if (!anyExists) { ImGui::TextDisabled("Not found: %s", folder.c_str()); return; }

        std::vector<fs::path> dirs;                              // nombres de subcarpeta (sin duplicados)
        std::vector<std::pair<fs::path, std::string>> files;     // fichero y la carpeta base contra la que se calcula su ruta relativa
        for (auto& f : folders) {
            if (!fs::exists(f)) continue;
            std::string base = _baseFolder;
            if (!projRoot.empty() && f.rfind(projRoot, 0) == 0) {
                for (auto& b : bases) if (b.rfind(projRoot, 0) == 0) base = b;
            }
            for (auto& entry : fs::directory_iterator(f)) {
                if (entry.is_directory()) {
                    const fs::path name = entry.path().filename();
                    if (std::find(dirs.begin(), dirs.end(), name) == dirs.end()) dirs.push_back(name);
                } else if (matchExt(entry.path())) {
                    files.emplace_back(entry.path(), base);
                }
            }
        }
        auto byName = [](const fs::path& a, const fs::path& b) { return lowerName(a) < lowerName(b); };
        std::sort(dirs.begin(), dirs.end(), byName);
        std::sort(files.begin(), files.end(), [&](const auto& a, const auto& b) { return byName(a.first, b.first); });

        for (auto& dir : dirs) {
            if (ImGui::BeginMenu(dir.string().c_str())) {
                drawFolder(popupId, (fs::path(folder) / dir).generic_string());   // en forma "core": se vuelven a unir core y proyecto
                ImGui::EndMenu();
            }
        }
        if (!dirs.empty() && !files.empty()) ImGui::Separator();
        for (auto& [file, base] : files) {
            std::string rel = _relPrefix + file.lexically_relative(fs::path(base)).string();
            std::replace(rel.begin(), rel.end(), '\\', '/');
            if (ImGui::MenuItem(file.filename().string().c_str())) {
                result   = rel;
                accepted = true;
                ImGui::CloseCurrentPopup();
            }
        }
    }
};

#endif //BRAKEZA3D_GUIADDONFILEPICKER_H
