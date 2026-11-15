/*
 * Brakeza3D - 3D/2D game engine
 * Author:  Eduardo Rodríguez Álvarez
 * GitHub:  https://github.com/rzeronte/brakeza3d
 * Web:     https://brakeza.com
 * Started: 2018-09-30
 * Copyright (C) 2018-2026 Eduardo Rodríguez Álvarez
 * License: GNU General Public License v3.0 (see LICENSE)
 */
#ifndef BRAKEZA3D_SCENECACHE_H
#define BRAKEZA3D_SCENECACHE_H

#include <string>
#include <memory>
#include <unordered_map>
#include <mutex>
#include <assimp/Importer.hpp>

class SceneCache {
    std::unordered_map<std::string, std::weak_ptr<Assimp::Importer>> entries;
    std::mutex mtx;
public:
    std::shared_ptr<Assimp::Importer> get(const std::string& path);
    void store(const std::string& path, std::shared_ptr<Assimp::Importer> imp);
    size_t size() const;
};

extern SceneCache sceneCache;

#endif
