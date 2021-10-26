
#include "../../include/Collisions/CollisionResolver.h"
#include "../../include/Game/NPCEnemyBody.h"
#include "../../include/Game/NPCEnemyPartBody.h"
#include "../../include/Game/ItemWeaponGhost.h"
#include "../../include/Game/ItemHealthGhost.h"
#include "../../include/Game/ItemAmmoGhost.h"

CollisionResolver::CollisionResolver(
        btPersistentManifold *contactManifold,
        Object3D *objA,
        Object3D *objB,
        BSPMap *bspMap,
        std::vector<Triangle *> &visibleTriangles
) :
        contactManifold(contactManifold), objA(objA), objB(objB), bspMap(bspMap), visibleTriangles(&visibleTriangles) {
    this->type = -1;
}

int CollisionResolver::getTypeCollision() const {
    if (isSomeItemAmmo() && isSomeProjectile()) {
        return 0;
    }

    if (isSomeItemWeapon() && isSomeProjectile()) {
        return 0;
    }

    if (isSomeProjectile() && isSomeNPCEnemy()) {
        return EngineSetup::CollisionResolverTypes::COLLISION_RESOLVER_PROJECTILE_AND_NPCENEMY;
    }

    if (isSomeCamera() && isSomeMesh3DTriggerMultiple()) {
        return EngineSetup::CollisionResolverTypes::COLLISION_RESOLVER_CAMERA_AND_TRIGGER_MULTIPLE;
    }

    if (isSomeCamera() && isSomeMesh3DTriggerTeleport()) {
        return EngineSetup::CollisionResolverTypes::COLLISION_RESOLVER_CAMERA_AND_TRIGGER_TELEPORT;
    }

    if (isSomeCamera() && isSomeMesh3DFuncDoor()) {
        return EngineSetup::CollisionResolverTypes::COLLISION_RESOLVER_CAMERA_AND_FUNCDOOR;
    }

    if (isSomeCamera() && isSomeMesh3DFuncButton()) {
        return EngineSetup::CollisionResolverTypes::COLLISION_RESOLVER_CAMERA_AND_FUNCBUTTON;
    }

    if (isSomeProjectile() && isSomeCamera()) {
        return EngineSetup::CollisionResolverTypes::COLLISION_RESOLVER_PROJECTILE_AND_CAMERA;
    }

    if (isSomeItemWeapon() && isSomeCamera()) {
        return EngineSetup::CollisionResolverTypes::COLLISION_RESOLVER_ITEMWEAPON_AND_CAMERA;
    }

    if (isSomeItemHealth() && isSomeCamera()) {
        return EngineSetup::CollisionResolverTypes::COLLISION_RESOLVER_ITEMHEALTH_AND_CAMERA;
    }

    if (isSomeItemAmmo() && isSomeCamera()) {
        return EngineSetup::CollisionResolverTypes::COLLISION_RESOLVER_ITEMAMMO_AND_CAMERA;
    }

    return 0;
}

bool CollisionResolver::isSomeCamera() const {
    std::string cameraIdentifier = EngineSetup::getInstance()->cameraTriggerNameIdentifier;
    if (!strcmp(objA->getLabel().c_str(), cameraIdentifier.c_str())) {
        return true;
    }

    if (!strcmp(objB->getLabel().c_str(), cameraIdentifier.c_str())) {
        return true;
    }

    return false;
}

bool CollisionResolver::isSomeNPCEnemy() const {
    auto *objANPC = dynamic_cast<NPCEnemyBody *> (objA);
    if (objANPC != nullptr) {
        return true;
    }

    auto *objBNPC = dynamic_cast<NPCEnemyBody *> (objB);
    if (objBNPC != nullptr) {
        return true;
    }

    return false;
}

bool CollisionResolver::isSomeNPCEnemyPart() const {
    auto *objANPC = dynamic_cast<NPCEnemyPartBody *> (objA);
    if (objANPC != nullptr) {
        return true;
    }

    auto *objBNPC = dynamic_cast<NPCEnemyPartBody *> (objB);
    if (objBNPC != nullptr) {
        return true;
    }

    return false;
}

bool CollisionResolver::isSomeProjectile() const {
    auto *objAProjectile = dynamic_cast<Projectile3DBody *> (objA);
    if (objAProjectile != nullptr) {
        return true;
    }

    auto *objBProjectile = dynamic_cast<Projectile3DBody *> (objB);
    if (objBProjectile != nullptr) {
        return true;
    }

    return false;
}

bool CollisionResolver::isSomeMesh3D() const {
    auto *objAMesh = dynamic_cast<Mesh3D *> (objA);
    if (objAMesh != nullptr) {
        return true;
    }

    auto *objBMesh = dynamic_cast<Mesh3D *> (objB);
    if (objBMesh != nullptr) {
        return true;
    }

    return false;
}

bool CollisionResolver::isSomeMesh3DFuncDoor() const {
    auto *objAMesh = dynamic_cast<DoorGhost *> (objA);
    if (objAMesh != nullptr) {
        return true;
    }

    auto *objBMesh = dynamic_cast<DoorGhost *> (objB);
    if (objBMesh != nullptr) {
        return true;
    }

    return false;
}

bool CollisionResolver::isSomeMesh3DFuncButton() const {
    auto *objAMesh = dynamic_cast<Mesh3D *> (objA);
    if (objAMesh != nullptr) {
        if (this->isBSPEntityOfClassName(objAMesh, "func_button")) {
            return true;
        }
    }

    auto *objBMesh = dynamic_cast<Mesh3D *> (objB);
    if (objBMesh != nullptr) {
        if (this->isBSPEntityOfClassName(objBMesh, "func_button")) {
            return true;
        }
    }

    return false;
}

bool CollisionResolver::isSomeMesh3DTriggerMultiple() const {
    auto *objAMesh = dynamic_cast<Mesh3D *> (objA);
    if (objAMesh != nullptr) {
        if (this->isBSPEntityOfClassName(objAMesh, "trigger_multiple")) {
            return true;
        }
    }

    auto *objBMesh = dynamic_cast<Mesh3D *> (objB);
    if (objBMesh != nullptr) {
        if (this->isBSPEntityOfClassName(objBMesh, "trigger_multiple")) {
            return true;
        }
    }

    return false;
}

bool CollisionResolver::isSomeMesh3DTriggerTeleport() const {
    auto *objAMesh = dynamic_cast<Mesh3D *> (objA);
    if (objAMesh != nullptr) {
        if (this->isBSPEntityOfClassName(objAMesh, "trigger_teleport")) {
            return true;
        }
    }

    auto *objBMesh = dynamic_cast<Mesh3D *> (objB);
    if (objBMesh != nullptr) {
        if (this->isBSPEntityOfClassName(objBMesh, "trigger_teleport")) {
            return true;
        }
    }

    return false;

}

bool CollisionResolver::isSomeItemWeapon() const {
    auto *objAItemWeapon = dynamic_cast<ItemWeaponGhost *> (objA);
    if (objAItemWeapon != nullptr) {
        return true;
    }

    auto *objBItemWeapon = dynamic_cast<ItemWeaponGhost *> (objB);
    if (objBItemWeapon != nullptr) {
        return true;
    }

    return false;
}

bool CollisionResolver::isSomeItemHealth() const {
    auto *tmpObjA = dynamic_cast<ItemHealthGhost *> (objA);
    if (tmpObjA != nullptr) {
        return true;
    }

    auto *tmpObjB = dynamic_cast<ItemHealthGhost *> (objB);
    if (tmpObjB != nullptr) {
        return true;
    }

    return false;
}

bool CollisionResolver::isSomeItemAmmo() const {
    auto *tmpObjA = dynamic_cast<ItemAmmoGhost *> (objA);
    if (tmpObjA != nullptr) {
        return true;
    }

    auto *tmpObjB = dynamic_cast<ItemAmmoGhost *> (objB);
    if (tmpObjB != nullptr) {
        return true;
    }

    return false;
}

bool CollisionResolver::isBSPEntityOfClassName(Mesh3D *oMesh, const std::string& query) const {
    int originalEntityIndex = oMesh->getBspEntityIndex();

    if (originalEntityIndex > 0) {
        char *classname = bspMap->getEntityValue(originalEntityIndex, "classname");

        if (!strcmp(classname, query.c_str())) {
            return true;
        }
    }

    return false;
}


void CollisionResolver::moveDoorGhost(DoorGhost *oRemoteBody, int targetEntityId) const {

    if (oRemoteBody->isMoving() || oRemoteBody->isReverseMoving() || oRemoteBody->isWaiting()) return;

    char *angle = bspMap->getEntityValue(targetEntityId, "angle");
    char *speed = bspMap->getEntityValue(targetEntityId, "speed");

    float angleFloat = (float) atof(std::string(angle).c_str());
    float speedFloat = (float) atof(std::string(speed).c_str());

    oRemoteBody->setMoving(true);
    oRemoteBody->setAngleMoving(angleFloat);

    if (speedFloat > 0) {
        oRemoteBody->setSpeedMoving(speedFloat);
    }
}

int CollisionResolver::getType() const {
    return type;
}

void CollisionResolver::setType(int type) {
    CollisionResolver::type = type;
}
