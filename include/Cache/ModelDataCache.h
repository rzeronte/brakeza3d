#ifndef BRAKEZA3D_MODELDATACACHE_H
#define BRAKEZA3D_MODELDATACACHE_H

#include "ResourceCacheBase.h"
#include "ModelData.h"
#include "../Misc/FilePaths.h"

struct ModelCacheStats {
    size_t totalMeshes = 0;
    size_t totalTriangles = 0;
    size_t totalVertices = 0;
};

class ModelDataCache : public ResourceCacheBase {
public:
    std::shared_ptr<ModelData> get(const FilePath::ModelFile& path) const;
    void store(const FilePath::ModelFile& path, std::shared_ptr<ModelData> data);
    void release(const FilePath::ModelFile& path);

    void visit(std::function<void(const FilePath::ModelFile& path, const ModelData& data)> visitor) const;
    ModelCacheStats getStats() const;
};

extern ModelDataCache modelDataCache;

#endif
