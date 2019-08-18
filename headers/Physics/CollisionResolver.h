
#ifndef BRAKEDA3D_COLLISIONRESOLVER_H
#define BRAKEDA3D_COLLISIONRESOLVER_H


#include "Mesh3DGhost.h"
#include "../Render/BSPMap.h"
#include "../Render/PhysicsDebugDraw.h"
#include "Mesh3DBody.h"

class CollisionResolver {
public:
    Camera3D *camera;
    Mesh3DGhost *triggerCamera;
    BSPMap *bspMap;
    std::vector<Object3D*> *gameObjects;

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

    CollisionResolver();

    void initBulletSystem();
    void makeGhostForCamera();
    void checkTriggerCamera();
    void checkAll();

    void moveMesh3DBody(Mesh3DBody *oRemoteBody, int targetEntityId);


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
};


#endif //BRAKEDA3D_COLLISIONRESOLVER_H
