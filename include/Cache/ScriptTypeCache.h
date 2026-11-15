/*
 * Brakeza3D - 3D/2D game engine
 * Author:  Eduardo Rodríguez Álvarez
 * GitHub:  https://github.com/rzeronte/brakeza3d
 * Web:     https://brakeza.com
 * Started: 2018-09-30
 * Copyright (C) 2018-2026 Eduardo Rodríguez Álvarez
 * License: GNU General Public License v3.0 (see LICENSE)
 */
#ifndef BRAKEZA3D_SCRIPTTYPECACHE_H
#define BRAKEZA3D_SCRIPTTYPECACHE_H

#include "ResourceCacheBase.h"
#include "../Misc/FilePaths.h"

class ScriptTypeCache : public ResourceCacheBase {
public:
    std::string getOrLoad(const FilePath::TypesFile& path);
    void release(const FilePath::TypesFile& path);
};

extern ScriptTypeCache scriptTypeCache;

#endif
