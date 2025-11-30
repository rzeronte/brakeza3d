
#ifndef BRAKEDA3D_COMPONENTCOLLISIONS_H
#define BRAKEDA3D_COMPONENTCOLLISIONS_H

#include "../3D/BSPMap.h"
#include "../Render/PhysicsDebugDraw.h"
#include "Component.h"

class ComponentCollisions : public Component {
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

    std::vector<Vector3D> debugDrawLinesCache;
public:
    ComponentCollisions();
    ~ComponentCollisions() override;

    bool isRayCollisionWith(const Vertex3D &from, const Vertex3D &to, const Object3D *o) const;
    void onStart() override;
    void preUpdate() override;
    void onUpdate() override;
    void postUpdate() override;
    void onEnd() override;
    void onSDLPollEvent(SDL_Event *event, bool &finish) override;
    void InitBulletSystem();
    void CheckCollisionsForAll() const;
    void UpdatePhysicObjects() const;
    void StepSimulation(float deltaTime);
    void setGravity(const Vertex3D &vertex3D) const;
    void ClearDebugCache();
    void AddVector3DIntoCache(const Vector3D &v);
    void DrawDebugCache() const;
    void setEnabled(bool enabled) override;
    void setEnableDebugMode(bool value) const;
    [[nodiscard]] btDiscreteDynamicsWorld *getDynamicsWorld() const;
    static void demoProjectile(int type);
};


#endif //BRAKEDA3D_COMPONENTCOLLISIONS_H
