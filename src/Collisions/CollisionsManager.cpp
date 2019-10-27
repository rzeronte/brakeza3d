#include "../../headers/Collisions/CollisionsManager.h"
#include "BulletCollision/CollisionDispatch/btGhostObject.h"
#include "../../headers/Render/Engine.h"
#include "../../headers/Collisions/CollisionResolver.h"
#include "../../headers/Collisions/CollisionResolverBetweenProjectileAndNPCEnemy.h"
#include "../../headers/Collisions/CollisionResolverBetweenCamera3DAndFuncDoor.h"
#include "../../headers/Collisions/CollisionResolverBetweenCamera3DAndFuncButton.h"
#include "../../headers/Collisions/CollisionResolverBetweenProjectileAndBSPMap.h"
#include "../../headers/Collisions/CollisionResolverBetweenEnemyPartAndBSPMap.h"
#include "../../headers/Collisions/CollisionResolverBetweenProjectileAndPlayer.h"

CollisionsManager::CollisionsManager()
{

}

void CollisionsManager::initBulletSystem()
{
    ///collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration.
    this->collisionConfiguration = new btDefaultCollisionConfiguration();

    ///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
    this->dispatcher = new btCollisionDispatcher(collisionConfiguration);

    ///btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
    this->overlappingPairCache = new btDbvtBroadphase();

    btVector3 worldMin(-500,-500,-500);
    btVector3 worldMax(500,500,500);
    btAxisSweep3* sweepBP = new btAxisSweep3(worldMin,worldMax);
    this->overlappingPairCache = sweepBP;

    ///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
    this->solver = new btSequentialImpulseConstraintSolver;

    /// Debug drawer
    this->debugDraw = new PhysicsDebugDraw(this->camera);

    this->dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);
    this->dynamicsWorld->setGravity(btVector3(0, EngineSetup::getInstance()->gravity.y, 0));

    this->overlappingPairCache->getOverlappingPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());

    this->camera->kinematicController->setGravity( dynamicsWorld->getGravity() );
    this->camera->kinematicController->setFallSpeed(00);

    this->dynamicsWorld->addCollisionObject(this->camera->m_ghostObject, btBroadphaseProxy::CharacterFilter, btBroadphaseProxy::StaticFilter);
    this->dynamicsWorld->addAction(this->camera->kinematicController);

    this->dynamicsWorld->setDebugDrawer(debugDraw);
    this->dynamicsWorld->getDebugDrawer()->setDebugMode(PhysicsDebugDraw::DBG_DrawWireframe);

    // Detect collisions without affect physics (used by triggers)
    this->makeGhostForCamera();
}

btDiscreteDynamicsWorld *CollisionsManager::getDynamicsWorld() const {
    return dynamicsWorld;
}

void CollisionsManager::setDynamicsWorld(btDiscreteDynamicsWorld *dynamicsWorld) {
    CollisionsManager::dynamicsWorld = dynamicsWorld;
}

Camera3D *CollisionsManager::getCamera() const {
    return camera;
}

void CollisionsManager::setCamera(Camera3D *camera) {
    CollisionsManager::camera = camera;
}

Mesh3DGhost *CollisionsManager::getTriggerCamera() const {
    return triggerCamera;
}

void CollisionsManager::setTriggerCamera(Mesh3DGhost *triggerCamera) {
    CollisionsManager::triggerCamera = triggerCamera;
}

BSPMap *CollisionsManager::getBspMap() const {
    return bspMap;
}

void CollisionsManager::setBspMap(BSPMap *bspMap) {
    CollisionsManager::bspMap = bspMap;
}

std::vector<Object3D *> *CollisionsManager::getGameObjects() const {
    return gameObjects;
}

void CollisionsManager::setGameObjects(std::vector<Object3D *> *gameObjects) {
    CollisionsManager::gameObjects = gameObjects;
}

void CollisionsManager::makeGhostForCamera()
{
    triggerCamera = new Mesh3DGhost();
    triggerCamera->setLabel(EngineSetup::getInstance()->cameraTriggerNameIdentifier);
    triggerCamera->setEnabled(true);
    triggerCamera->setPosition(*camera->getPosition());
    triggerCamera->getGhostObject()->setCollisionShape(camera->kinematicController->getGhostObject()->getCollisionShape());
    triggerCamera->getGhostObject()->setUserPointer(triggerCamera);
    dynamicsWorld->addCollisionObject(triggerCamera->getGhostObject(), EngineSetup::collisionGroups::CameraTrigger, EngineSetup::collisionGroups::DefaultFilter|EngineSetup::collisionGroups::BSPHullTrigger);
}

bool CollisionsManager::needsCollision(const btCollisionObject* body0, const btCollisionObject* body1)
{
    bool collides = (body0->getBroadphaseHandle()->m_collisionFilterGroup & body1->getBroadphaseHandle()->m_collisionFilterMask) != 0;
    collides = collides && (body1->getBroadphaseHandle()->m_collisionFilterGroup & body0->getBroadphaseHandle()->m_collisionFilterMask);

    return collides;
}

void CollisionsManager::checkCollisionsForTriggerCamera()
{
    for (int i = 0; i < this->triggerCamera->getGhostObject()->getNumOverlappingObjects(); i++) {
        const btCollisionObject *obj = this->triggerCamera->getGhostObject()->getOverlappingObject(i);
        Mesh3D *brkObjectB = (Mesh3D *) obj->getUserPointer();

        // No siempre tienen por qué ser Mesh
        if (brkObjectB != NULL) {
            if (brkObjectB->getLabel().find("hull") != std::string::npos) {
                int entityIndex = brkObjectB->getBspEntityIndex();
                char *classname = bspMap->getEntityValue(entityIndex, "classname");

                if (!strcmp(classname, "trigger_teleport")) {
                    int targetEntityId = bspMap->getIndexOfFirstEntityByTargetname(bspMap->getEntityValue(entityIndex, "target") );
                    if (targetEntityId >= 0) {

                        if (this->bspMap->hasEntityAttribute(targetEntityId, "origin")) {
                            char *value = bspMap->getEntityValue(targetEntityId, "origin");
                            char *angle = bspMap->getEntityValue(targetEntityId, "angle");
                            Vertex3D teleportPos = bspMap->parsePositionFromEntityAttribute(value);

                            float BSP_YOffset = -5;
                            float BSP_ZOffset = 0;

                            int angleInt = atoi( std::string(angle).c_str() );
                            camera->yaw   = 90-angleInt;
                            camera->pitch = 0;
                            camera->roll  = 0;

                            btVector3 btPos = btVector3(teleportPos.x, teleportPos.y+BSP_YOffset, teleportPos.z+BSP_ZOffset);

                            btTransform initialTransform;
                            initialTransform.setOrigin( btPos );

                            this->camera->kinematicController->getGhostObject()->setWorldTransform(initialTransform);

                            if (EngineSetup::getInstance()->LOG_COLLISION_OBJECTS) {
                                Logging::getInstance()->getInstance()->Log( "[LOG_COLLISION_OBJECTS] teleporting to " +std::string(bspMap->getEntityValue(entityIndex, "target")));
                            }
                        }
                    }
                }

                if (!strcmp(classname, "func_door")) {
                    //Tools::writeTextCenter(Engine::renderer, Engine::font, Color::white(), std::string("func_door") );
                }

                if (!strcmp(classname, "trigger_multiple")) {
                    // check for message response
                    if (strlen(bspMap->getEntityValue(entityIndex, "message")) > 0) {
                        //Tools::writeTextCenter(Engine::renderer, Engine::font, Color::white(), std::string(bspMap->getEntityValue(entityIndex, "message")) );
                    }
                }
            }
        }

        if (EngineSetup::getInstance()->LOG_COLLISION_OBJECTS) {
            Logging::getInstance()->getInstance()->Log("[LOG_COLLISION_OBJECTS] Collision between triggerCamera and " + brkObjectB->getLabel());
        }
    }
}

void CollisionsManager::checkCollisionsForAll()
{
    if (!EngineSetup::getInstance()->BULLET_CHECK_ALL_PAIRS) return;

    // All collisions pairs
    if (EngineSetup::getInstance()->BULLET_CHECK_ALL_PAIRS) {
        int numManifolds = this->dynamicsWorld->getDispatcher()->getNumManifolds();
        for (int i = 0; i < numManifolds; i++) {
            btPersistentManifold *contactManifold = this->dynamicsWorld->getDispatcher()->getManifoldByIndexInternal(i);
            if (contactManifold->getNumContacts() > 0) {
                const btCollisionObject *obA = contactManifold->getBody0();
                const btCollisionObject *obB = contactManifold->getBody1();

                Object3D *brkObjectA = (Object3D *) obA->getUserPointer();
                Object3D *brkObjectB = (Object3D *) obB->getUserPointer();

                auto *collisionResolver = new CollisionResolver(contactManifold, brkObjectA, brkObjectB, getBspMap(), getVisibleTriangles());
                Brakeza3D::get()->getCollisionManager()->getCollisions().push_back( collisionResolver );

            }
        }
    }
}

void CollisionsManager::updatePhysicObjects()
{
    std::vector<Object3D *>::iterator it;
    for (it = gameObjects->begin(); it != gameObjects->end(); it++) {
        // projectiles
        Projectile3DBody *projectileBody = dynamic_cast<Projectile3DBody*> ((*it));
        if (projectileBody != NULL) {
            projectileBody->integrate();
        }

        // mesh physics
        Mesh3DBody *meshBody = dynamic_cast<Mesh3DBody*> ((*it));
        if (meshBody) {
            meshBody->integrate( );
        }

        // sprite3d
        Sprite3DBody *spriteBody = dynamic_cast<Sprite3DBody*> ((*it));
        if (spriteBody != NULL) {
            spriteBody->integrate();
        }

        // sprite directional
        SpriteDirectional3DBody *spriteDirectionalBody = dynamic_cast<SpriteDirectional3DBody*> ((*it));
        if (spriteDirectionalBody != NULL) {
            spriteDirectionalBody->integrate();
        }
    }

    // Sync position for triggerCamera
    Vertex3D direction = camera->getRotation().getTranspose() * EngineSetup::getInstance()->forward;
    Vertex3D p = *camera->getPosition();

    float farDist = 1;
    p.x = p.x + direction.x * farDist;
    p.y = p.y + direction.y * farDist;
    p.z = p.z + direction.z * farDist;

    btTransform t;
    t.setIdentity();
    t.setOrigin(btVector3(p.x, p.y, p.z));
    getTriggerCamera()->getGhostObject()->setWorldTransform(t);
}

Vertex3D CollisionsManager::stepSimulation()
{
    float time = Brakeza3D::get()->getDeltaTime();

    // check for collisions
    if (bspMap->isLoaded()) {
        checkCollisionsForTriggerCamera();
        checkCollisionsForAll();
    }

    Vertex3D vel = getCamera()->velocity.getComponent();
    btVector3 bulletVelocity = btVector3(vel.x, vel.y, vel.z);

    if (this->camera->kinematicController->onGround()) {
        bulletVelocity = btVector3(vel.x, vel.y, vel.z);
    } else {
        bulletVelocity = btVector3(vel.x, vel.y, vel.z) / EngineSetup::getInstance()->AIR_RESISTANCE;
    }

    this->camera->kinematicController->setWalkDirection(bulletVelocity);

    Vertex3D finalVelocity;

    if (EngineSetup::getInstance()->BULLET_STEP_SIMULATION) {
        // Bullet Step Simulation
        getDynamicsWorld()->stepSimulation(time, 1);

        // Physics for meshes
        updatePhysicObjects();

        btTransform trans = this->camera->kinematicController->getGhostObject()->getWorldTransform();

        btVector3 pos = trans.getOrigin();
        float BSP_YOffset = 3;

        // El offset es porqué nuestros ojos deberian estar por encima del punto central
        // de la cápsula que hemos utilizando. De lo contrario lo colocaríamos en el centro del mismo la cámara.
        finalVelocity = Vertex3D(pos.getX(), pos.getY() - BSP_YOffset, pos.getZ());
    } else {
        finalVelocity = this->camera->velocity.vertex2;
    }

    return finalVelocity;
}

WeaponsManager *CollisionsManager::getWeaponManager() const {
    return weaponManager;
}

void CollisionsManager::setWeaponManager(WeaponsManager *weaponManager) {
    CollisionsManager::weaponManager = weaponManager;
}

std::vector<Triangle *> &CollisionsManager::getVisibleTriangles() {
    return *visibleTriangles;
}

void CollisionsManager::setVisibleTriangles(std::vector<Triangle *> &visibleTriangles) {
    CollisionsManager::visibleTriangles = &visibleTriangles;
}

std::vector<CollisionResolver *> &CollisionsManager::getCollisions() {
    return collisions;
}

void CollisionsManager::setCollisions(const std::vector<CollisionResolver *> &collisions) {
    CollisionsManager::collisions = collisions;
}
