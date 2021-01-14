#include "../../headers/Components/ComponentCollisions.h"
#include "../../headers/Collisions/CollisionResolverBetweenProjectileAndNPCEnemy.h"

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

    this->dynamicsWorld->setDebugDrawer(debugDraw);
    this->dynamicsWorld->getDebugDrawer()->setDebugMode(PhysicsDebugDraw::DBG_DrawWireframe);
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

BSPMap *ComponentCollisions::getBspMap() const {
    return bspMap;
}

void ComponentCollisions::setBspMap(BSPMap *bspMap) {
    ComponentCollisions::bspMap = bspMap;
}

bool ComponentCollisions::needsCollision(const btCollisionObject* body0, const btCollisionObject* body1)
{
    bool collides = (body0->getBroadphaseHandle()->m_collisionFilterGroup & body1->getBroadphaseHandle()->m_collisionFilterMask) != 0;
    collides = collides && (body1->getBroadphaseHandle()->m_collisionFilterGroup & body0->getBroadphaseHandle()->m_collisionFilterMask);

    return collides;
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
}

Vertex3D ComponentCollisions::stepSimulation()
{
    float time = Brakeza3D::get()->getDeltaTime();

    // check for collisions
    if (bspMap->isLoaded()) {
        checkCollisionsForAll();
    }

    if (SETUP->BULLET_STEP_SIMULATION) {
        // Bullet Step Simulation
        getDynamicsWorld()->stepSimulation( time * SETUP->BULLET_STEP_SIMULATION_MULTIPLIER );
        // Physics for meshes
        this->updatePhysicObjects();
    }

    this->finalVelocity = this->camera->velocity.vertex2;
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

void ComponentCollisions::SV_AddGravity (float deltaTime)
{
    float ent_gravity = 1.0;
    this->getCamera()->velocity.vertex2.y -= ent_gravity * 9.8 * deltaTime;
}
