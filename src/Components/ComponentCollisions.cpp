#include "../../headers/Components/ComponentCollisions.h"
#include "BulletCollision/CollisionDispatch/btGhostObject.h"
#include "../../headers/Collisions/CollisionResolverBetweenProjectileAndNPCEnemy.h"
#include "../../headers/Collisions/CollisionResolverBetweenEnemyPartAndBSPMap.h"

ComponentCollisions::ComponentCollisions()
{

}

void ComponentCollisions::onStart() {
    std::cout << "ComponentCollisions onStart" << std::endl;

    this->initBulletSystem();
}

void ComponentCollisions::preUpdate()
{
}

void ComponentCollisions::onUpdate()
{
    this->stepSimulation();
}

void ComponentCollisions::postUpdate() {

}

void ComponentCollisions::onEnd() {

}

void ComponentCollisions::onSDLPollEvent(SDL_Event *e, bool &finish) {
}

void ComponentCollisions::initBulletSystem()
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
    this->dynamicsWorld->setGravity(btVector3(0, SETUP->gravity.y, 0));

    this->overlappingPairCache->getOverlappingPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());

    this->camera->kinematicController->setGravity( dynamicsWorld->getGravity() );
    this->camera->kinematicController->setFallSpeed( EngineSetup::getInstance()->BULLET_FALL_SPEED);

    this->dynamicsWorld->addCollisionObject(this->camera->m_ghostObject, btBroadphaseProxy::CharacterFilter, btBroadphaseProxy::StaticFilter);
    this->dynamicsWorld->addAction(this->camera->kinematicController);

    this->dynamicsWorld->setDebugDrawer(debugDraw);
    this->dynamicsWorld->getDebugDrawer()->setDebugMode(PhysicsDebugDraw::DBG_DrawWireframe);

    // Detect collisions without affect physics (used by triggers)
    this->makeGhostForCamera();
}

btDiscreteDynamicsWorld *ComponentCollisions::getDynamicsWorld() const {
    return dynamicsWorld;
}

void ComponentCollisions::setDynamicsWorld(btDiscreteDynamicsWorld *dynamicsWorld) {
    ComponentCollisions::dynamicsWorld = dynamicsWorld;
}

Camera3D *ComponentCollisions::getCamera() const {
    return camera;
}

void ComponentCollisions::setCamera(Camera3D *camera) {
    ComponentCollisions::camera = camera;
}

Mesh3DGhost *ComponentCollisions::getTriggerCamera() const {
    return triggerCamera;
}

void ComponentCollisions::setTriggerCamera(Mesh3DGhost *triggerCamera) {
    ComponentCollisions::triggerCamera = triggerCamera;
}

BSPMap *ComponentCollisions::getBspMap() const {
    return bspMap;
}

void ComponentCollisions::setBspMap(BSPMap *bspMap) {
    ComponentCollisions::bspMap = bspMap;
}

void ComponentCollisions::makeGhostForCamera()
{
    triggerCamera = new Mesh3DGhost();
    triggerCamera->setLabel(SETUP->cameraTriggerNameIdentifier);
    triggerCamera->setEnabled(true);
    triggerCamera->setPosition(*camera->getPosition());
    triggerCamera->getGhostObject()->setCollisionShape(camera->kinematicController->getGhostObject()->getCollisionShape());
    triggerCamera->getGhostObject()->setUserPointer(triggerCamera);
    dynamicsWorld->addCollisionObject(triggerCamera->getGhostObject(), EngineSetup::collisionGroups::CameraTrigger, EngineSetup::collisionGroups::DefaultFilter|EngineSetup::collisionGroups::BSPHullTrigger);
}

bool ComponentCollisions::needsCollision(const btCollisionObject* body0, const btCollisionObject* body1)
{
    bool collides = (body0->getBroadphaseHandle()->m_collisionFilterGroup & body1->getBroadphaseHandle()->m_collisionFilterMask) != 0;
    collides = collides && (body1->getBroadphaseHandle()->m_collisionFilterGroup & body0->getBroadphaseHandle()->m_collisionFilterMask);

    return collides;
}

void ComponentCollisions::checkCollisionsForTriggerCamera()
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

                            if (SETUP->LOG_COLLISION_OBJECTS) {
                                Logging::getInstance()->getInstance()->Log( "[LOG_COLLISION_OBJECTS] teleporting to " +std::string(bspMap->getEntityValue(entityIndex, "target")));
                            }
                        }
                    }
                }

                if (!strcmp(classname, "func_door")) {
                    auto *CW = dynamic_cast<ComponentWindow*>((*getComponents())[ComponentID::COMPONENT_WINDOW]);
                    Tools::writeTextCenter( CW->renderer, CW->fontDefault, Color::white(), std::string("func_door") );
                }

                if (!strcmp(classname, "trigger_multiple")) {
                    auto *CW = dynamic_cast<ComponentWindow*>((*getComponents())[ComponentID::COMPONENT_WINDOW]);
                    // check for message response
                    if (strlen(bspMap->getEntityValue(entityIndex, "message")) > 0) {
                        Tools::writeTextCenter( CW->renderer, CW->fontDefault, Color::white(), std::string(bspMap->getEntityValue(entityIndex, "message")) );
                    }
                }
            }
        }

        if (SETUP->LOG_COLLISION_OBJECTS) {
            Logging::getInstance()->getInstance()->Log("[LOG_COLLISION_OBJECTS] Collision between triggerCamera and " + brkObjectB->getLabel());
        }
    }
}

void ComponentCollisions::checkCollisionsForAll()
{
    if (!SETUP->BULLET_CHECK_ALL_PAIRS) return;

    // All collisions pairs
    if (SETUP->BULLET_CHECK_ALL_PAIRS) {
        int numManifolds = this->dynamicsWorld->getDispatcher()->getNumManifolds();
        for (int i = 0; i < numManifolds; i++) {
            btPersistentManifold *contactManifold = this->dynamicsWorld->getDispatcher()->getManifoldByIndexInternal(i);
            if (contactManifold->getNumContacts() > 0) {
                const btCollisionObject *obA = contactManifold->getBody0();
                const btCollisionObject *obB = contactManifold->getBody1();

                auto *brkObjectA = (Object3D *) obA->getUserPointer();
                auto *brkObjectB = (Object3D *) obB->getUserPointer();

                auto *collisionResolver = new CollisionResolver(
                        contactManifold,
                        brkObjectA,
                        brkObjectB,
                        getBspMap(),
                        getVisibleTriangles()
                );
                Brakeza3D::get()->getComponentsManager()->getComponentCollisions()->getCollisions().emplace_back( collisionResolver );

            }
        }
    }
}

void ComponentCollisions::updatePhysicObjects()
{
    std::vector<Object3D *>::iterator it;
    for (it = getSceneObjects()->begin(); it != getSceneObjects()->end(); it++) {
        Body *body = dynamic_cast<Body*> (*it);
        if ( body != nullptr ) {
            if ( !body->bodyEnabled ) continue;
            body->integrate();
        }
    }

    this->syncTriggerGhostCamera();
}

void ComponentCollisions::syncTriggerGhostCamera()
{
    Vertex3D direction = camera->getRotation().getTranspose() * SETUP->forward;
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

Vertex3D ComponentCollisions::stepSimulation()
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
        bulletVelocity = btVector3(vel.x, vel.y, vel.z) / SETUP->AIR_RESISTANCE;
    }

    if (!Tools::isZeroVector(vel)) {
        this->camera->kinematicController->setLinearVelocity(bulletVelocity);
    }

    Vertex3D finalVelocity;

    if (SETUP->BULLET_STEP_SIMULATION) {
        // Bullet Step Simulation
        getDynamicsWorld()->stepSimulation( time);

        // Physics for meshes
        updatePhysicObjects();

        btTransform trans = this->camera->kinematicController->getGhostObject()->getWorldTransform();

        btVector3 pos = trans.getOrigin();
        float BSP_YOffset = 0;

        // El offset es porqué nuestros ojos deberian estar por encima del punto central
        // de la cápsula que hemos utilizando. De lo contrario lo colocaríamos en el centro del mismo la cámara.
        finalVelocity = Vertex3D(pos.getX(), pos.getY() - BSP_YOffset, pos.getZ());
    } else {
        finalVelocity = this->camera->velocity.vertex2;
    }

    this->finalVelocity = finalVelocity;
}

std::vector<Triangle *> &ComponentCollisions::getVisibleTriangles() {
    return *visibleTriangles;
}

void ComponentCollisions::setVisibleTriangles(std::vector<Triangle *> &visibleTriangles) {
    ComponentCollisions::visibleTriangles = &visibleTriangles;
}

std::vector<CollisionResolver *> &ComponentCollisions::getCollisions() {
    return collisions;
}

void ComponentCollisions::setCollisions(const std::vector<CollisionResolver *> &collisions) {
    ComponentCollisions::collisions = collisions;
}

