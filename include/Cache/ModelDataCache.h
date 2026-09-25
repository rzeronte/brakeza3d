#ifndef BRAKEZA3D_MODELDATACACHE_H
#define BRAKEZA3D_MODELDATACACHE_H

#include <mutex>
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
    // La secuencia get()-miss->parse->store() de Mesh3D::AssimpLoadGeometryFromFile se protege con
    // ResourceCacheBase::getKeyLoadMutex(fichero). Antes era un mutex GLOBAL que serializaba TODAS
    // las mallas estáticas (CITY_TREES_Q1..Q4 en cadena: ~12.5 s en la carga, ver
    // .claude/memory/loading-profile-report.md).
};

extern ModelDataCache modelDataCache;

#endif
