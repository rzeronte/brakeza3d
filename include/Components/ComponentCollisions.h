
#ifndef BRAKEDA3D_COMPONENTCOLLISIONS_H
#define BRAKEDA3D_COMPONENTCOLLISIONS_H

#include "../Render/PhysicsDebugDraw.h"
#include "Component.h"
#include <btBulletDynamicsCommon.h>

class ComponentCollisions : public Component
{
    btDefaultCollisionConfiguration *collisionConfiguration = nullptr;
    btCollisionDispatcher *dispatcher = nullptr;
    btBroadphaseInterface *overlappingPairCache = nullptr;
    btGhostPairCallback *ghostPairCallback = nullptr;
    btSequentialImpulseConstraintSolver *solver = nullptr;
    btDiscreteDynamicsWorld *dynamicsWorld = nullptr;
    PhysicsDebugDraw *debugDraw = nullptr;
    std::vector<Vector3D> debugDrawLinesCache;
public:
    ComponentCollisions() = default;
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
};


#endif //BRAKEDA3D_COMPONENTCOLLISIONS_H
