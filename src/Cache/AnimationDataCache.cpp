#include "../../include/Cache/AnimationDataCache.h"
#include "../../include/Misc/Logging.h"

AnimationDataCache animationDataCache;

void AnimationDataCache::store(const FilePath::ModelFile& path, std::shared_ptr<AnimationData> data)
{
    LOG_MESSAGE("[AnimationDataCache] Store '%s'", std::string(path).c_str());
    storeInternal(normalizePath(path), std::move(data));
}

std::shared_ptr<AnimationData> AnimationDataCache::get(const FilePath::ModelFile& path) const
{
    auto normalized = normalizePath(path);
    auto result = std::static_pointer_cast<AnimationData>(getInternal(normalized));
    if (result) {
        LOG_MESSAGE("[AnimationDataCache] HIT '%s'", normalized.c_str());
    } else {
        LOG_MESSAGE("[AnimationDataCache] MISS '%s'", normalized.c_str());
    }
    return result;
}

void AnimationDataCache::release(const FilePath::ModelFile& path)
{
    removeInternal(normalizePath(path));
}

void AnimationDataCache::visit(
    std::function<void(const FilePath::ModelFile&, const AnimationData&)> visitor) const
{
    forEach([&visitor](const std::string& key, std::shared_ptr<void> ptr) {
        auto* data = static_cast<AnimationData*>(ptr.get());
        if (data) {
            visitor(key, *data);
        }
    });
}

AnimationCacheStats AnimationDataCache::getStats() const
{
    AnimationCacheStats stats;
    visit([&](const FilePath::ModelFile&, const AnimationData& data) {
        for (const auto& m : data.meshes) {
            stats.totalMeshes++;
            stats.totalTriangles += m.triangleVertices.size() / 3;
            stats.totalVertices += m.vertices.size();
        }
    });
    return stats;
}
