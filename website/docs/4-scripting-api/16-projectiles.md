---
sidebar_position: 17
title: Projectile System
description: Fire physics-based projectiles from Lua scripts in Brakeza3D.
---

# Projectile System
---

A `Projectile` is a physics-enabled `Mesh3D` that can be launched in a direction with an impulse. It participates in the collision system and triggers `onCollision` on whatever it hits.

## Creating a Projectile

```lua
local proj = ObjectFactory.Projectile(
    "../assets/models/bullet.fbx",   -- mesh file
    Vertex3D.new(x, y, z)            -- initial position
)
```

The mesh loads asynchronously. `fire()` should be called after the object is in the scene (e.g. the frame after creation, or after a short delay).

## Firing

```lua
proj:fire(direction, impulse, accuracy, shapeRadius, collisionGroup, collisionMask)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `direction` | `Vertex3D` | Normalized direction vector |
| `impulse` | `float` | Force applied at launch |
| `accuracy` | `float` | Spread radius (0 = perfect, higher = more random scatter) |
| `shapeRadius` | `float` | Radius of the collision sphere (use `0.1` for small bullets) |
| `collisionGroup` | `int` | Collision group this projectile belongs to |
| `collisionMask` | `int` | Bitmask of groups this projectile collides with |

### Collision Groups

Use the `CollisionGroups` constants for group and mask values:

| Constant | Description |
|----------|-------------|
| `CollisionGroups.Player` | Player units |
| `CollisionGroups.Enemy` | Enemy units |
| `CollisionGroups.ProjectileGroup` | Player projectiles |
| `CollisionGroups.ProjectileEnemy` | Enemy projectiles |
| `CollisionGroups.Health` | Health pickups |
| `CollisionGroups.Weapon` | Weapon pickups |

```lua
-- Player bullet — belongs to ProjectileGroup, hits Enemy
proj:fire(
    dir,
    80,                              -- impulse
    0,                               -- perfect accuracy
    0.1,                             -- collision sphere radius
    CollisionGroups.ProjectileGroup, -- this projectile's group
    CollisionGroups.Enemy            -- what it can hit
)
```

## Direction Helpers

| Method | Parameters | Return | Description |
|--------|------------|--------|-------------|
| `getDirection()` | — | `Vertex3D` | Returns the current direction vector |
| `setDirection()` | `Vertex3D` | void | Overrides the direction after firing |

## Detecting Hits

When a projectile hits something, `onCollision` fires on **both** the projectile and the target. To handle the hit on the target, add a script with:

```lua
function onCollision(info)
    local other = info:getObject()
    if other == nil then return end
    -- check if it was a projectile that hit us
    print("hit by " .. other:getName())
end
```

To self-destruct the projectile on impact, handle it inside the projectile's own script:

```lua
function onCollision(info)
    this:setRemoved(true)
end
```

## Full Example — Shooting Projectiles

```lua
local fireTimer = 0
local FIRE_RATE = 0.3   -- seconds between shots

function onUpdate()
    local dt    = Brakeza:getDeltaTime()
    local input = Components:Input()

    fireTimer = fireTimer + dt

    if input:isCharPressed("Space") and fireTimer >= FIRE_RATE then
        fireTimer = 0
        shoot()
    end
end

function shoot()
    local cam = Components:Camera():getCamera()

    -- Spawn bullet slightly in front of the camera
    local spawnPos = cam:getPosition() + cam:AxisForward():getScaled(2)
    local proj = ObjectFactory.Projectile("../assets/models/bullet.fbx", spawnPos)

    -- Fire along camera forward axis
    proj:fire(
        cam:AxisForward(),
        120,
        0.02,
        0.1,
        CollisionGroups.ProjectileGroup,
        CollisionGroups.Enemy
    )
end
```

## Notes

- Projectiles inherit from `Mesh3D` and `Object3D` — all standard methods (`setEnabled`, `setRemoved`, `getPosition`, etc.) apply.
- Call `setRemoved(true)` to destroy a projectile after impact; otherwise it stays in the world indefinitely.
- `accuracy > 0` adds random scatter to the direction at fire time. Values around `0.05`–`0.1` produce a realistic spread.
- CCD (continuous collision detection) is automatically enabled on `fire()`, preventing fast projectiles from tunnelling through thin geometry.
