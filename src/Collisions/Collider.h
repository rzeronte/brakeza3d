#ifndef BRAKEDA3D_COLLISIONABLE_H
#define BRAKEDA3D_COLLISIONABLE_H

#include <BulletCollision/CollisionDispatch/btGhostObject.h>
#include <BulletDynamics/Dynamics/btDiscreteDynamicsWorld.h>
#include "../../include/Objects/Vertex3D.h"

enum CollisionShape {
    SIMPLE_SHAPE = 0,
    TRIANGLE_MESH_SHAPE = 1
};

enum CollisionMode {
    NONE = -1,
    GHOST = 0,
    BODY = 1
};

class Collider {

protected:
    bool collisionsEnabled;
    bool colliderStatic;
    float mass;
    Vertex3D simpleShapeSize;
    CollisionMode collisionMode;

    //ghost
    btPairCachingGhostObject *ghostObject;

    // Body
    btRigidBody *body;
public:
    Collider();

    virtual void resolveCollision(Collider *o);
    virtual void integrate();

    [[nodiscard]] bool isCollisionsEnabled() const;
    void setCollisionsEnabled(bool value);

    [[nodiscard]] CollisionMode getCollisionMode() const;
    void setCollisionMode(CollisionMode collisionMode);

    [[nodiscard]] CollisionShape getCollisionShape() const;
    void setCollisionShape(CollisionShape collisionShape);

    void removeCollisionObject() const;

    virtual void setupGhostCollider(CollisionShape mode);

    virtual void setupRigidBodyCollider(CollisionShape shapeMode);

    virtual //ghost
    void makeGhostBody(btDiscreteDynamicsWorld *world, int collisionGroup, int collisionMask);
    void makeSimpleGhostBody(
        Vertex3D dimensions,
        btDiscreteDynamicsWorld *world,
        int collisionGroup,
        int collisionMask
    );

    [[nodiscard]] btPairCachingGhostObject *getGhostObject() const;
    [[nodiscard]] btRigidBody *getRigidBody() const;

    void drawImGuiCollisionModeSelector();

    virtual void drawImGuiCollisionShapeSelector();

    CollisionShape collisionShape;

    virtual void makeSimpleRigidBody(
        float mass,
        btDiscreteDynamicsWorld *world,
        int collisionGroup,
        int collisionMask
    );

    [[nodiscard]] float getMass() const;

    void setMass(float mass);

    void applyImpulse(Vertex3D f, Vertex3D rel);
    void applyCentralForce(Vertex3D f);
    void applyCentralImpulse(Vertex3D f);

    [[nodiscard]] bool isColliderStatic() const;

    void setColliderStatic(bool colliderStatic);

    void UpdateShape();
};


#endif //BRAKEDA3D_COLLISIONABLE_H
