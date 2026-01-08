#ifndef BRAKEDA3D_COLLISIONABLE_H
#define BRAKEDA3D_COLLISIONABLE_H

#include <BulletCollision/CollisionDispatch/btGhostObject.h>
#include <BulletDynamics/Dynamics/btDiscreteDynamicsWorld.h>
#include "../3D/Vertex3D.h"
#include "M3.h"
#include "CollisionInfo.h"
#include <glm/vec2.hpp>
#include <BulletDynamics/Character/btKinematicCharacterController.h>

enum CollisionShape {
    SIMPLE_SHAPE = 0,
    CAPSULE_SHAPE = 1,
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
    ~Collider() = default;

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
    CollisionShape collisionShape;

    Collider();

    void setCollisionsEnabled(bool value);
    void setCollisionMode(CollisionMode collisionMode);
    void setCollisionShape(CollisionShape collisionShape);
    void RemoveCollisionObject();
    void makeSimpleGhostBody(Vertex3D position, glm::mat4 modelMatrix, Vertex3D dimensions, btDiscreteDynamicsWorld *world, int collisionGroup, int collisionMask);
    void drawImGuiCollisionModeSelector();
    void drawWorldPhysicVariables();
    void setMass(float mass);
    void ApplyImpulse(Vertex3D f, Vertex3D rel);
    void ApplyCentralForce(Vertex3D f);
    void ApplyCentralImpulse(Vertex3D f);
    void setColliderStatic(bool colliderStatic);
    void UpdateShapeCollider();
    void setLinearVelocity(Vertex3D f);
    void setCapsuleColliderSize(float x, float y);
    void setWalkingDirection(Vertex3D d) const;
    void Jump(Vertex3D d) const;
    bool onGround() const;
    void setAngularFactor(const Vertex3D &angularFactor);
    void setFriction(float friction);
    void setLinearDamping(float linearDamping);
    void setAngularDamping(float angularDamping);
    void setRestitution(float restitution);
    void setShapeMargin(float shapeMargin);
    void setCcdMotionThreshold(float ccdMotionThreshold);
    void setCcdSweptSphereRadius(float ccdSweptSphereRadius);
    void SleepCollider() const;
    void DisableSimulationCollider() const;
    void EnableSimulationCollider();
    void DisableDeactivationCollider();
    void setAngularVelocity(Vertex3D f);
    void setGravityCollider(Vertex3D g);
    void setLinearFactor(Vertex3D linearFactor);
    void setScalingCollider(Vertex3D v);
    void moveCollider(Vertex3D v);
    void setSimpleShapeSize(const Vertex3D &simple_shape_size);
    virtual void ResolveCollision(CollisionInfo o);
    virtual void Integrate();
    virtual void SetupGhostCollider(CollisionShape mode) = 0;
    virtual void SetupRigidBodyCollider(CollisionShape shapeMode);
    virtual void setupKinematicCollider();
    virtual void makeGhostBody(btDiscreteDynamicsWorld *world, int collisionGroup, int collisionMask);
    virtual void DrawImGuiCollisionShapeSelector();
    virtual void MakeSimpleRigidBody(float mass, btDiscreteDynamicsWorld *world, int collisionGroup, int collisionMask);
    virtual void MakeKineticBody(float x, float y, btDiscreteDynamicsWorld *world, int collisionGroup, int collisionMask);
    [[nodiscard]] Vertex3D getLinearVelocity() const;
    [[nodiscard]] float getMass() const;
    [[nodiscard]] bool isColliderStatic() const;
    [[nodiscard]] btPairCachingGhostObject *getGhostObject() const;
    [[nodiscard]] btRigidBody *getRigidBody() const;
    [[nodiscard]] bool isCollisionsEnabled() const;
    [[nodiscard]] CollisionMode getCollisionMode() const;
    [[nodiscard]] CollisionShape getCollisionShape() const;
};


#endif //BRAKEDA3D_COLLISIONABLE_H
