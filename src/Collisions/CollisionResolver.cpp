
#include "../../headers/Collisions/CollisionResolver.h"
#include "../../headers/PhysicsGame/NPCEnemyBody.h"
#include "../../headers/PhysicsGame/NPCEnemyPartBody.h"

CollisionResolver::CollisionResolver(btPersistentManifold *contactManifold, Object3D *objA, Object3D *objB, BSPMap *bspMap) :
                                                contactManifold(contactManifold), objA(objA), objB(objB), bspMap(bspMap)
{
}

int CollisionResolver::getTypeCollision()
{
    if (isSomeProjectile() && isSomeBSPMap()) {
        return EngineSetup::CollisionResolverTypes::COLLISION_RESOLVER_PROJECTILE_AND_BSPMAP;
    }

    if (isSomeProjectile() && isSomeNPCEnemy()) {
        return EngineSetup::CollisionResolverTypes::COLLISION_RESOLVER_PROJECTILE_AND_NPCENEMY;
    }

    if (isSomeCamera() && isSomeMesh3dFuncDoor()) {
        return EngineSetup::CollisionResolverTypes::COLLISION_RESOLVER_CAMERA_AND_FUNCDOOR;
    }

    if (isSomeCamera() && isSomeMesh3dFuncButton()) {
        return EngineSetup::CollisionResolverTypes::COLLISION_RESOLVER_CAMERA_AND_FUNCBUTTON;
    }

    if (isSomeNPCEnemyPart() && isSomeBSPMap()) {
        return EngineSetup::CollisionResolverTypes::COLLISION_RESOLVER_NPCENEMYPART_AND_BSPMAP;
    }

    return 0;
}

bool CollisionResolver::isSomeCamera()
{
    if (!strcmp(objA->getLabel().c_str(), EngineSetup::getInstance()->cameraNameIdentifier.c_str())) {
        return true;
    }

    if (!strcmp(objB->getLabel().c_str(), EngineSetup::getInstance()->cameraNameIdentifier.c_str())) {
        return true;
    }
}

bool CollisionResolver::isSomeBSPMap()
{
    BSPMap *objAMap = dynamic_cast<BSPMap*> (objA);
    if (objAMap != NULL) {
        return true;
    }

    BSPMap *objBMap = dynamic_cast<BSPMap*> (objB);
    if (objBMap != NULL) {
        return true;
    }

    return false;
}

bool CollisionResolver::isSomeNPCEnemy()
{
    NPCEnemyBody *objANPC = dynamic_cast<NPCEnemyBody*> (objA);
    if (objANPC != NULL) {
        return true;
    }

    NPCEnemyBody *objBNPC = dynamic_cast<NPCEnemyBody*> (objB);
    if (objBNPC != NULL) {
        return true;
    }

    return false;
}

bool CollisionResolver::isSomeNPCEnemyPart()
{
    NPCEnemyPartBody *objANPC = dynamic_cast<NPCEnemyPartBody*> (objA);
    if (objANPC != NULL) {
        return true;
    }

    NPCEnemyPartBody *objBNPC = dynamic_cast<NPCEnemyPartBody*> (objB);
    if (objBNPC != NULL) {
        return true;
    }

    return false;
}

bool CollisionResolver::isSomeProjectile()
{
    Projectile3DBody *objAProjectile = dynamic_cast<Projectile3DBody*> (objA);
    if (objAProjectile != NULL) {
        return true;
    }

    Projectile3DBody *objBProjectile = dynamic_cast<Projectile3DBody*> (objB);
    if (objBProjectile != NULL) {
        return true;
    }

    return false;
}

bool CollisionResolver::isSomeMesh3D()
{
    Mesh3D *objAMesh = dynamic_cast<Mesh3D*> (objA);
    if (objAMesh != NULL) {
        return true;
    }

    Mesh3D *objBMesh = dynamic_cast<Mesh3D*> (objB);
    if (objBMesh != NULL) {
        return true;
    }

    return false;
}

bool CollisionResolver::isSomeMesh3dFuncDoor()
{
    Mesh3D *objAMesh = dynamic_cast<Mesh3D*> (objA);
    if (objAMesh != NULL) {
        if (this->isBSPEntityOfClassName(objAMesh, "func_door")) {
            return true;
        }
    }

    Mesh3D *objBMesh = dynamic_cast<Mesh3D*> (objB);
    if (objBMesh != NULL) {
        if (this->isBSPEntityOfClassName(objBMesh, "func_door")) {
            return true;
        }
    }

    return false;
}

bool CollisionResolver::isSomeMesh3dFuncButton()
{
    Mesh3D *objAMesh = dynamic_cast<Mesh3D*> (objA);
    if (objAMesh != NULL) {
        if (this->isBSPEntityOfClassName(objAMesh, "func_button")) {
            return true;
        }
    }

    Mesh3D *objBMesh = dynamic_cast<Mesh3D*> (objB);
    if (objBMesh != NULL) {
        if (this->isBSPEntityOfClassName(objBMesh, "func_button")) {
            return true;
        }
    }

    return false;
}

bool CollisionResolver::isBSPEntityOfClassName(Mesh3D *oMesh, std::string query)
{
    int originalEntityIndex = oMesh->getBspEntityIndex();

    if (originalEntityIndex > 0) {
        char *classname = bspMap->getEntityValue(originalEntityIndex, "classname");

        if (!strcmp(classname, query.c_str())) {
            return true;
        }
    }

    return false;
}


void CollisionResolver::moveMesh3DBody(Mesh3DBody *oRemoteBody, int targetEntityId) {

    if ( oRemoteBody->isMoving()|| oRemoteBody->isReverseMoving() || oRemoteBody->isWaiting()) return;

    char *angle = bspMap->getEntityValue(targetEntityId, "angle");
    char *speed = bspMap->getEntityValue(targetEntityId, "speed");

    float angleFloat = (float) atof( std::string(angle).c_str() );
    float speedFloat = (float) atof( std::string(speed).c_str() );

    oRemoteBody->setMoving(true);
    oRemoteBody->setAngleMoving(angleFloat);

    if (speedFloat > 0) {
        oRemoteBody->setSpeedMoving(speedFloat);
    }
}

CollisionResolver::~CollisionResolver() {

}
