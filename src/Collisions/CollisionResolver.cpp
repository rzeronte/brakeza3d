
#include "../../include/Physics/CollisionResolver.h"

CollisionResolver::CollisionResolver(
        Body *objA,
        Body *objB,
        std::vector<Triangle *> &visibleTriangles
) :objA(objA), objB(objB), visibleTriangles(&visibleTriangles)
{
}

bool CollisionResolver::isBSPEntityOfClassName(BSPMap *bspMap, Mesh3D *oMesh, const std::string& query) const {
    int originalEntityIndex = oMesh->getBspEntityIndex();

    if (originalEntityIndex > 0) {
        char *classname = bspMap->getEntityValue(originalEntityIndex, "classname");

        if (!strcmp(classname, query.c_str())) {
            return true;
        }
    }

    return false;
}
