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
