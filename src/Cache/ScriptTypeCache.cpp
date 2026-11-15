/*
 * Brakeza3D - 3D/2D game engine
 * Author:  Eduardo Rodríguez Álvarez
 * GitHub:  https://github.com/rzeronte/brakeza3d
 * Web:     https://brakeza.com
 * Started: 2018-09-30
 * Copyright (C) 2018-2026 Eduardo Rodríguez Álvarez
 * License: GNU General Public License v3.0 (see LICENSE)
 */
#include "../../include/Cache/ScriptTypeCache.h"
#include "../../include/Misc/Tools.h"
#include "../../include/Misc/Logging.h"

ScriptTypeCache scriptTypeCache;

std::string ScriptTypeCache::getOrLoad(const FilePath::TypesFile& path)
{
    auto normalized = normalizePath(path);
    auto cached = std::static_pointer_cast<std::string>(
        getOrLoadInternal(normalized, [&normalized]() -> std::shared_ptr<void> {
            LOG_MESSAGE("[ScriptTypeCache] Loading '%s'", normalized.c_str());
            char* raw = Tools::ReadFile(normalized);
            std::string content(raw ? raw : "");
            if (raw) free(raw);
            return std::make_shared<std::string>(std::move(content));
        })
    );
    return *cached;
}

void ScriptTypeCache::release(const FilePath::TypesFile& path)
{
    removeInternal(normalizePath(path));
}
