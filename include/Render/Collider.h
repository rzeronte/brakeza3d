#ifndef BRAKEDA3D_COLLISIONABLE_H
#define BRAKEDA3D_COLLISIONABLE_H

#include <BulletCollision/CollisionDispatch/btGhostObject.h>
#include <BulletDynamics/Dynamics/btDiscreteDynamicsWorld.h>
#include "../3D/Vertex3D.h"
#include "M3.h"
#include "CollisionInfo.h"
#include <glm/vec2.hpp>
#include <BulletDynamics/Character/btKinematicCharacterController.h>
#include <vector>

enum CollisionShape {
    SIMPLE_SHAPE = 0,
    CAPSULE = 1,
    TRIANGLE_MESH_SHAPE = 2,
};

enum CollisionMode {
    NONE = -1,
    GHOST = 0,
    BODY = 1,
    KINEMATIC = 2
};

class Collider {
public:
    Vertex3D simpleShapeSize;
    Vertex3D angularFactor;
    Vertex3D linearFactor;

protected:
    bool collisionsEnabled;

    CollisionMode collisionMode;

    //ghost
    btPairCachingGhostObject *ghostObject;

    // Body
    btRigidBody *body;
    bool colliderStatic;
    float mass;
    float friction;
    float linearDamping;
    float angularDamping;
    float restitution;
    float shapeMargin;
    float ccdMotionThreshold;
    float ccdSweptSphereRadius;

    //kinematic
    btPairCachingGhostObject *kinematicBody;
    glm::vec2 kinematicCapsuleSize;
    btKinematicCharacterController *characterController;
public:
    Collider();

    virtual void resolveCollision(CollisionInfo o);
    virtual void integrate();

    [[nodiscard]] bool isCollisionsEnabled() const;
    void setCollisionsEnabled(bool value);

    [[nodiscard]] CollisionMode getCollisionMode() const;
    void setCollisionMode(CollisionMode collisionMode);

    [[nodiscard]] CollisionShape getCollisionShape() const;
    void setCollisionShape(CollisionShape collisionShape);

    void removeCollisionObject();

    virtual void setupGhostCollider(CollisionShape mode) = 0;

    virtual void setupRigidBodyCollider(CollisionShape shapeMode);

    virtual void setupKinematicCollider();

    virtual //ghost
    void makeGhostBody(btDiscreteDynamicsWorld *world, int collisionGroup, int collisionMask);
    void makeSimpleGhostBody(
        Vertex3D position,
        glm::mat4 modelMatrix,
        Vertex3D dimensions,
        btDiscreteDynamicsWorld *world,
        int collisionGroup,
        int collisionMask
    );

    [[nodiscard]] btPairCachingGhostObject *getGhostObject() const;
    [[nodiscard]] btRigidBody *getRigidBody() const;

    void drawImGuiCollisionModeSelector();

    virtual void drawImGuiCollisionShapeSelector();

    void drawImGuiVariables();

    CollisionShape collisionShape;

    virtual void makeSimpleRigidBody(
        float mass,
        btDiscreteDynamicsWorld *world,
        int collisionGroup,
        int collisionMask
    );

    virtual void makeKineticBody(
        float x,
        float y,
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

    void UpdateShapeCollider();

    void setLinearVelocity(Vertex3D f);

    void setCapsuleColliderSize(float x, float y);

    void setWalkingDirection(Vertex3D d);

    void jump(Vertex3D d);

    bool onGround();

    void setAngularFactor(const Vertex3D &angularFactor);

    void setFriction(float friction);

    [[nodiscard]] Vertex3D getLinearVelocity() const;

    void setLinearDamping(float linearDamping);

    void setAngularDamping(float angularDamping);

    void setRestitution(float restitution);

    void setShapeMargin(float shapeMargin);

    void setCcdMotionThreshold(float ccdMotionThreshold);

    void setCcdSweptSphereRadius(float ccdSweptSphereRadius);

    void sleepCollider();

    void disableSimulationCollider();

    void enableSimulationCollider();

    void disableDeactivationCollider();

    void setAngularVelocity(Vertex3D f);

    void setGravityCollider(Vertex3D g);

    void setLinearFactor(Vertex3D linearFactor);

    void setScalingCollider(Vertex3D v);

    void moveCollider(Vertex3D v);

    void setSimpleShapeSize(const Vertex3D &simple_shape_size);
};


#endif //BRAKEDA3D_COLLISIONABLE_H
