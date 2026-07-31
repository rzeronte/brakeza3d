#include "../../include/Cache/ResourceCacheBase.h"
#include <algorithm>

std::string ResourceCacheBase::normalizePath(const std::string& path)
{
    std::string normalized;
    normalized.reserve(path.size());

    bool inSlash = false;
    for (size_t i = 0; i < path.size(); i++) {
        char c = path[i];
        if (c == '\\') c = '/';
        if (c == '/') {
            if (inSlash) continue;
            inSlash = true;
        } else {
            inSlash = false;
        }
        normalized += c;
    }

    return normalized;
}

void ResourceCacheBase::clear()
{
    { std::lock_guard<std::mutex> lock(_mutex); _cache.clear(); }
    { std::lock_guard<std::mutex> hlock(_hitsMutex); _hitsPerKey.clear(); }
    _hits = 0;
    _misses = 0;
}

size_t ResourceCacheBase::getHitsForKey(const std::string& key) const
{
    std::lock_guard<std::mutex> lock(_hitsMutex);
    auto it = _hitsPerKey.find(normalizePath(key));
    return it != _hitsPerKey.end() ? it->second : 0;
}

size_t ResourceCacheBase::size() const
{
    std::lock_guard<std::mutex> lock(_mutex);
    return _cache.size();
}

bool ResourceCacheBase::contains(const std::string& key) const
{
    std::lock_guard<std::mutex> lock(_mutex);
    return _cache.find(key) != _cache.end();
}

std::shared_ptr<void> ResourceCacheBase::getOrLoadInternal(
    const std::string& key,
    std::function<std::shared_ptr<void>()> factory)
{
    std::lock_guard<std::mutex> lock(_mutex);

    auto it = _cache.find(key);
    if (it != _cache.end()) {
        _hits++;
        { std::lock_guard<std::mutex> hlock(_hitsMutex); _hitsPerKey[key]++; }
        return it->second;
    }

    _misses++;
    auto resource = factory();
    _cache[key] = resource;
    return resource;
}

void ResourceCacheBase::storeInternal(const std::string& key, std::shared_ptr<void> resource)
{
    std::lock_guard<std::mutex> lock(_mutex);
    _cache[key] = std::move(resource);
}

std::shared_ptr<void> ResourceCacheBase::getInternal(const std::string& key) const
{
    std::lock_guard<std::mutex> lock(_mutex);
    auto it = _cache.find(key);
    if (it != _cache.end()) {
        _hits++;
        { std::lock_guard<std::mutex> hlock(_hitsMutex); _hitsPerKey[key]++; }
        return it->second;
    }
    _misses++;
    return nullptr;
}

void ResourceCacheBase::removeInternal(const std::string& key)
{
    std::lock_guard<std::mutex> lock(_mutex);
    _cache.erase(key);
}

void ResourceCacheBase::forEach(std::function<void(const std::string&, std::shared_ptr<void>)> callback) const
{
    std::lock_guard<std::mutex> lock(_mutex);
    for (const auto& [key, ptr] : _cache) {
        callback(key, ptr);
    }
}
