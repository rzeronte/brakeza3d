---
sidebar_position: 17
title: Sound3D
description: Sound3D object in Brakeza3D for positional audio sources with distance-based volume attenuation.
---

# Sound3D
---

## The Sound3D object
---

`Sound3D` is an invisible scene object that plays a WAV file with **automatic volume attenuation
based on the distance between the object's position and the camera**. It is the recommended
way to place ambient sounds, environmental audio, and zone music inside a 3D world.

Unlike `ComponentSound`, which plays sounds globally, `Sound3D` is a first-class scene object:
it can be placed in the editor, saved in scene files, and created at runtime from Lua.

---

## How it works
---

Every frame, `Sound3D` computes the distance from the camera to its position and derives a
volume using two radii:

```
dist ≤ innerRadius          → vol = baseVolume          (full volume)
innerRadius < dist < outerRadius → vol = baseVolume × (1 − t)  (linear fade)
dist ≥ outerRadius          → vol = 0                   (silence, channel released)
```

Where `t = (dist − innerRadius) / (outerRadius − innerRadius)`.

When the volume crosses from zero to positive the SDL_mixer channel is acquired automatically.
When it drops back to zero the channel is released so other sounds can use it.

---

## Properties
---

| Property      | Type     | Default | Description                                                  |
|---------------|----------|---------|--------------------------------------------------------------|
| `sourceFile`  | `string` | —       | Path to the WAV file (relative to the executable)            |
| `innerRadius` | `float`  | `15.0`  | Distance within which volume is at maximum                   |
| `outerRadius` | `float`  | `60.0`  | Distance beyond which the sound is completely silent         |
| `baseVolume`  | `int`    | `128`   | Maximum volume (SDL_mixer range: 0–128)                      |
| `loop`        | `bool`   | `true`  | Whether the sound loops indefinitely                         |

---

## Scene file format
---

```json
{
  "name": "sound_tavern",
  "type": 13,
  "position": { "x": 100, "y": 0, "z": 200 },
  "sourceFile": "../assets/sounds/tavern_ambient.wav",
  "innerRadius": 15,
  "outerRadius": 60,
  "baseVolume": 100,
  "loop": true
}
```

---

## Creating from Lua
---

Use `ObjectFactory.Sound3D` to create a positional sound source at runtime:

```lua
local snd = ObjectFactory.Sound3D(
    "../assets/sounds/tavern_ambient.wav",  -- WAV file path
    Vertex3D.new(100, 0, 200),             -- world position
    15,                                     -- innerRadius
    60,                                     -- outerRadius
    100                                     -- baseVolume (0–128)
)
snd:setName("sound_tavern")
snd:setSoundLoop(true)
Brakeza:addObject3D(snd, "sound_tavern")
```

### Lua methods

| Method | Description |
|--------|-------------|
| `snd:setSoundLoop(bool)` | Enable or disable looping |
| `snd:setInnerRadius(float)` | Change the full-volume radius at runtime |
| `snd:setOuterRadius(float)` | Change the silence radius at runtime |
| `snd:setBaseVolume(int)` | Change the maximum volume (0–128) |

These methods take effect immediately on the next frame update.

---

## Zone music / ambient crossfade
---

The recommended pattern for zone-based ambient music is to place multiple `Sound3D` objects
with **overlapping outer radii**. In the overlap zone both sounds play simultaneously — one
fading in, the other fading out — producing a natural crossfade with no extra scripting.

```lua
-- Residential district — calm ambient
ObjectFactory.Sound3D("../assets/sounds/ambient_residential.wav",
    Vertex3D.new(0, 0, 0), 30, 80, 90)

-- Market district — busy ambient
ObjectFactory.Sound3D("../assets/sounds/ambient_market.wav",
    Vertex3D.new(150, 0, 0), 30, 80, 100)

-- Port district
ObjectFactory.Sound3D("../assets/sounds/ambient_port.wav",
    Vertex3D.new(300, 0, 0), 40, 100, 85)
```

```
Residential       Market            Port
|←inner→|         |←inner→|         |←inner→|
|←——outer——→| |←——outer——→| |←——outer——→|
              ↑ crossfade   ↑ crossfade
```

---

## Background music (non-positional)
---

To use `Sound3D` as a global background music track that plays at full volume regardless of
camera position, set `innerRadius` to a very large value:

```lua
local bgm = ObjectFactory.Sound3D(
    "../assets/sounds/bgm_main_theme.wav",
    Vertex3D.new(0, 0, 0),
    99999,   -- inner: always at full volume
    100000,  -- outer: never silenced
    100
)
bgm:setName("bgm")
bgm:setSoundLoop(true)
Brakeza:addObject3D(bgm, "bgm")
```

---

## Limitations
---

| Limitation | Cause | Workaround |
|------------|-------|------------|
| WAV only | `Mix_LoadWAV` does not decode OGG/MP3 | Convert to WAV before use |
| 32 simultaneous sources | SDL_mixer channel limit | Increase in `ComponentSound.cpp` via `Mix_AllocateChannels` |
| Linear attenuation | Volume curve is `1 − t` | Change exponent in `Sound3D::onUpdate()` for a more natural roll-off |
| No occlusion | SDL_mixer has no geometry awareness | Not applicable for ambient music use cases |

---

## Test scene
---

`assets/scenes/Sound3DTest.json` ships with the engine and demonstrates three `Sound3D` objects
placed at different positions:

- **Radio Beep (left)** — small inner radius (5), medium outer radius (22).
- **Day Ambient (center)** — larger radii (8 / 35), lower volume.
- **Close Beep (right)** — tight inner radius (3), fast fall-off to outer (15).

A HUD overlay shows the live distance and volume percentage for each source as you walk
toward them with WASD.
