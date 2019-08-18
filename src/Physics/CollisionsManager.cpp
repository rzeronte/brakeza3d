#include "../../headers/Physics/CollisionsManager.h"
#include "BulletCollision/CollisionDispatch/btGhostObject.h"
#include "../../headers/Render/Engine.h"

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
    triggerCamera->setLabel("triggerCamera");
    triggerCamera->setEnabled(true);
    triggerCamera->setPosition(*camera->getPosition());
    triggerCamera->getGhostObject()->setCollisionShape(camera->kinematicController->getGhostObject()->getCollisionShape());
    triggerCamera->getGhostObject()->setUserPointer(triggerCamera);
    dynamicsWorld->addCollisionObject(triggerCamera->getGhostObject(), collisionGroups::CameraTrigger, collisionGroups::DefaultFilter|collisionGroups::BSPHullTrigger);
}

void CollisionsManager::moveMesh3DBody(Mesh3DBody *oRemoteBody, int targetEntityId) {

    if ( oRemoteBody->isMoving()|| oRemoteBody->isReverseMoving() || oRemoteBody->isWaiting()) return;

    char *angle = bspMap->getEntityValue(targetEntityId, "angle");
    char *speed = bspMap->getEntityValue(targetEntityId, "speed");

    float angleFloat = atof( std::string(angle).c_str() );
    float speedFloat = atof( std::string(speed).c_str() );

    oRemoteBody->setMoving(true);
    oRemoteBody->setAngleMoving(angleFloat);

    if (speedFloat > 0) {
        oRemoteBody->setSpeedMoving(speedFloat);
    }
}


bool CollisionsManager::needsCollision(const btCollisionObject* body0, const btCollisionObject* body1)
{
    bool collides = (body0->getBroadphaseHandle()->m_collisionFilterGroup & body1->getBroadphaseHandle()->m_collisionFilterMask) != 0;
    collides = collides && (body1->getBroadphaseHandle()->m_collisionFilterGroup & body0->getBroadphaseHandle()->m_collisionFilterMask);

    return collides;
}

void CollisionsManager::checkTriggerCamera()
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

void CollisionsManager::checkAll()
{
// All collisions pairs
    int numManifolds = this->dynamicsWorld->getDispatcher()->getNumManifolds();
    if (EngineSetup::getInstance()->BULLET_CHECK_ALL_PAIRS) {
        for (int i = 0; i < numManifolds; i++) {
            btPersistentManifold *contactManifold = this->dynamicsWorld->getDispatcher()->getManifoldByIndexInternal(i);
            if (contactManifold->getNumContacts() > 0) {
                const btCollisionObject *obA = contactManifold->getBody0();
                const btCollisionObject *obB = contactManifold->getBody1();

                Object3D *brkObjectA = (Object3D *) obA->getUserPointer();
                Object3D *brkObjectB = (Object3D *) obB->getUserPointer();

                BSPMap *oMap = dynamic_cast<BSPMap*> (brkObjectB);
                if (oMap != NULL) {

                    if (brkObjectA->getLabel() == "projectile") {
                        dynamicsWorld->removeCollisionObject((btCollisionObject *) obA);
                        //this->removeObject3D(brkObjectA);
                    }

                    if (EngineSetup::getInstance()->LOG_COLLISION_OBJECTS) {
                        Logging::getInstance()->getInstance()->Log("[AllPairs] Collision between " + brkObjectA->getLabel() + " and BSPMap");
                    }
                }

                SpriteDirectional3D *oSpriteDirectional = dynamic_cast<SpriteDirectional3D*> (brkObjectB);
                if (oSpriteDirectional != NULL) {

                    Enemy *oSpriteDirectionalEnemyA = dynamic_cast<Enemy*> (brkObjectA);
                    Enemy *oSpriteDirectionalEnemyB = dynamic_cast<Enemy*> (brkObjectB);

                    if ( ( oSpriteDirectionalEnemyA != NULL || oSpriteDirectionalEnemyB != NULL) &&
                         ( brkObjectA->getLabel() == "projectile" || brkObjectB->getLabel() == "projectile" )
                            ) {
                        if (EngineSetup::getInstance()->LOG_COLLISION_OBJECTS) {
                            Logging::getInstance()->getInstance()->Log("[AllPairs] Collision between projectile and Enemy");
                        }
                        if (oSpriteDirectionalEnemyA != NULL) {
                            oSpriteDirectionalEnemyA->stamina -= 10; //weapon->getCurrentWeaponType()->getDamage();

                            if (oSpriteDirectionalEnemyA->stamina < 0) {
                                //SpriteDirectional3D *tmp = dynamic_cast<SpriteDirectional3D*> (brkObjectA);
                                //tmp->setAnimation(EngineSetup::getInstance()->SpriteDoom2SoldierAnimations::SOLDIER_DEAD);
                                //dynamicsWorld->removeCollisionObject( (btCollisionObject *) obA );
                            }

                        } else {
                            dynamicsWorld->removeCollisionObject( (btCollisionObject *) obB );
                            //this->removeObject3D( brkObjectB );
                        }

                        if (oSpriteDirectionalEnemyB != NULL) {
                            oSpriteDirectionalEnemyB->stamina--;
                            if (oSpriteDirectionalEnemyB->stamina < 0) {
                                //SpriteDirectional3D *tmp = dynamic_cast<SpriteDirectional3D*> (brkObjectB);
                                //tmp->setAnimation(EngineSetup::getInstance()->SpriteDoom2SoldierAnimations::SOLDIER_DEAD);
                                //dynamicsWorld->removeCollisionObject( (btCollisionObject *) obB );

                            }

                        } else {
                            dynamicsWorld->removeCollisionObject( (btCollisionObject *) obA );
                            //this->removeObject3D( brkObjectA );
                        }
                    }

                }

                Mesh3D *oMesh = dynamic_cast<Mesh3D*> (brkObjectB);
                if (oMesh != NULL) {
                    int originalEntityIndex = oMesh->getBspEntityIndex();

                    if (originalEntityIndex > 0) {
                        char *classname = bspMap->getEntityValue(originalEntityIndex, "classname");
                        char *currentTargetName = bspMap->getEntityValue(originalEntityIndex, "targetname");

                        if ( !strcmp(classname, "func_door") ) {
                            if (EngineSetup::getInstance()->LOG_COLLISION_OBJECTS) {
                                Logging::getInstance()->getInstance()->Log("[AllPairs] Collision with func_door");
                            }
                            if (!bspMap->hasEntityAttribute(originalEntityIndex, "targetname")) {
                                // No tiene targetname
                                Mesh3DBody *originalBody = dynamic_cast<Mesh3DBody*> (brkObjectB);
                                if (originalBody != NULL) {
                                    this->moveMesh3DBody(originalBody, originalEntityIndex);
                                    if (EngineSetup::getInstance()->LOG_COLLISION_OBJECTS) {
                                        Logging::getInstance()->getInstance()->Log("moveMesh3DBody: " + originalBody->getLabel());
                                    }
                                }
                            } else {
                                int targetRemoteEntityId = bspMap->getIndexOfFirstEntityByTarget( currentTargetName );
                                char *classnameRemote = bspMap->getEntityValue(targetRemoteEntityId, "classname");

                                if ( !strcmp(classnameRemote, "trigger_counter") ) {
                                    for (int k = 0; k < this->gameObjects->size(); k++) {
                                        Mesh3D *oRemoteMesh = dynamic_cast<Mesh3D*> ((*this->gameObjects)[k]);
                                        if (oRemoteMesh != NULL) {
                                            if (oRemoteMesh->getBspEntityIndex() == targetRemoteEntityId) {
                                                Mesh3DGhost *oRemoteGhost = dynamic_cast<Mesh3DGhost*> (oRemoteMesh);
                                                int currentCounter = oRemoteGhost->currentTriggerCounter;

                                                char *countValue = bspMap->getEntityValue(targetRemoteEntityId, "count");
                                                int countValueInt = atoi( std::string(countValue).c_str() );

                                                if (countValueInt == currentCounter) {
                                                    Mesh3DBody *originalBody = dynamic_cast<Mesh3DBody*> (brkObjectB);

                                                    this->moveMesh3DBody(originalBody, originalEntityIndex);
                                                } else {
                                                    if (strlen(bspMap->getEntityValue(originalEntityIndex, "message")) > 0) {
                                                        //Tools::writeTextCenter(Engine::renderer, Engine::font, Color::white(), std::string(bspMap->getEntityValue(originalEntityIndex, "message")) );
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }

                        //***************************
                        if ( !strcmp(classname, "func_button") ) {
                            char *targetRemote = bspMap->getEntityValue(originalEntityIndex, "target");
                            int targetRemoteEntityId = bspMap->getIndexOfFirstEntityByTargetname(targetRemote );

                            Mesh3DBody *originalBody = dynamic_cast<Mesh3DBody*> (brkObjectB);
                            this->moveMesh3DBody(originalBody, originalEntityIndex);

                            if (targetRemoteEntityId >= 0) {
                                char *classnameRemote = bspMap->getEntityValue(targetRemoteEntityId, "classname");
                                if (!strcmp(classnameRemote, "func_door")) {
                                    // Buscamos algún objeto cuya BSPEntity coincida
                                    for (int k = 0; k < this->gameObjects->size(); k++) {
                                        Mesh3D *oRemoteMesh = dynamic_cast<Mesh3D*> ((*this->gameObjects)[k]);
                                        if (oRemoteMesh != NULL) {
                                            if (oRemoteMesh->getBspEntityIndex() == targetRemoteEntityId) {

                                                Mesh3DBody *oRemoteBody = dynamic_cast<Mesh3DBody*> (oRemoteMesh);
                                                this->moveMesh3DBody(oRemoteBody, targetRemoteEntityId);

                                                if (EngineSetup::getInstance()->LOG_COLLISION_OBJECTS) {
                                                    Logging::getInstance()->getInstance()->Log("Moving Door: " + oRemoteBody->getLabel());
                                                }
                                            }
                                        }
                                    }
                                }

                                if (!strcmp(classnameRemote, "trigger_counter") ) {
                                    // Si el objeto original era un botón
                                    if (!strcmp(classname, "func_button")) {
                                        Mesh3DBody *oButton = dynamic_cast<Mesh3DBody*> (brkObjectB);
                                        if (oButton->active) {
                                            for (int k = 0; k < this->gameObjects->size(); k++) {
                                                Mesh3D *oRemoteMesh = dynamic_cast<Mesh3D*> ((*this->gameObjects)[k]);
                                                if (oRemoteMesh != NULL) {
                                                    if (oRemoteMesh->getBspEntityIndex() == targetRemoteEntityId) {

                                                        Mesh3DGhost *oRemoteGhost = dynamic_cast<Mesh3DGhost*> (oRemoteMesh);
                                                        oRemoteGhost->currentTriggerCounter++;
                                                        oButton->active = false;
                                                        if (EngineSetup::getInstance()->LOG_COLLISION_OBJECTS) {
                                                            Logging::getInstance()->getInstance()->Log("trigger_counter for BSPEntity: " + std::to_string(targetRemoteEntityId) + "=" + std::to_string(oRemoteGhost->currentTriggerCounter));
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    } else {
                        if (EngineSetup::getInstance()->LOG_COLLISION_OBJECTS) {
                            Logging::getInstance()->getInstance()->Log("[AllPairs] Collision between " + brkObjectA->getLabel() + " and " + brkObjectB->getLabel());
                        }
                    }
                }
            }
        }
    }
}

void CollisionsManager::updatePhysicObjects()
{
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
    checkTriggerCamera();
    checkAll();

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