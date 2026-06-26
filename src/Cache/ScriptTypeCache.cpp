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
