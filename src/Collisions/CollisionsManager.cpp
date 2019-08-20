#include "../../headers/Collisions/CollisionsManager.h"
#include "BulletCollision/CollisionDispatch/btGhostObject.h"
#include "../../headers/Render/Engine.h"
#include "../../headers/Collisions/CollisionResolver.h"
#include "../../headers/Collisions/CollisionResolverBetweenProjectileAndNPCEnemy.h"
#include "../../headers/Collisions/CollisionResolverBetweenCamera3DAndFuncDoor.h"
#include "../../headers/Collisions/CollisionResolverBetweenCamera3DAndFuncButton.h"
#include "../../headers/Collisions/CollisionResolverBetweenProjectileAndBSPMap.h"

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


std::vector<Mesh3DBody *> *CollisionsManager::getMeshPhysics() const {
    return meshPhysics;
}

void CollisionsManager::setMeshPhysics(std::vector<Mesh3DBody *> *meshPhysics) {
    CollisionsManager::meshPhysics = meshPhysics;
}

std::vector<SpriteDirectional3DBody *> *CollisionsManager::getProjectilePhysics() const {
    return projectilePhysics;
}

void CollisionsManager::setProjectilePhysics(std::vector<SpriteDirectional3DBody *> *projectilePhysics) {
    CollisionsManager::projectilePhysics = projectilePhysics;
}

void CollisionsManager::makeGhostForCamera()
{
    triggerCamera = new Mesh3DGhost();
    triggerCamera->setLabel(EngineSetup::getInstance()->cameraTriggerNameIdentifier);
    triggerCamera->setEnabled(true);
    triggerCamera->setPosition(*camera->getPosition());
    triggerCamera->getGhostObject()->setCollisionShape(camera->kinematicController->getGhostObject()->getCollisionShape());
    triggerCamera->getGhostObject()->setUserPointer(triggerCamera);
    dynamicsWorld->addCollisionObject(triggerCamera->getGhostObject(), collisionGroups::CameraTrigger, collisionGroups::DefaultFilter|collisionGroups::BSPHullTrigger);
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

                CollisionResolver *collisionResolver = new CollisionResolver(brkObjectA, brkObjectB, getBspMap());

                int collisionType = collisionResolver->getTypeCollision();

                if (!collisionType) continue;

                if ( collisionType == EngineSetup::getInstance()->CollisionResolverTypes::COLLISION_RESOLVER_PROJECTILE_AND_BSPMAP ) {
                    CollisionResolverBetweenProjectileAndBSPMap *resolver = new CollisionResolverBetweenProjectileAndBSPMap(brkObjectA, brkObjectB, getBspMap(), getProjectilePhysics(), getDynamicsWorld(), getWeaponManager());
                    resolver->dispatch();
                    continue;
                }

                if ( collisionType == EngineSetup::getInstance()->CollisionResolverTypes::COLLISION_RESOLVER_PROJECTILE_AND_NPCENEMY ) {
                    CollisionResolverBetweenProjectileAndNPCEnemy *resolver = new CollisionResolverBetweenProjectileAndNPCEnemy(brkObjectA, brkObjectB, getBspMap(), getProjectilePhysics(), getDynamicsWorld(), getWeaponManager());
                    resolver->dispatch();
                    continue;
                }

                if ( collisionType == EngineSetup::getInstance()->CollisionResolverTypes::COLLISION_RESOLVER_CAMERA_AND_FUNCDOOR ) {
                    CollisionResolverBetweenCamera3DAndFuncDoor *resolver = new CollisionResolverBetweenCamera3DAndFuncDoor(brkObjectA, brkObjectB, getBspMap(), getGameObjects());
                    resolver->dispatch();
                    continue;
                }

                if ( collisionType == EngineSetup::getInstance()->CollisionResolverTypes::COLLISION_RESOLVER_CAMERA_AND_FUNCBUTTON ) {
                    CollisionResolverBetweenCamera3DAndFuncButton *resolver = new CollisionResolverBetweenCamera3DAndFuncButton(brkObjectA, brkObjectB, getBspMap(), getGameObjects());
                    resolver->dispatch();
                    continue;
                }
            }
        }
    }
}

void CollisionsManager::updatePhysicObjects()
{
    // projectiles
    std::vector<SpriteDirectional3DBody *>::iterator it;
    for (it = projectilePhysics->begin(); it != projectilePhysics->end(); it++) {
        (*it)->integrate();
    }

    // mesh physics
    std::vector<Mesh3DBody *>::iterator it2;
    for (it2 = meshPhysics->begin() ; it2 != meshPhysics->end() ; it2++) {
        (*it2)->integrate( );
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

Vertex3D CollisionsManager::stepSimulation(float time)
{
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
