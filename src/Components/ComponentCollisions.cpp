#include "../../include/Components/ComponentCollisions.h"
#include "../../include/Brakeza.h"
#include "../../include/3D/Projectile.h"
#include "../../include/Components/ComponentsManager.h"
#include "../../include/Render/CollisionInfo.h"
#include "../../include/Misc/Logging.h"

ComponentCollisions::ComponentCollisions()
    :
    bspMap(nullptr),
    collisionConfiguration(nullptr),
    dispatcher(nullptr),
    overlappingPairCache(nullptr),
    ghostPairCallback(nullptr),
    solver(nullptr),
    dynamicsWorld(nullptr),
    debugDraw(nullptr)
{
}

void ComponentCollisions::onStart()
{
    Component::onStart();

    setEnabled(true);
}

void ComponentCollisions::preUpdate()
{
    Component::preUpdate();
}

void ComponentCollisions::onUpdate()
{
    Component::onUpdate();
}

void ComponentCollisions::postUpdate()
{
    Component::postUpdate();

    if (!isEnabled()) {
        this->StepSimulation(0);
        return;
    }

    StepSimulation(Brakeza::get()->getDeltaTime());
}

void ComponentCollisions::onEnd()
{
}

void ComponentCollisions::onSDLPollEvent(SDL_Event *e, bool &finish) {
}

void ComponentCollisions::InitBulletSystem()
{
    ///collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration.
    collisionConfiguration = new btDefaultCollisionConfiguration();

    ///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
    dispatcher = new btCollisionDispatcher(collisionConfiguration);

    ///btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
    overlappingPairCache = new btDbvtBroadphase();

    //auto *sweepBP = new btAxisSweep3(btVector3(-50000, -50000, -50000), btVector3(50000, 50000, 50000));
    //this->overlappingPairCache = sweepBP;

    ///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
    solver = new btSequentialImpulseConstraintSolver;

    /// Debug drawer
    debugDraw = new PhysicsDebugDraw();

    dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);
    dynamicsWorld->setGravity(Config::get()->gravity.toBullet());

    dynamicsWorld->setDebugDrawer(debugDraw);
    setEnableDebugMode(false);

    ghostPairCallback = new btGhostPairCallback();
    overlappingPairCache->getOverlappingPairCache()->setInternalGhostPairCallback(ghostPairCallback);
}

void ComponentCollisions::CheckCollisionsForAll() const
{
    if (!SETUP->BULLET_CHECK_ALL_PAIRS) return;

    int numManifolds = this->dynamicsWorld->getDispatcher()->getNumManifolds();

    for (int i = 0; i < numManifolds; i++) {

        btPersistentManifold *contactManifold = this->dynamicsWorld->getDispatcher()->getManifoldByIndexInternal(i);

        if (contactManifold->getNumContacts() > 0) {
            const btCollisionObject *obA = contactManifold->getBody0();
            const btCollisionObject *obB = contactManifold->getBody1();

            auto *brkObjectA = static_cast<Collider *>(obA->getUserPointer());
            auto *brkObjectB = static_cast<Collider *>(obB->getUserPointer());

            auto cIA = CollisionInfo(brkObjectB, obA->getUserIndex(), obA->getUserIndex2());
            auto cIB = CollisionInfo(brkObjectA, obB->getUserIndex(), obB->getUserIndex2());

            brkObjectA->ResolveCollision(cIA);
            brkObjectB->ResolveCollision(cIB);
        }
    }
}

btDiscreteDynamicsWorld *ComponentCollisions::getDynamicsWorld() const {
    return dynamicsWorld;
}

void ComponentCollisions::UpdatePhysicObjects() const
{
    if (!isEnabled()) return;

    for (auto object : Brakeza::get()->getSceneObjects()) {
        if (object->isRemoved()) continue;

        auto *collisionable = dynamic_cast<Collider *> (object);
        if (collisionable != nullptr) {
            if (!collisionable->isCollisionsEnabled()) continue;
            collisionable->Integrate();
        }
    }
}

void ComponentCollisions::StepSimulation(float deltaTime)
{
    ClearDebugCache();

    if (!isEnabled() || SETUP->ENABLE_BULLET_STEP_SIMULATION) {
        getDynamicsWorld()->stepSimulation(
            deltaTime,
            SETUP->BULLET_MAX_SUBSTEPS,
            btScalar(1.) / btScalar(SETUP->BULLET_FIXED_TIME_STEPS)
        );
        UpdatePhysicObjects();
    }

    if (Config::get()->BULLET_DEBUG_MODE) {
        dynamicsWorld->debugDrawWorld();
        DrawDebugCache();
    }

    CheckCollisionsForAll();
}

void ComponentCollisions::demoProjectile(int type)
{
    std::string fileName;
    Logging::Message("Launching %s", fileName.c_str());

    switch (type) {
        case 0:
            fileName = "basic/cube.fbx";
            break;
        case 1:
            fileName = "basic/cone.fbx";
            break;
        case 2:
            fileName = "basic/torus.fbx";
            break;
        case 3:
            fileName = "basic/cilinder.fbx";
            break;
        case 4:
            fileName = "basic/icosphere.fbx";
            break;
        case 5:
            fileName = "basic/piramid.fbx";
            break;
        default:
            fileName = "basic/cube.fbx";

    }
    Camera3D *camera = ComponentsManager::get()->getComponentCamera()->getCamera();

    Vertex3D direction = camera->getRotation().getTranspose() * Config::get()->forward;

    auto *projectile = new Projectile(direction);
    projectile->setCollisionMode(BODY);
    projectile->setCollisionShape(TRIANGLE_MESH_SHAPE);
    projectile->setParent(camera);
    projectile->AssimpLoadGeometryFromFile(std::string(Config::get()->MODELS_FOLDER + fileName));
    projectile->setRotation(M3::getMatrixRotationForEulerAngles(
        static_cast<float>(Tools::random(0, 180)),
        static_cast<float>(Tools::random(0, 180)),
        static_cast<float>(Tools::random(0, 180))
    ));
    projectile->setPosition( camera->getPosition() + direction.getScaled(1));
    projectile->setEnabled(true);
    projectile->makeProjectileRigidBody(
        Config::get()->PROJECTILE_DEMO_MASS,
        direction,
        Config::get()->PROJECTILE_DEMO_IMPULSE,
        Config::get()->PROJECTILE_DEMO_ACCURACY,
        ComponentsManager::get()->getComponentCollisions()->getDynamicsWorld(),
        Config::collisionGroups::AllFilter,
        Config::collisionGroups::AllFilter
    );

    Brakeza::get()->addObject3D(projectile, Brakeza::UniqueObjectLabel("demoProjectile"));
}

ComponentCollisions::~ComponentCollisions()
{
    delete bspMap;
    delete collisionConfiguration;
    delete dispatcher;
    delete overlappingPairCache;
    delete solver;
    delete debugDraw;
    delete dynamicsWorld;
    delete ghostPairCallback;
}

void ComponentCollisions::setGravity(const Vertex3D &v) const
{
    dynamicsWorld->setGravity(v.toBullet());
}

bool ComponentCollisions::isRayCollisionWith(const Vertex3D &from, const Vertex3D &to, const Object3D *o) const
{
    btCollisionWorld::ClosestRayResultCallback rayCallback(
        from.toBullet(),
        to.toBullet()
    );

    rayCallback.m_collisionFilterGroup = Config::collisionGroups::AllFilter;
    rayCallback.m_collisionFilterMask = Config::collisionGroups::AllFilter;
    rayCallback.m_closestHitFraction = 1.0;

    dynamicsWorld->rayTest(from.toBullet(),to.toBullet(),rayCallback);

    if (rayCallback.hasHit()) {
        auto *objectWithCollision = static_cast<Object3D *>(rayCallback.m_collisionObject->getUserPointer());
        if (o == objectWithCollision) {
            return true;
        }
    }

    return false;
}

void ComponentCollisions::ClearDebugCache()
{
    debugDrawLinesCache.clear();
}

void ComponentCollisions::AddVector3DIntoCache(const Vector3D &v)
{
    debugDrawLinesCache.push_back(v);
}

void ComponentCollisions::DrawDebugCache() const
{
    ComponentsManager::get()->getComponentRender()->getShaderOGLLine3D()->renderLines(
        debugDrawLinesCache,
        ComponentsManager::get()->getComponentWindow()->getForegroundFramebuffer(),
        Color::fuchsia()
    );
}

void ComponentCollisions::setEnabled(bool enabled)
{
    Component::setEnabled(enabled);
    Config::get()->ENABLE_BULLET_STEP_SIMULATION = enabled;
}

void ComponentCollisions::setEnableDebugMode(bool value) const
{
    if (value) {
        dynamicsWorld->getDebugDrawer()->setDebugMode(PhysicsDebugDraw::DBG_DrawWireframe);
        Logging::Message("[ComponentCollisions] Physics Debug mode ON");
        return;
    }

    Logging::Message("[ComponentCollisions] Physics Debug mode OFF");
    dynamicsWorld->getDebugDrawer()->setDebugMode(btIDebugDraw::DBG_NoDebug);
}
