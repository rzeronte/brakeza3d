# Lua scripts

```
assets/scripts/
  Examples/   the tutorial and every demo, one folder per topic   <- start here
  Browser/    the on-screen demo browser (the tutorial project's menu) and its catalog
  RTS/        the RTS game's own scripts (not part of the tutorial)
```

## Try them

```
Brakeza3D -p TutorialsProject.json
```

The project opens a **demo browser**: a bar at the bottom (Demos / Tour|All / Prev / Next / Auto / Reload / Info) and an info
panel with what the demo shows, its controls and what to look at in the code.

It starts in **Tour** mode: a short playlist with the most striking demos (show room, particles, day/night, post-processing,
boids, physics, projectiles, third person, twin-stick). `PgDn` / `PgUp` walk the tour; `F3` (or the Tour/All button) switches
to **All**, which walks every demo; `F4` (Auto button) advances by itself every 20 s, like a slide show.
`Home` reload, `F1` demo list (the tour demos are marked with `*`), `F2` info panel.

## What is where

| Folder (`Examples/`) | What you learn |
|---|---|
| `Basics/` | **16 numbered lessons. Read them in order.** Hello world, input, moving and rotating objects, creating objects, parenting, look-at, a follow camera, reading another object's variables, lights, physics, collisions, skeletal animation, particles from a script, audio, updating a shader from a script |
| `Graphics/` | day/night cycle, post-processing chain (toggle each shader with the keys) |
| `ParticlesAndCrowds/` | the particle emitter (30 effects: spirals, emission shapes, bursts) and a boids swarm |
| `Physics/` | rain of rigid bodies, projectiles and collisions |
| `Characters/` | ready-to-copy character controllers: first person, third person, 2D platformer, twin-stick shooter |
| `InputAndUI/` | UI widgets, text rendering, mouse cursor, multi-selection |
| `AudioAndVideo/` | positional sound (`Sound3D`), video player |
| `Performance/` | a stress test that spawns objects in batches |

Each demo is the same trio: a **scene** (`assets/scenes/Examples/<Topic>/`), its **script(s)** (`assets/scripts/Examples/<Topic>/`)
and a **card** in `Browser/demos.json` (title, category, summary, controls, what to look at).

## How a script is made

A script is a pair of files with the same name:

- `Name.lua` — the code.
- `Name.json` — what the engine needs to attach it: `codeFile`, `typesFile`, `name`, `type` and the script **variables**
  (`types`). Variables show up in the editor and are available in the code as globals.

There are two kinds, set by `type` in the json:

| Type | Attached to | Runs | `this` |
|---|---|---|---|
| `Global` | the scene | for the whole scene | — |
| `Object` | one object | for that object (one copy per object) | the object |

Functions the engine calls for you (all optional): `onStart()` once, `onUpdate()` every frame, `onCollision(info)`,
`onEnd()`. Object scripts can also use `postUpdate()`; note that it only runs while the object is on screen, so put game logic in `onUpdate()`.

**Attach one:** in the editor (Object Scripts window for an object scripts; the scene scripts for a global one), or by hand in the
scene json: a global script goes in the top-level `"scripts"` list, an object script in the `"scripts"` list of that object.

## Add a demo

1. Write `Examples/<Topic>/MyDemo.lua` and `MyDemo.json`, starting with a comment that says what it is, its type and its keys.
2. Build a scene (editor) and save it to `assets/scenes/Examples/<Topic>/MyDemo.json` with the script attached.
3. Add the scene to `TutorialsProject.json` (`scenes`) and a card to `Browser/demos.json`. Cards with the same `category` must be together. To put it in the tour, add its scene name to the `tour` list of `demos.json`.
4. Open the project and press `Reload` in the browser.

## Things that bite

- API names are **case sensitive**: `Jump`, `ApplyCentralImpulse`, `AddObject3D`... A wrong case fails only when that line runs.
- `ObjectFactory.*` already adds the object to the scene (in the background): do not call `Brakeza:AddObject3D` on it too. Added twice, the object is freed twice when removed and the engine crashes (heap corruption). Use `AddObject3D(object, name)` (both arguments) only for objects you built another way.
- `object:onGround()` only works for kinematic characters. A rigid body has to check the ground with a ray.
- Script variables like `ParticlesContext.MIN_ALPHA` are integers (0-255), not 0..1.
- `setAnimationByName` needs the exact animation name stored in the FBX (e.g. `Armature|IDLE`).
- The scene loads in the background: an object or shader may not exist yet in `onStart()`. Look it up again in `onUpdate()` until it is there.
- The demo browser unloads a scene without calling its scripts' `onEnd()`. It resets the input, audio and background itself
  (`resetEngineState` in `Browser/DemoBrowser.lua`); a new demo that changes other global state should be undone there too.
