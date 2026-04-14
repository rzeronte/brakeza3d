---
name: Build System & Dependencies
description: CMake configuration, external libraries, platforms, compile flags, embedded libraries, build targets
type: project
originSessionId: dc70ba7c-6393-464f-b834-22487807b205
---
# Brakeza3D — Build System & Dependencies

## Build System
- **CMake** 3.10+
- **Standard:** C++17
- **Output:** `Brakeza3D` executable
- **IDE:** CLion (Windows primary development)

## Compile Flags
| Config | Flags |
|--------|-------|
| Debug | `-O0 -Wall -g` |
| Release | `-O3 -w` |
| Both | `-DENABLE_LOGGING_BUILD` |

## External Libraries (linked via CMakeLists.txt)
| Library | Purpose | Platform Notes |
|---------|---------|---------------|
| SDL2 | Window, input, events | Win32 + Unix |
| SDL2_image | Image loading (PNG, JPEG, etc.) | |
| SDL2_ttf | Font/text rendering | |
| SDL2_mixer | Audio (WAV, OGG, FLAC, MP3) | |
| OpenGL 3.3+ | Graphics | Core profile |
| GLEW | GL extension loading | Platform-specific libs |
| Bullet Physics | Physics simulation | BulletDynamics, BulletCollision, LinearMath |
| Assimp | 3D model import | Multi-format |
| Lua | Scripting | `lua52` (Win32) / `lua5.2` (Unix) |
| cURL | HTTP client (Resource Hub) | |
| pthreads | Threading | `-pthread` flag |

## Embedded Libraries (in repo, no install needed)
| Library | Location | Purpose |
|---------|----------|---------|
| Dear ImGui | `/imgui/` | UI/Editor framework |
| ImGui Node Editor | `/include/GUI/imgui-node-editor-master/` | Node graph UI |
| Sol2 | `/sol/` | Lua C++ binding (header-only) |
| cxxxopts | `/cxxxopts/` | CLI argument parsing |
| cJSON | (embedded) | JSON parsing |

## Build Directories
```
cmake-build-debug-coverage/
cmake-build-debug-mingw-brakezabundle/
cmake-build-release-mingw/
cmake-build-release-mingw-brakezabundle/
```
"brakezabundle" targets bundle all dependencies statically for distribution.

## Platform Notes
- Primary development: Windows 11 + MinGW + CLion
- Also builds on Linux/macOS (Lua lib name differs: `lua5.2` vs `lua52`)
- OpenGL core profile 3.3 minimum — no legacy fixed-function pipeline

**How to apply:** When adding new C++ dependencies, update `CMakeLists.txt` with both Win32 and Unix conditional blocks (matching the Lua lib name pattern). Embedded header-only libs just go in `/include/` or a subdirectory.
