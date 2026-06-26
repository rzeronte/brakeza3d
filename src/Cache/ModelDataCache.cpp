#include "../../include/Cache/ModelDataCache.h"
#include "../../include/Misc/Logging.h"

ModelDataCache modelDataCache;

void ModelDataCache::store(const FilePath::ModelFile& path, std::shared_ptr<ModelData> data)
{
    LOG_MESSAGE("[ModelDataCache] Store '%s'", std::string(path).c_str());
    storeInternal(normalizePath(path), std::move(data));
}

std::shared_ptr<ModelData> ModelDataCache::get(const FilePath::ModelFile& path) const
{
    auto normalized = normalizePath(path);
    auto cached = std::static_pointer_cast<ModelData>(getInternal(normalized));
    if (cached) {
        LOG_MESSAGE("[ModelDataCache] HIT '%s'", normalized.c_str());
    } else {
        LOG_MESSAGE("[ModelDataCache] MISS '%s'", normalized.c_str());
    }
    return cached;
}

void ModelDataCache::release(const FilePath::ModelFile& path)
{
    removeInternal(normalizePath(path));
}

void ModelDataCache::visit(std::function<void(const FilePath::ModelFile&, const ModelData&)> visitor) const
{
    forEach([&visitor](const std::string& key, std::shared_ptr<void> ptr) {
        auto* data = static_cast<ModelData*>(ptr.get());
        if (data) {
            visitor(key, *data);
        }
    });
}

ModelCacheStats ModelDataCache::getStats() const
{
    ModelCacheStats stats;
    visit([&](const FilePath::ModelFile&, const ModelData& data) {
        for (const auto& m : data.meshes) {
            stats.totalMeshes++;
            stats.totalTriangles += m.triangleVertices.size() / 3;
            stats.totalVertices += m.vertices.size();
        }
    });
    return stats;
}
