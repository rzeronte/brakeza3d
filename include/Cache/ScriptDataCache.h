#ifndef BRAKEZA3D_SCRIPTDATACACHE_H
#define BRAKEZA3D_SCRIPTDATACACHE_H

#include "ResourceCacheBase.h"
#include "../Misc/FilePaths.h"

class ScriptDataCache : public ResourceCacheBase {
public:
    std::string getOrLoad(const FilePath::ScriptFile& path);
    void release(const FilePath::ScriptFile& path);

    void visit(std::function<void(const FilePath::ScriptFile&, const std::string&)> visitor) const;
};

extern ScriptDataCache scriptDataCache;

#endif
