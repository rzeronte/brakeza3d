---
name: Physics System (Bullet Physics)
description: Bullet Physics integration, body modes, collision shapes, groups, raycasting, debug draw
type: project
originSessionId: dc70ba7c-6393-464f-b834-22487807b205
---
# Brakeza3D — Physics System

## Backend
- **Library:** Bullet Physics (`btDiscreteDynamicsWorld`)
- **Component:** `include/Components/ComponentCollisions.h` / `src/Components/ComponentCollisions.cpp`
- **Debug draw:** `src/Render/PhysicsDebugDraw.cpp`
- **Collider base:** `include/Render/Collider.h`
- **Physics runs on:** worker thread (`ThreadJobStepSimulation`)

## Configuration (per project)
- Global gravity vector (configurable in Project Settings GUI)
- Sub-stepping: max 4 substeps at fixed 60 Hz

## Body Modes
| Mode | Class Tag | Behavior |
|------|-----------|---------|
| Dynamic | btRigidBody (mass > 0) | Affected by gravity, forces, collisions |
| Kinematic | btRigidBody (mass = 0, kinematic flag) | Animation-driven, collides but not moved by physics |
| Ghost | btGhostObject | Trigger volume — fires collision events, no physical response |

Set via `Collider::setBodyType()`. Character controllers use Kinematic.

## Collision Shapes
| Shape | Use Case |
|-------|---------|
| Box (AABB) | Generic mesh approximation |
| Capsule (height + radius) | Characters, humanoids |
| Triangle Mesh (btBvhTriangleMeshShape) | Static world geometry |

## Collision Groups (6 predefined)
```
Player, Enemy, Projectile, ProjectileEnemy, Health, Weapon
```
Each object has a group + mask (which groups it collides with). Configurable in inspector.

## Advanced Features
- **Raycasting:** Used for object picking (UI click → ray → object select)
- **Per-bone colliders:** `ColliderMesh3DAnimation` — collision shapes per bone for skeletal meshes (hitbox system)
- **Physics debug visualization:** toggle in toolbar → renders wireframe colliders via `PhysicsDebugDraw`

## Specialized Physics Objects
- `RayCollisionable` — objects that support ray intersection queries
- `SimplePendulum` — pendulum physics helper
- `Projectile` — projectile trajectory calculation
- `TentacleIK` — inverse kinematics chains

## Collision Events in Lua
```lua
function onCollisionEnter(info) end  -- CollisionInfo object
function onCollisionStay(info) end
function onCollisionExit(info) end
```
`CollisionInfo` contains references to both colliding objects.

**How to apply:** When adding new game objects that need physics, set body mode + shape in the Collider component. For triggers (e.g., pickups, zones), use Ghost mode. For characters, use Kinematic + Capsule. Static geometry should use TriangleMesh with Dynamic disabled.
