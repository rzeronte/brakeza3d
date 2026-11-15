/*
 * Brakeza3D - 3D/2D game engine
 * Author:  Eduardo Rodríguez Álvarez
 * GitHub:  https://github.com/rzeronte/brakeza3d
 * Web:     https://brakeza.com
 * Started: 2018-09-30
 * Copyright (C) 2018-2026 Eduardo Rodríguez Álvarez
 * License: GNU General Public License v3.0 (see LICENSE)
 */
#include "../../include/Cache/ScriptDataCache.h"
#include "../../include/Misc/Tools.h"
#include "../../include/Misc/Logging.h"

ScriptDataCache scriptDataCache;

std::string ScriptDataCache::getOrLoad(const FilePath::ScriptFile& path)
{
    auto normalized = normalizePath(path);
    auto cached = std::static_pointer_cast<std::string>(
        getOrLoadInternal(normalized, [&normalized]() -> std::shared_ptr<void> {
            LOG_MESSAGE("[ScriptDataCache] Loading '%s'", normalized.c_str());
            char* raw = Tools::ReadFile(normalized);
            std::string content(raw ? raw : "");
            if (raw) free(raw);
            return std::make_shared<std::string>(std::move(content));
        })
    );
    return *cached;
}

void ScriptDataCache::release(const FilePath::ScriptFile& path)
{
    removeInternal(normalizePath(path));
}

void ScriptDataCache::visit(
    std::function<void(const FilePath::ScriptFile&, const std::string&)> visitor) const
{
    forEach([&visitor](const std::string& key, std::shared_ptr<void> ptr) {
        auto* data = static_cast<std::string*>(ptr.get());
        if (data) {
            visitor(key, *data);
        }
    });
}
