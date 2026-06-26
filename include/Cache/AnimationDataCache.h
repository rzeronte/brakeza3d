#ifndef BRAKEZA3D_ANIMATIONDATACACHE_H
#define BRAKEZA3D_ANIMATIONDATACACHE_H

#include "ResourceCacheBase.h"
#include "AnimationData.h"
#include "../Misc/FilePaths.h"

struct AnimationCacheStats {
    size_t totalMeshes = 0;
    size_t totalTriangles = 0;
    size_t totalVertices = 0;
};

class AnimationDataCache : public ResourceCacheBase {
public:
    std::shared_ptr<AnimationData> get(const FilePath::ModelFile& path) const;
    void store(const FilePath::ModelFile& path, std::shared_ptr<AnimationData> data);
    void release(const FilePath::ModelFile& path);

    void visit(std::function<void(const FilePath::ModelFile& path, const AnimationData& data)> visitor) const;
    AnimationCacheStats getStats() const;
};

extern AnimationDataCache animationDataCache;

#endif
