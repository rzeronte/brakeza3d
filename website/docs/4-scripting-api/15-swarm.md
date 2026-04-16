---
sidebar_position: 16
title: Swarm
description: Swarm object in Brakeza3D for simulating flocking behaviour with boids, predators, and configurable steering parameters.
---

# Swarm
---

## The Swarm object
---

**Brakeza3D** includes a dedicated object for simulating **flocking behaviour**, called `Swarm`.

A `Swarm` manages a group of scene objects (called **boids**) that move together using the classic
[Reynolds boids algorithm](https://www.red3d.com/cwr/boids/), which combines three steering forces:

- **Separation** — boids steer away from neighbours that are too close.
- **Alignment** — boids steer towards the average heading of nearby neighbours.
- **Cohesion** — boids steer towards the average position of nearby neighbours.

A `Swarm` also supports **predator** objects that make boids flee, and an axis-aligned bounding box
(AABB) that keeps the flock inside a defined volume.

Any `Object3D` already present in the scene can be assigned as a boid or predator — the `Swarm`
controls their position every frame but does not own them.

---

## Editor workflow
---

1. Add a **Swarm** object via the *Add Object* menu in the editor.
2. Add the boid/predator meshes to the scene normally (e.g. `Mesh3D`).
3. Open the **Swarm Members** section in the properties panel.
4. Use the combo selector to assign scene objects as boids or predators, then click **Assign**.
5. Press **Load members** to resolve any pending names saved in the scene file.

Enable **Debug AABB** in the inspector to visualise the swarm volume as a yellow wireframe box.

---

## Properties
---

### Volume

| Property | Type      | Description                                         |
|----------|-----------|-----------------------------------------------------|
| `size`   | `Vertex3D`| Half-size of the AABB that bounds the flock         |
| `debug`  | `bool`    | Draw the AABB wireframe each frame when `true`      |

---

### Boid weights

| Property           | Type    | Default | Description                                       |
|--------------------|---------|---------|---------------------------------------------------|
| `separationWeight` | `float` | 1.2     | Strength of the separation steering force         |
| `alignmentWeight`  | `float` | 0.9     | Strength of the alignment steering force          |
| `cohesionWeight`   | `float` | 0.7     | Strength of the cohesion steering force           |

---

### Movement

| Property              | Type    | Default | Description                                                  |
|-----------------------|---------|---------|--------------------------------------------------------------|
| `neighborDist`        | `float` | 2.0     | Radius within which other boids are considered neighbours    |
| `maxSpeed`            | `float` | 0.8     | Maximum velocity magnitude for each boid                     |
| `velocityBoidsFactor` | `float` | 5.0     | Global multiplier applied to boid velocity each frame        |
| `turnFactor`          | `float` | 0.15    | How sharply boids turn when approaching the AABB boundary    |

---

### Center attraction

| Property                | Type    | Default | Description                                                       |
|-------------------------|---------|---------|-------------------------------------------------------------------|
| `centerThreshold`       | `float` | 1.0     | Distance from center below which attraction is not applied        |
| `centerAttractionWeight`| `float` | 0.5     | Strength of the force pulling boids back towards the swarm center |

---

### Predator avoidance

| Property                | Type    | Default | Description                                               |
|-------------------------|---------|---------|-----------------------------------------------------------|
| `predatorThreshold`     | `float` | 5.0     | Radius within which a predator triggers avoidance         |
| `predatorAvoidanceWeight`| `float`| 50.0    | Strength of the force pushing boids away from a predator  |

---

### Global bias

These multipliers scale their respective steering forces globally, making it easy to tune the
overall balance of the flock without touching individual weights.

| Property              | Type    | Default | Description                        |
|-----------------------|---------|---------|------------------------------------|
| `globalBiasSeparation`| `float` | 1.0     | Global multiplier for separation   |
| `globalBiasAlignment` | `float` | 1.0     | Global multiplier for alignment    |
| `globalBiasCohesion`  | `float` | 1.0     | Global multiplier for cohesion     |

---

## Scene file format
---

A `Swarm` is serialised like any other `Object3D` but with additional fields:

```json
{
  "name": "SwarmDemo",
  "type": 12,
  "position": { "x": 0, "y": 5, "z": 0 },
  "size":     { "x": 10, "y": 10, "z": 10 },
  "debug": false,
  "separationWeight": 1.2,
  "alignmentWeight":  0.9,
  "cohesionWeight":   0.7,
  "neighborDist":     2.0,
  "maxSpeed":         0.8,
  "velocityBoidsFactor": 5.0,
  "turnFactor":       0.15,
  "centerThreshold":  1.0,
  "centerAttractionWeight": 0.5,
  "predatorThreshold":      5.0,
  "predatorAvoidanceWeight": 50.0,
  "globalBiasSeparation": 1.0,
  "globalBiasAlignment":  1.0,
  "globalBiasCohesion":   1.0,
  "boids":     ["Boid_1", "Boid_2", "Boid_3"],
  "predators": ["Predator_1"]
}
```

The `"boids"` and `"predators"` arrays store the **names** of other scene objects. On load, the
engine resolves these names after all objects have been added to the scene.

---

## Example — DemoScene05
---

`assets/scenes/DemoScene05.json` ships with the engine and demonstrates a fully configured swarm:

- **30 icosphere boids** scattered around the origin.
- **1 red cube predator** at (6, 5, 0) that the boids flee from.
- Swarm volume: 10 × 10 × 10, centered at (0, 5, 0).
- Strong predator avoidance (`predatorAvoidanceWeight = 50`) so the flight pattern is clearly visible.

Load the scene, press **Load members** in the Swarm inspector, and the flock will start moving immediately.
