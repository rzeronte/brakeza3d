---
sidebar_position: 9
title: Collision system
description: Collision system API in Brakeza3D with collision modes, shapes, forces, and bone mapping for animations.
---

# Collision system
---

This section describes the collision system in **Brakeza3D** and how objects can interact physically within
the scene. 


## Collision between objects
---

Collidable objects allow developers to implement logic based on collision events.
In Brakeza3D, all Object3D instances can work with physics and collisions.

Any collidable object will trigger a call to the onCollision method in your LUA scripts for each collision that
occurs.

```lua
function onCollision(with)
    print("Collision with " .. with:getName())
end
```

## Collision Modes
---

In **Brakeza3D**, every physical object has a **collision shape**, which defines the geometric boundaries used for detecting collisions with other objects.  

The collision shape determines how the object interacts with the physics system and can be configured to match the object's visual geometry or simplified for performance.  

Once a collision shape is assigned, you can select the **collision mode** for the object, controlling whether it behaves as a ghost, a rigid body, or a kinematic object. These modes define how the object responds to physics and collisions, and can be set either from the GUI or programmatically using LUA scripts.

### Ghost mode

A **Ghost** is a collidable object that does **not react to physics**.

Ghosts are often used for triggers, sensors, or objects that should detect collisions without being physically affected.  
You can configure an object as a Ghost from the GUI or programmatically using the `setupGhostCollider()` method.


### RigidBody mode

A **RigidBody** is a fully physical object that **reacts to forces, collisions, and gravity**.

RigidBodies are used for objects that should behave realistically in the physics simulation.  
You can configure an object as a RigidBody from the GUI or using the `setupRigidBodyCollider()` method.

In RigidBody collision mode, you can specify whether the geometry is static. Static objects do not
move (their mass is automatically set to 0). This is the only collision mode allowed for non-convex geometries.

### Kinematic mode

The kinematic collision mode provides character-like movement behavior and supports the following methods:

| Function                        | Description                                                                                       |
|---------------------------------|---------------------------------------------------------------------------------------------------|
| `setWalkingDirection(Vertex3D)` | Sets the desired movement direction for a kinematic object, typically used for character motion   |
| `onGround()`                    | Returns `true` if the kinematic object is currently in contact with the ground, otherwise `false` |
| `Jump()`                        | Makes the kinematic object perform a jump according to its current physics settings               |


## Collider Shape
---

Brakeza3D provides several **collider shapes** that define the physical boundaries of objects for collision detection:

- `CollisionShape.SIMPLE_SHAPE`
- `CollisionShape.CAPSULE_SHAPE`
- `CollisionShape.TRIANGLE3D_MESH_SHAPE`

### Simple Shape
A basic collider shape suitable for most objects. Offers **excellent performance** and is supported by any `Object3D`.

```json
eye = ObjectFactory.Mesh3D(
  "../assets/models/Eye.fbx",
  Vertex3D.new(0, 0, 10)
)
eye:setCollisionsEnabled(true)                              -- enable collider
eye:setupGhostCollider(CollisionShape.SIMPLE_SHAPE);        -- Ghost / SIMPLE_SHAPE
```

### Capsule Shape
A capsule-shaped collider, often used for characters or elongated objects. Like the simple shape, it provides **good performance** and works with any `Object3D`.

```json
eye = ObjectFactory.Mesh3D(
  "../assets/models/Eye.fbx",
  Vertex3D.new(10, 10, 10)
)
eye:setCollisionsEnabled(true)                              -- enable collider
eye:setupRigidBodyCollider(CollisionShape.CAPSULE_SHAPE);   -- RigidBody / CAPSULE_SHAPE
```

### Triangle Mesh Shape
A precise collider that **matches the exact geometry of a `Mesh3D` object**. This shape provides high accuracy for collisions but has a **higher computational cost** compared to simple or capsule shapes. It is ideal for static meshes or situations where precise collision detection is required.
You can configure collider shapes either from the GUI or from LUA scripts.

```json
eye = ObjectFactory.Mesh3D(
    "../assets/models/Eye.fbx",
    Vertex3D.new(10, 10, 10)
)
eye:setCollisionsEnabled(true)                                -- enable collider
eye:setupGhostCollider(CollisionShape.TRIANGLE3D_MESH_SHAPE); -- Ghost / TRIANGLE3D_MESH_SHAPE
```


## Disabling Collisions
---

You can disable collisions for an object using the `setCollisionsEnabled(bool)` method.
This will remove any collision shape previously configured for the object.



## Movement of Collidable Objects
---

Both GHOST and RIGIDBODY objects can be moved within the scene.

    - `setLinearVelocity(Vertex3D)`: Set linear velocity for current collider

### Applying Forces to Rigid Objects

For RigidBody objects, forces can be applied using the following methods:

| Function                               | Description                                                                                               |
|----------------------------------------|-----------------------------------------------------------------------------------------------------------|
| `ApplyCentralImpulse(Vertex3D f)`      | Applies an instantaneous central impulse to the object, affecting its linear velocity                     |
| `ApplyCentralForce(Vertex3D f)`        | Applies a continuous central force to the object, influencing its movement over time                      |
| `ApplyImpulse(Vertex3D f, Vertex3D r)` | Applies an impulse at a specific point relative to the object, affecting both linear and angular velocity |

## Collider Properties
---

You can use the following methods to configure the physical behavior of an object:

| Function                       | Description                                                                          |
|--------------------------------|--------------------------------------------------------------------------------------|
| `setMass(float)`               | Sets the mass of the object, affecting how it responds to forces                     |
| `setFriction(float)`           | Sets the surface friction of the object, controlling resistance to sliding           |
| `setRestitution(float)`        | Sets the bounciness of the object during collisions (0 = no bounce, 1 = full bounce) |
| `setAngularDamping(float)`     | Sets rotational damping, reducing angular velocity over time                         |
| `setLinearDamping(float)`      | Sets linear damping, reducing translational velocity over time                       |
| `setAngularFactor(Vertex3D)`   | Restricts or scales rotation along each axis                                         |
| `setLinearFactor(Vertex3D)`    | Restricts or scales movement along each axis                                         |
| `setGravityCollider(Vertex3D)` | Defines the gravity vector applied to the object                                     |


## Collider and Bone Mapping


Objects of type Mesh3DAnimation can define colliders for the bones used to animate the model geometry.

From LUA scripts, you can determine whether a collision comes from the object's main collider or from a
bone collider, as well as the bone mapping index to which it belongs.

A common use case is defining colliders for bones such as weapons in animations, allowing you to differentiate
collisions coming from a specific bone.

```lua
function onCollision(with)
    if with ~= nil then
        print("Script: Collision With: " .. with:getObject():getName())
        print("Script: Collision Source: " .. with:getSource())
        print("Script: Collision BoneIndexMapping: " .. with:getBoneIndexMapping())
    else
        print("Script: Collision with unknow object")
    end
end
```
