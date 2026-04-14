---
name: Lua Scripting System
description: Lua 5.2 + Sol2 bridge, lifecycle hooks, exposed API types, script levels, editor integration
type: project
originSessionId: dc70ba7c-6393-464f-b834-22487807b205
---
# Brakeza3D — Lua Scripting System

## Stack
- **Language:** Lua 5.2 (`lua52` on Win32, `lua5.2` on Unix)
- **Binding:** Sol2 (header-only, `/sol/`)
- **Bridge class:** `include/LUA/BrakezaLuaBridge.h`
- **Object factory:** `include/LUA/ObjectFactory.h`
- **Script class:** `include/Misc/ScriptLUA.h` / `src/Misc/ScriptLUA.cpp`
- **Component:** `ComponentScripting` — manages lifecycle

## Script Levels
1. **Global Scripts** — project-level, run at startup
2. **Scene Scripts** — per-scene initialization
3. **Object Scripts** — per-entity logic; multiple scripts per object allowed

## Lifecycle Hooks (per object script)
```lua
function onStart() end          -- called once when object is created
function preUpdate() end         -- before physics/logic update
function onUpdate() end          -- main update (delta time available)
function postUpdate() end        -- after update
function onCollisionEnter(info) end
function onCollisionStay(info) end
function onCollisionExit(info) end
```

## Exposed API — Math Types
| Lua Type | C++ Type | Constructor |
|----------|----------|-------------|
| `Vertex3D` | `Vector3D` / `Vertex3D` | `Vertex3D.new(x, y, z)` |
| `M3` | `M3` (3x3 matrix) | rotation matrix |
| `Color` | `Color` | `Color.new(r, g, b, a)` |

## Exposed API — Object Methods
```lua
-- Position
obj:setPosition(Vertex3D)
obj:AddToPosition(Vertex3D)
obj:getPosition()  -- returns Vertex3D

-- Rotation
obj:setRotation(M3)
obj:getRotation()  -- returns M3

-- Scale
obj:setScale(float)

-- Hierarchy
obj:setParent(Object3D)
obj:AttachObject(Object3D)

-- Visibility
obj:setAlpha(float)       -- 0.0 = invisible, 1.0 = opaque
obj:setRemoved(bool)      -- destroy object

-- Scripting
obj:AttachScript(string)  -- attach script file
obj:LookAt(Vertex3D)      -- orient towards target
```

## Exposed API — Specialized Types
```lua
-- Camera
Components.get().camera:setPosition(Vertex3D)
Components.get().camera:setRotation(M3)

-- ParticleEmitter
emitter:setEmission(float)
emitter:setActive(bool)

-- Mesh3DAnimation
anim:playAnimation(string)
anim:setLooping(bool)

-- brakeza (engine singleton)
brakeza:loadScene(string)
brakeza:addObject(Object3D)
```

## Script Type Parameters
- `ScriptLUATypeData` — typed parameters stored with script metadata
- Supported types: int, float, string, Vertex3D
- Type hints stored in JSON `.meta` files alongside `.lua` scripts
- Editable in GUI inspector (Object Properties → Scripts panel)

## Asset Location
- Scripts: `assets/scripts/*.lua`
- Script metadata: alongside `.lua` files as `.json`

## Editor Integration
- Lua syntax highlighting in built-in code editor
- Console/REPL for live testing (`GUIConsole`)
- Script parameters panel in Object Properties inspector

**How to apply:** When writing Lua scripts for this engine, always implement at minimum `onUpdate()`. Use `Vertex3D.new()` for positions (not tables). Access engine components via `Components.get()`. Collision callbacks receive a `CollisionInfo` object.
