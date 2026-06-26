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
