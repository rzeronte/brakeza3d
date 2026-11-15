/*
 * Brakeza3D - 3D/2D game engine
 * Author:  Eduardo Rodríguez Álvarez
 * GitHub:  https://github.com/rzeronte/brakeza3d
 * Web:     https://brakeza.com
 * Started: 2018-09-30
 * Copyright (C) 2018-2026 Eduardo Rodríguez Álvarez
 * License: GNU General Public License v3.0 (see LICENSE)
 */
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
