#include "../../include/Components/ComponentCollisions.h"
#include "../../include/Brakeza3D.h"

ComponentCollisions::ComponentCollisions() = default;

void ComponentCollisions::onStart() {
    Logging::Log("ComponentCollisions onStart", "ComponentCollisions");
}

void ComponentCollisions::preUpdate() {
}

void ComponentCollisions::onUpdate() {
    this->stepSimulation();
}

void ComponentCollisions::postUpdate() {
}

void ComponentCollisions::onEnd() {

}

void ComponentCollisions::onSDLPollEvent(SDL_Event *e, bool &finish) {
}

void ComponentCollisions::initBulletSystem() {
    ///collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration.
    this->collisionConfiguration = new btDefaultCollisionConfiguration();

    ///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
    this->dispatcher = new btCollisionDispatcher(collisionConfiguration);

    ///btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
    this->overlappingPairCache = new btDbvtBroadphase();

    btVector3 worldMin(-50000, -50000, -50000);
    btVector3 worldMax(50000, 50000, 50000);

    auto *sweepBP = new btAxisSweep3(worldMin, worldMax);
    this->overlappingPairCache = sweepBP;

    ///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
    this->solver = new btSequentialImpulseConstraintSolver;

    /// Debug drawer
    this->debugDraw = new PhysicsDebugDraw(this->camera);

    this->dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);
    this->dynamicsWorld->setGravity(btVector3(0, EngineSetup::get()->gravity.y, 0));

    this->dynamicsWorld->setDebugDrawer(debugDraw);
    this->dynamicsWorld->getDebugDrawer()->setDebugMode(PhysicsDebugDraw::DBG_DrawWireframe);

    this->overlappingPairCache->getOverlappingPairCache()->setInternalGhostPairCallback( new btGhostPairCallback() );

    /*this->dynamicsWorld->addCollisionObject(
        this->camera->m_ghostObject,
        btBroadphaseProxy::CharacterFilter,
        btBroadphaseProxy::StaticFilter
    );*/

    //this->makeGhostForCamera();
}

btDiscreteDynamicsWorld *ComponentCollisions::getDynamicsWorld() const {
    return dynamicsWorld;
}

void ComponentCollisions::setDynamicsWorld(btDiscreteDynamicsWorld *world) {
    ComponentCollisions::dynamicsWorld = world;
}

Camera3D *ComponentCollisions::getCamera() const {
    return camera;
}

void ComponentCollisions::setCamera(Camera3D *cam) {
    ComponentCollisions::camera = cam;
}

BSPMap *ComponentCollisions::getBspMap() const {
    return bspMap;
}

void ComponentCollisions::setBSPMap(BSPMap *map) {
    ComponentCollisions::bspMap = map;
}

bool ComponentCollisions::needsCollision(const btCollisionObject *body0, const btCollisionObject *body1) {
    bool collides = (body0->getBroadphaseHandle()->m_collisionFilterGroup &
                     body1->getBroadphaseHandle()->m_collisionFilterMask) != 0;
    collides = collides && (body1->getBroadphaseHandle()->m_collisionFilterGroup &
                            body0->getBroadphaseHandle()->m_collisionFilterMask);

    return collides;
}

void ComponentCollisions::checkCollisionsForAll() {
    if (!SETUP->BULLET_CHECK_ALL_PAIRS) return;

    // All frameCollisions pairs
    int numManifolds = this->dynamicsWorld->getDispatcher()->getNumManifolds();

    for (int i = 0; i < numManifolds; i++) {

        btPersistentManifold *contactManifold = this->dynamicsWorld->getDispatcher()->getManifoldByIndexInternal(i);

        if (contactManifold->getNumContacts() > 0) {
            const btCollisionObject *obA = contactManifold->getBody0();
            const btCollisionObject *obB = contactManifold->getBody1();

            auto *brkObjectA = (Collisionable *) obA->getUserPointer();
            auto *brkObjectB = (Collisionable *) obB->getUserPointer();

            brkObjectA->dispatchCollision(brkObjectB);
            brkObjectB->dispatchCollision(brkObjectA);
        }
    }
}

void ComponentCollisions::updatePhysicObjects() {
    std::vector<Object3D *>::iterator it;
    for (it = getSceneObjects()->begin(); it != getSceneObjects()->end(); it++) {
        auto *collisionable = dynamic_cast<Collisionable *> (*it);
        if (collisionable != nullptr) {
            if (!collisionable->isCollisionsEnabled()) continue;
            collisionable->integrate();
        }
    }
}

void ComponentCollisions::stepSimulation() {
    // check for frameCollisions
    checkCollisionsForAll();

    if (SETUP->BULLET_STEP_SIMULATION) {

        // Bullet Step Simulation
        getDynamicsWorld()->stepSimulation(Brakeza3D::get()->getDeltaTime() * 1000);

        // Physics for meshes
        this->updatePhysicObjects();

        // Sync Ghost with bullet
        //this->updatePhysicsGhosts();

        //this->syncTriggerGhostCamera();
    }
}

std::vector<Triangle *> &ComponentCollisions::getVisibleTriangles() const {
    return *visibleTriangles;
}

void ComponentCollisions::setVisibleTriangles(std::vector<Triangle *> &newVisibleTriangles) {
    ComponentCollisions::visibleTriangles = &newVisibleTriangles;
}

void ComponentCollisions::syncTriggerGhostCamera() {
    Vertex3D direction = camera->getRotation().getTranspose() * SETUP->forward;
    Vertex3D p = camera->getPosition();

    float farDist = 1;
    p.x = p.x + direction.x * farDist;
    p.y = p.y + direction.y * farDist;
    p.z = p.z + direction.z * farDist;

    btTransform t;
    t.setIdentity();
    t.setOrigin(btVector3(p.x, p.y, p.z));
    this->triggerCamera->getGhostObject()->setWorldTransform(t);
}

Mesh3DGhost *ComponentCollisions::getTriggerCamera() const {
    return triggerCamera;
}

void ComponentCollisions::makeGhostForCamera() {
    triggerCamera = new Mesh3DGhost();
    triggerCamera->setLabel(SETUP->cameraTriggerNameIdentifier);
    triggerCamera->setEnabled(true);
    triggerCamera->setPosition(camera->getPosition());
    triggerCamera->getGhostObject()->setCollisionShape(getCamera()->getGhostObject()->getCollisionShape());
    triggerCamera->getGhostObject()->setUserPointer(triggerCamera);
    dynamicsWorld->addCollisionObject(triggerCamera->getGhostObject(), EngineSetup::collisionGroups::CameraTrigger,
                                      EngineSetup::collisionGroups::DefaultFilter |
                                      EngineSetup::collisionGroups::BSPHullTrigger);
}

void ComponentCollisions::setTriggerCamera(Mesh3DGhost *ghost) {
    ComponentCollisions::triggerCamera = ghost;
}