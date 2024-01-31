
#ifndef BRAKEDA3D_COMPONENTCOLLISIONS_H
#define BRAKEDA3D_COMPONENTCOLLISIONS_H

#include "../Render/BSPMap.h"
#include "../Render/PhysicsDebugDraw.h"
#include "../Physics/Mesh3DBody.h"
#include "../Physics/Mesh3DGhost.h"
#include "../Physics/SpriteDirectional3DBody.h"
#include "Component.h"

class ComponentCollisions : public Component {
private:
    BSPMap *bspMap;

    ///collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration.
    btDefaultCollisionConfiguration *collisionConfiguration;
    ///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
    btCollisionDispatcher *dispatcher;
    ///btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
    btBroadphaseInterface *overlappingPairCache;

    btGhostPairCallback *ghostPairCallback;

    ///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
    btSequentialImpulseConstraintSolver *solver;
    btDiscreteDynamicsWorld *dynamicsWorld;
    PhysicsDebugDraw *debugDraw;

public:
    ComponentCollisions();

    void onStart() override;

    void preUpdate() override;

    void onUpdate() override;

    void postUpdate() override;

    void onEnd() override;

    void onSDLPollEvent(SDL_Event *event, bool &finish) override;

    void initBulletSystem();

    void checkCollisionsForAll();

    [[nodiscard]] btDiscreteDynamicsWorld *getDynamicsWorld() const;

    [[nodiscard]] BSPMap *getBspMap() const;

    void setBSPMap(BSPMap *bspMap);

    void updatePhysicObjects();

    void stepSimulation(float deltaTime);

    static void demoProjectile(int type);

    virtual ~ComponentCollisions();
};


#endif //BRAKEDA3D_COMPONENTCOLLISIONS_H
