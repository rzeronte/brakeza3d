---
sidebar_position: 9
title: Collision system
description: Collision system API in Brakeza3D with setup methods, forces, kinematic control, and bone mapping.
---

# Collision system
---

This section covers the LUA API for collisions in **Brakeza3D**.

:::tip
For conceptual understanding of collision modes and shapes, see [Collision Concepts](../engine-concepts/collisions).
:::

## Collision Callback
---

Any collidable object triggers the `onCollision` method in your LUA scripts:

```lua
function onCollision(with)
    if with ~= nil then
        print("Collision with: " .. with:getObject():getName())
    end
end
```

## Setting Up Colliders
---

### Enable Collisions

```lua
object:setCollisionsEnabled(true)   -- enable
object:setCollisionsEnabled(false)  -- disable and remove collider
```

### Ghost Collider

Creates a collider that detects but doesn't react to physics:

```lua
object:setupGhostCollider(CollisionShape.SIMPLE_SHAPE)
```

### RigidBody Collider

Creates a fully physical collider:

```lua
-- Dynamic rigid body
object:SetupRigidBodyCollider(CollisionShape.SIMPLE_SHAPE, mass, isStatic)

-- Examples
object:SetupRigidBodyCollider(CollisionShape.SIMPLE_SHAPE, 10, false)  -- 10kg dynamic
object:SetupRigidBodyCollider(CollisionShape.SIMPLE_SHAPE, 0, true)    -- static obstacle
```

### Collision Shapes

| Shape | Constant | Use Case |
|-------|----------|----------|
| Simple | `CollisionShape.SIMPLE_SHAPE` | General objects |
| Capsule | `CollisionShape.CAPSULE_SHAPE` | Characters |
| Triangle Mesh | `CollisionShape.TRIANGLE_MESH_SHAPE` | Precise static geometry |

```lua
-- Capsule for character
player:setCollisionsEnabled(true)
player:setupGhostCollider(CollisionShape.CAPSULE_SHAPE)

-- Triangle mesh for terrain
terrain:setCollisionsEnabled(true)
terrain:SetupRigidBodyCollider(CollisionShape.TRIANGLE_MESH_SHAPE, 0, true)
```

### Capsule Size

```lua
object:setCapsuleColliderSize(radius, height)
```

## Kinematic Mode
---

For character-like movement controlled by code:

| Method | Description |
|--------|-------------|
| `setWalkingDirection(Vertex3D)` | Set movement direction |
| `onGround()` | Returns `true` if touching ground |
| `Jump(Vertex3D)` | Perform a jump with given force |

```lua
function onUpdate()
    local input = Components:Input()
    local forward = this:AxisForward()
    local right = this:AxisRight()
    local velocity = Vertex3D.new(0, 0, 0)

    if input:isCharPressed("W") then
        velocity = velocity + forward:getScaled(-speed)
    end
    if input:isCharPressed("S") then
        velocity = velocity + forward:getScaled(speed)
    end
    if input:isCharPressed("A") then
        velocity = velocity + right:getScaled(-speed)
    end
    if input:isCharPressed("D") then
        velocity = velocity + right:getScaled(speed)
    end

    this:setWalkingDirection(velocity)

    -- Jump when on ground
    if input:isCharPressed("SPACE") and this:onGround() then
        this:Jump(Vertex3D.new(0, jumpForce, 0))
    end
end
```

## RigidBody Movement
---

### Velocity

```lua
object:setLinearVelocity(Vertex3D.new(vx, vy, vz))
object:setAngularVelocity(Vertex3D.new(rx, ry, rz))

local vel = object:getLinearVelocity()
```

### Applying Forces

| Method | Description |
|--------|-------------|
| `ApplyCentralForce(Vertex3D)` | Continuous force (use in onUpdate) |
| `ApplyCentralImpulse(Vertex3D)` | Instant impulse (one-time) |
| `ApplyImpulse(Vertex3D f, Vertex3D r)` | Impulse at offset point |

```lua
-- Push object forward continuously
object:ApplyCentralForce(Vertex3D.new(0, 0, -100))

-- Instant kick
object:ApplyCentralImpulse(Vertex3D.new(0, 50, 0))

-- Hit at specific point (causes rotation)
object:ApplyImpulse(
    Vertex3D.new(10, 0, 0),   -- force
    Vertex3D.new(0, 1, 0)     -- offset from center
)
```

## Collider Properties
---

| Method | Description |
|--------|-------------|
| `setMass(float)` | Object mass (affects force response) |
| `setFriction(float)` | Surface friction (0-1) |
| `setRestitution(float)` | Bounciness (0 = none, 1 = full) |
| `setLinearDamping(float)` | Velocity reduction over time |
| `setAngularDamping(float)` | Rotation reduction over time |
| `setLinearFactor(Vertex3D)` | Restrict movement axes |
| `setAngularFactor(Vertex3D)` | Restrict rotation axes |
| `setGravityCollider(Vertex3D)` | Custom gravity for this object |

```lua
-- Bouncy ball
ball:setRestitution(0.9)
ball:setFriction(0.1)

-- Heavy box
box:setMass(100)
box:setFriction(0.8)

-- Lock Y rotation (no tipping over)
object:setAngularFactor(Vertex3D.new(0, 1, 0))

-- Zero gravity object
object:setGravityCollider(Vertex3D.new(0, 0, 0))
```

## Collider State Control
---

| Method | Description |
|--------|-------------|
| `SleepCollider()` | Put collider to sleep (optimization) |
| `DisableSimulationCollider()` | Pause physics simulation |
| `EnableSimulationCollider()` | Resume physics simulation |
| `DisableDeactivationCollider()` | Prevent auto-sleep |
| `RemoveCollisionObject()` | Remove collider completely |

```lua
-- Temporarily disable physics
object:DisableSimulationCollider()

-- Re-enable later
object:EnableSimulationCollider()
```

## Collider Transform
---

| Method | Description |
|--------|-------------|
| `moveCollider(Vertex3D)` | Move collider position |
| `setScalingCollider(Vertex3D)` | Scale collider size |
| `UpdateShapeCollider()` | Refresh collider after changes |
| `setDrawOffset(Vertex3D)` | Visual offset from collider |
| `setColliderStatic(bool)` | Toggle static state |

## Bone Mapping (Animations)
---

For `Mesh3DAnimation` objects, you can detect which bone caused a collision:

```lua
function onCollision(with)
    if with ~= nil then
        local obj = with:getObject()
        local source = with:getSource()           -- "main" or "bone"
        local boneIndex = with:getBoneIndexMapping()

        if source == "bone" then
            print("Hit by bone #" .. boneIndex .. " of " .. obj:getName())
        else
            print("Hit by main collider of " .. obj:getName())
        end
    end
end
```

This is useful for:
- Weapon hit detection (sword, fist)
- Damage zones on enemies
- Precise hit feedback

## Raycasting
---

Check if a ray hits a collider:

```lua
local collisions = Components:Collisions()
local hit = collisions:isRayCollisionWith(origin, direction, maxDistance, object)

if hit then
    print("Ray hit the object!")
end
```

## Debug Mode
---

Enable visual debugging:

```lua
Components:Collisions():setEnableDebugMode(true)
```

## Practical Example
---

### Collectible Item

```lua
function onStart()
    -- Ghost collider (no physics, just detection)
    this:setCollisionsEnabled(true)
    this:setupGhostCollider(CollisionShape.SIMPLE_SHAPE)
end

function onUpdate()
    -- Rotate for visual effect
    local rot = this:getRotation()
    local spin = M3:getMatrixRotationForEulerAngles(0, Brakeza:getDeltaTime(), 0)
    this:setRotation(rot * spin)
end

function onCollision(with)
    if with ~= nil then
        local other = with:getObject()
        if other:getName() == "Player" then
            print("Item collected!")
            -- Add to inventory, play sound, etc.
            this:setRemoved(true)
        end
    end
end
```

## Method Summary
---

### Setup
| Method | Description |
|--------|-------------|
| `setCollisionsEnabled(bool)` | Enable/disable collisions |
| `setupGhostCollider(shape)` | Create ghost collider |
| `SetupRigidBodyCollider(shape, mass, static)` | Create rigid body |
| `setCapsuleColliderSize(r, h)` | Set capsule dimensions |

### Movement
| Method | Description |
|--------|-------------|
| `setWalkingDirection(Vertex3D)` | Kinematic movement |
| `Jump(Vertex3D)` | Kinematic jump |
| `onGround()` | Check ground contact |
| `setLinearVelocity(Vertex3D)` | Set velocity |
| `ApplyCentralForce(Vertex3D)` | Apply continuous force |
| `ApplyCentralImpulse(Vertex3D)` | Apply instant impulse |

### Properties
| Method | Description |
|--------|-------------|
| `setMass(float)` | Set mass |
| `setFriction(float)` | Set friction |
| `setRestitution(float)` | Set bounciness |
| `setGravityCollider(Vertex3D)` | Set custom gravity |
