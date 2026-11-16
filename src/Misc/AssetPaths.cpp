/*
 * Brakeza3D - 3D/2D game engine
 * Author:  Eduardo Rodríguez Álvarez
 * GitHub:  https://github.com/rzeronte/brakeza3d
 * Web:     https://brakeza.com
 * Started: 2018-09-30
 * Copyright (C) 2018-2026 Eduardo Rodríguez Álvarez
 * License: GNU General Public License v3.0 (see LICENSE)
 */
#include "../../include/Misc/AssetPaths.h"

#include <atomic>
#include <filesystem>
#include <mutex>
#include <unordered_map>
#include <vector>

namespace {
    std::mutex                           gMutex;
    std::string                          gProjectRoot;                 // con "/" final, o vacía
    std::string                          gCorePrefix = "../assets/";   // con "/" final
    std::unordered_map<std::string, bool> gExists;                     // ruta candidata -> existe
    std::atomic<bool>                    gActive{false};               // lectura rápida sin bloquear cuando no hay proyecto

    std::string WithTrailingSlash(std::string s)
    {
        for (auto &c : s) if (c == '\\') c = '/';
        if (!s.empty() && s.back() != '/') s += '/';
        return s;
    }

    // ¿Cuelga `path` de `prefix` ("../assets/")? Si sí, `rel` es la parte que sigue (sin barras iniciales; vacía si path es la propia raíz).
    bool Under(const std::string &path, const std::string &prefix, std::string &rel)
    {
        const std::string bare = prefix.substr(0, prefix.size() - 1);   // "../assets"
        if (path.size() < bare.size() || path.compare(0, bare.size(), bare) != 0) return false;
        if (path.size() > bare.size() && path[bare.size()] != '/') return false;   // "../assets_otros" no cuelga de "../assets"
        size_t i = bare.size();
        while (i < path.size() && path[i] == '/') i++;
        rel = path.substr(i);
        return true;
    }

    // Colapsa "." y ".." de una ruta relativa (los modelos referencian texturas como ".../models/pack/BASE/../../../textures/x.png": contado desde
    // la raíz del PROYECTO esos ".." se saldrían de ella). Devuelve false si los ".." escapan de la raíz (no se puede resolver contra ninguna carpeta).
    bool Normalize(const std::string &rel, std::string &out)
    {
        std::vector<std::string> parts;
        size_t i = 0;
        while (i <= rel.size()) {
            size_t j = rel.find('/', i);
            if (j == std::string::npos) j = rel.size();
            const std::string seg = rel.substr(i, j - i);
            if (seg == "..") {
                if (parts.empty()) return false;
                parts.pop_back();
            } else if (!seg.empty() && seg != ".") {
                parts.push_back(seg);
            }
            i = j + 1;
        }
        out.clear();
        for (size_t k = 0; k < parts.size(); k++) out += (k ? "/" : "") + parts[k];
        if (!rel.empty() && rel.back() == '/' && !out.empty()) out += '/';
        return true;
    }
}

namespace AssetPaths {

    void SetProjectRoot(const std::string &folder)
    {
        std::lock_guard<std::mutex> lock(gMutex);
        gProjectRoot = folder.empty() ? "" : WithTrailingSlash(folder);
        gExists.clear();
        gActive = !gProjectRoot.empty();
    }

    std::string GetProjectRoot()
    {
        std::lock_guard<std::mutex> lock(gMutex);
        return gProjectRoot;
    }

    void SetCorePrefix(const std::string &prefix)
    {
        std::lock_guard<std::mutex> lock(gMutex);
        gCorePrefix = prefix.empty() ? "../assets/" : WithTrailingSlash(prefix);
        gExists.clear();
    }

    std::string GetCorePrefix()
    {
        std::lock_guard<std::mutex> lock(gMutex);
        return gCorePrefix;
    }

    bool IsActive()
    {
        return gActive.load();
    }

    void ClearCache()
    {
        std::lock_guard<std::mutex> lock(gMutex);
        gExists.clear();
    }

    std::string RelativeToCore(const std::string &path)
    {
        std::lock_guard<std::mutex> lock(gMutex);
        std::string rel;
        return Under(path, gCorePrefix, rel) ? rel : std::string();
    }

    std::string Resolve(const std::string &path)
    {
        if (!gActive.load() || path.empty()) return path;             // sin proyecto con assets propios: ruta intacta (comportamiento anterior)

        std::lock_guard<std::mutex> lock(gMutex);
        std::string rel;
        if (!Under(path, gCorePrefix, rel) || rel.empty()) return path;   // no es un asset del core ("../GLSL/...", "../config/...", absolutas...)

        std::string norm;
        if (!Normalize(rel, norm) || norm.empty()) return path;       // los ".." escapan de la raíz de assets: no hay equivalente en el proyecto

        const std::string candidate = gProjectRoot + norm;
        auto it = gExists.find(candidate);
        if (it == gExists.end()) {
            std::error_code ec;
            const bool ok = std::filesystem::exists(candidate, ec) && !ec;
            it = gExists.emplace(candidate, ok).first;
        }
        return it->second ? candidate : path;                         // el proyecto primero, el core de respaldo
    }

    std::vector<std::string> ResolveDirs(const std::string &dirPath)
    {
        if (!gActive.load() || dirPath.empty()) return {dirPath};

        std::lock_guard<std::mutex> lock(gMutex);
        std::string rel;
        if (!Under(dirPath, gCorePrefix, rel)) return {dirPath};
        std::string norm;
        if (!Normalize(rel, norm)) return {dirPath};

        std::vector<std::string> out;
        std::error_code ec;
        if (std::filesystem::is_directory(dirPath, ec)) out.push_back(dirPath);                  // el core primero...
        const std::string candidate = gProjectRoot + norm;
        if (std::filesystem::is_directory(candidate, ec)) out.push_back(candidate);              // ...y el proyecto después (gana en conflictos)
        if (out.empty()) out.push_back(dirPath);                                                 // ninguno existe: que el llamador lo gestione como siempre
        return out;
    }

} // namespace AssetPaths
