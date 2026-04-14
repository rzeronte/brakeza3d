---
name: GUI & Editor System
description: ImGui-based editor, window list, shortcuts, toolbar, status bar, node editor, shader node types, addon architecture
type: project
originSessionId: dc70ba7c-6393-464f-b834-22487807b205
---
# Brakeza3D — GUI & Editor System

## Framework
- **Library:** Dear ImGui (embedded `/imgui/`)
- **Rendering backend:** OpenGL 3.3
- **Input backend:** SDL2
- **Node editor:** ImGui Node Editor (embedded `/include/GUI/imgui-node-editor-master/`)
- **Manager:** `include/GUI/GUIManager.h` / `src/GUI/GUIManager.cpp`
- **Core:** `include/GUI/GUI.h` / `src/GUI/GUI.cpp`

## Layout Presets
3 presets: Default, Developers, Designers  
20+ dockable windows

## Main Editor Windows
| Window | Shortcut | Purpose |
|--------|----------|---------|
| Scene Objects | F9 | Object hierarchy (tree/list view), type filters |
| Object Properties | F10 | Inspector: transforms, shaders, scripts, colliders |
| Project Settings | F11 | Project config, scene list, global scripts |
| File Browser | F12 | Asset navigator, image/model preview |
| Code Editor | Menu | Multi-tab, Lua/GLSL syntax highlighting |
| Console | Menu | Lua REPL + colored logs |
| Resource Hub | Menu | Online marketplace (login, search, download) |
| Documentation | Menu | Searchable inline help |
| Profiler | Menu | Frame time, GPU/CPU stats, draw call count |
| Thread Workers | Menu | Thread pool status visualization |

## Object Inspector (per type)
- **Mesh3D:** model file, material selection, custom shader chain UI
- **LightPoint/LightSpot:** ADS color pickers, attenuation/cutoff sliders
- **ParticleEmitter:** emission rate, lifetime, velocity, gravity
- **Image3D/2D:** size, billboard settings, animation frames
- **Collider:** shape selector, mass, friction, damping, body mode
- **Swarm:** attached objects list

Inspectors live in `/src/GUI/Objects/` + `/include/GUI/Objects/`

## Toolbar Buttons
- Transform: Translate, Rotate, Scale (gizmos)
- Debug: simulation toggle, collider visualization, picking mode
- View: wireframe, grid, mouse-look, light debug, shadow debug
- Utility: screenshot

## Status Bar
- FPS counter
- Object/Light/Script counts
- Project + Scene names
- Online/Offline indicator
- Engine version

## Icon System
- `include/GUI/IconsGUI.h` — 150+ sprite sheet icons
- Object type icons + UI state indicators

## Shader Node Editor
- `include/GUI/ShaderNodeEditorManager.h` / `src/GUI/ShaderNodeEditorManager.cpp`
- `include/GUI/NodeEditorManager.h` / `src/GUI/NodeEditorManager.cpp`
- Supports both scene (postprocessing) and object (mesh) node graphs
- Auto-generates GLSL from node graph
- Real-time compilation feedback
- JSON save/load of node graphs

### Node Types (30+)
**Texture/Input:** Image, InternalTexture (chain output), MeshTexture  
**Math:** Add, Subtract, Multiply, Divide  
**Trig:** Sin, Cos, Smoothstep  
**Color:** Saturation, Grayscale, Contrast, Invert, Mix, Gradient, Vignette  
**UV:** Scale, Rotate, Offset, Coords  
**Time:** time-based animation nodes  
**Output:** Final output node

## GUI Addon Architecture
Modular addon classes in `/src/GUI/AddOns/` + `/include/GUI/AddOns/`:
- `GUIAddonMenu` — main menu bar
- `GUIAddonToolbar` — tool buttons
- `GUIAddonObject3DProperties` — inspector panel
- `GUIAddonObjects3D` — scene hierarchy panel
- `GUIAddonProjectSetup` — project configuration
- `GUIAddonResourceHub` — marketplace integration
- `GUIConsole` — REPL + logging

## Editable Components (Pickers)
`/src/GUI/Editable/` + `/include/GUI/Editable/`:
- `EditableOpenFile` — generic file picker
- `EditableOpenScriptFile` — Lua script selector
- `EditableOpenShaderFile` — GLSL shader selector
- `EditableOpenNode` — node graph file selector

**How to apply:** To add a new inspector panel for a new entity type: subclass the addon pattern in `/src/GUI/Objects/`, register in `GUIManager`. To add a node shader type: add to `ShaderBaseNodes` subclass + register in `ShaderNodeEditorManager`.
