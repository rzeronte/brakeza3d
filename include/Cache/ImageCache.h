#ifndef BRAKEZA3D_IMAGECACHE_H
#define BRAKEZA3D_IMAGECACHE_H

#include "ResourceCacheBase.h"
#include "../Render/Image.h"
#include "../Misc/FilePaths.h"

class ImageCache : public ResourceCacheBase {
public:
    Image* getOrLoad(const FilePath::ImageFile& path);
    void release(const FilePath::ImageFile& path);

    void visit(std::function<void(const FilePath::ImageFile&, Image*)> visitor) const;
    size_t getMemoryUsage() const;
};

extern ImageCache imageCache;

#endif
