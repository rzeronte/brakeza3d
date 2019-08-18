
#ifndef BRAKEDA3D_COLLISIONSMANAGER_H
#define BRAKEDA3D_COLLISIONSMANAGER_H


#include "../Physics/Mesh3DGhost.h"
#include "../Render/BSPMap.h"
#include "../Render/PhysicsDebugDraw.h"
#include "../Physics/Mesh3DBody.h"
#include "../Physics/SpriteDirectional3DBody.h"

class CollisionsManager {
public:
    Camera3D *camera;
    Mesh3DGhost *triggerCamera;
    BSPMap *bspMap;
    std::vector<Object3D*> *gameObjects;
    std::vector<Mesh3DBody *> *meshPhysics;
    std::vector<SpriteDirectional3DBody *> *projectilePhysics;

    ///collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration.
    btDefaultCollisionConfiguration* collisionConfiguration;
    ///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
    btCollisionDispatcher* dispatcher;
    ///btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
    btBroadphaseInterface* overlappingPairCache;
    ///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
    btSequentialImpulseConstraintSolver* solver;
    btDiscreteDynamicsWorld* dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);
    PhysicsDebugDraw* debugDraw;

    CollisionsManager();

    void initBulletSystem();
    void makeGhostForCamera();
    void checkCollisionsForTriggerCamera();
    void checkCollisionsForAll();

    btDiscreteDynamicsWorld *getDynamicsWorld() const;
    void setDynamicsWorld(btDiscreteDynamicsWorld *dynamicsWorld);

    Camera3D *getCamera() const;
    void setCamera(Camera3D *camera);

    Mesh3DGhost *getTriggerCamera() const;
    void setTriggerCamera(Mesh3DGhost *triggerCamera);

    BSPMap *getBspMap() const;
    void setBspMap(BSPMap *bspMap);

    std::vector<Object3D *> *getGameObjects() const;
    void setGameObjects(std::vector<Object3D *> *gameObjects);

    std::vector<Mesh3DBody *> *getMeshPhysics() const;
    void setMeshPhysics(std::vector<Mesh3DBody *> *meshPhysics);

    std::vector<SpriteDirectional3DBody *> *getProjectilePhysics() const;
    void setProjectilePhysics(std::vector<SpriteDirectional3DBody *> *projectilePhysics);

    bool needsCollision(const btCollisionObject* body0, const btCollisionObject* body1);

    void updatePhysicObjects();

    Vertex3D stepSimulation(float time);
};


#endif //BRAKEDA3D_COLLISIONSMANAGER_H
