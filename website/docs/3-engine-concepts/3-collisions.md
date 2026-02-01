---
title: Collisions
---

# Collisions
---

**Brakeza3D** provides flexible collision modes and shapes, allowing developers to detect
collisions, respond to physical interactions, and apply custom gameplay logic through LUA scripts.

All objects instances can participate in the collision system, supporting `ghost`, `rigid body`, and
`kinematic` behaviors depending on the configured mode.

From the menu, you can configure fundamental aspects such as gravity strength and various performance settings:

![Menu collisions](/img/engine-concepts/03.png)

## Collision modes
---

| Collision Mode | Description                                      | Physics Reaction                          | Typical Uses                        | Special Notes                                                                |
|----------------|--------------------------------------------------|-------------------------------------------|-------------------------------------|------------------------------------------------------------------------------|
| Ghost          | Collidable object that does not react to physics | No physical response                      | Triggers, sensors, detection zones  | Detects collisions but is not affected by them                               |
| RigidBody      | Fully physical object                            | Reacts to forces, collisions, and gravity | Realistic physics objects           | Can be set as static (mass = 0). Only mode allowed for non-convex geometries |
| Kinematic      | Character-like movement                          | Controlled by code, not physics forces    | Player characters, moving platforms | Movement handled manually via methods                                        |

## Collider shapes
---

Brakeza3D provides several **collider shapes** that define the physical boundaries of objects for collision detection:

| Collider Shape | Description                                   | Performance            | Typical Uses                        | Notes                                    |
|----------------|-----------------------------------------------|------------------------|-------------------------------------|------------------------------------------|
| Simple Shape   | Basic collider suitable for most objects      | Excellent              | General-purpose objects             | Supported by any object                  |
| Capsule Shape  | Capsule-shaped collider for elongated objects | Good                   | Characters, tall or rounded objects | Works with any `Object3D`                |
| Triangle Mesh  | Matches exact geometry of a `Mesh3D` object   | Lower (more expensive) | Static meshes, precise collisions   | High accuracy, higher computational cost |

Collider shapes can be configured either through the GUI or via LUA scripts.

## Debug collisions
---

You can **debug the collision system** both **visually** and through the **system logs**. Visual debugging allows you to display collision shapes and boundaries directly in the scene, making it easier to identify incorrect alignments or unexpected intersections.

Additionally, system logs provide detailed runtime information about collision events, including object interactions and trigger activations, helping you track down logic errors and fine-tune collision behavior.

![Debug collisions](/img/engine-concepts/04.png)
