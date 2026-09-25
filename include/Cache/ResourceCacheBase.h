#ifndef BRAKEZA3D_RESOURCECACHEBASE_H
#define BRAKEZA3D_RESOURCECACHEBASE_H

#include <string>
#include <map>
#include <memory>
#include <mutex>
#include <functional>
#include <atomic>

class ResourceCacheBase {
public:
    ResourceCacheBase() = default;
    virtual ~ResourceCacheBase() = default;

    void clear();
    size_t size() const;
    bool contains(const std::string& key) const;
    void forEach(std::function<void(const std::string&, std::shared_ptr<void>)> callback) const;

    size_t getHits() const    { return _hits; }
    size_t getMisses() const  { return _misses; }
    size_t getHitsForKey(const std::string& key) const;
    void resetStats()         { _hits = 0; _misses = 0; std::lock_guard<std::mutex> hlock(_hitsMutex); _hitsPerKey.clear(); }

    // Mutex POR CLAVE para secuencias get()-miss -> parse -> store() que no son atómicas (Mesh3D /
    // Mesh3DAnimation cargan fuera de getOrLoadInternal). Serializa solo cargas del MISMO fichero
    // (evita parsearlo N veces en paralelo y subir geometría duplicada); ficheros distintos se
    // cargan en paralelo. Las entradas no se borran nunca (unas pocas decenas de ficheros), así un
    // hilo que tenga el mutex tomado nunca lo ve destruido, ni siquiera tras clear().
    std::shared_ptr<std::mutex> getKeyLoadMutex(const std::string& key);

protected:
    std::shared_ptr<void> getOrLoadInternal(
        const std::string& key,
        std::function<std::shared_ptr<void>()> factory
    );
    void storeInternal(const std::string& key, std::shared_ptr<void> resource);
    std::shared_ptr<void> getInternal(const std::string& key) const;
    void removeInternal(const std::string& key);

    static std::string normalizePath(const std::string& path);

private:
    std::map<std::string, std::shared_ptr<void>> _cache;
    mutable std::map<std::string, size_t> _hitsPerKey;
    std::map<std::string, std::shared_ptr<std::mutex>> _keyLoadMutexes;
    std::mutex _keyLoadMapMutex;
    mutable std::mutex _mutex;
    mutable std::mutex _hitsMutex;
    mutable std::atomic<size_t> _hits{0};
    mutable std::atomic<size_t> _misses{0};
};

#endif
