
#include "../../include/Collisions/CollisionResolver.h"
#include "../../include/Game/NPCEnemyPartBody.h"

CollisionResolver::CollisionResolver(
        btPersistentManifold *contactManifold,
        Body *objA,
        Body *objB,
        std::vector<Triangle *> &visibleTriangles
) :contactManifold(contactManifold), objA(objA), objB(objB), visibleTriangles(&visibleTriangles)
{
    this->type = -1;
}

int CollisionResolver::getTypeCollision() const {
    return 0;
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

int CollisionResolver::getType() const {
    return type;
}

void CollisionResolver::setType(int type) {
    CollisionResolver::type = type;
}
