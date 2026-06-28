#include "../../include/Cache/SceneCache.h"
#include "../../include/Misc/Logging.h"

SceneCache sceneCache;

std::shared_ptr<Assimp::Importer> SceneCache::get(const std::string& path)
{
    std::lock_guard<std::mutex> lock(mtx);
    auto it = entries.find(path);
    if (it != entries.end()) {
        auto imp = it->second.lock();
        if (imp) {
            LOG_MESSAGE("[SceneCache] HIT '%s'", path.c_str());
            return imp;
        }
        entries.erase(it);
    }
    LOG_MESSAGE("[SceneCache] MISS '%s'", path.c_str());
    return nullptr;
}

void SceneCache::store(const std::string& path, std::shared_ptr<Assimp::Importer> imp)
{
    std::lock_guard<std::mutex> lock(mtx);
    entries[path] = imp;
    LOG_MESSAGE("[SceneCache] Stored '%s'", path.c_str());
}

size_t SceneCache::size() const
{
    std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(mtx));
    size_t alive = 0;
    for (auto& kv : entries) {
        if (!kv.second.expired()) alive++;
    }
    return alive;
}
