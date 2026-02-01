---
title: Welcome to Brakeza3D!
description: Introduction to Brakeza3D, a cross-platform 2D/3D game engine for indie developers with Lua scripting and GLSL shaders support.
---

# Welcome to Brakeza3D!
---

**Brakeza3D** is a simple and powerful cross-platform 2D/3D game engine designed for indie developers and small teams.

It features a clean, component-based architecture, a built-in scene editor, and full Lua scripting support and GLSL shaders,
allowing developers to prototype and build games quickly and efficiently.

![Use example](/img/getting-started/sc01.png)


## Purpose of this document
---

This document is designed for programmers who wish to use **Brakeza3D** in their own projects. Throughout this documentation, 
you will find detailed information about how the application works and the features it offers, enabling you to develop amazing things on your own!

:::note
This document is currently in BETA. We are continuously working on it.
:::

## Technologies used
---

**Brakeza3D** is designed entirely from scratch and integrates third-party libraries to provide a complete and self-contained solution.

| Technology     | Description                             | Official Link                           |
|----------------|-----------------------------------------|-----------------------------------------|
| C++            | Main programming language               | https://isocpp.org                      |
| SDL2           | Window management and input handling    | https://www.libsdl.org                  |
| SDL2_image     | Image loading library for SDL           | https://github.com/libsdl-org/SDL_image |
| SDL2_ttf       | Font rendering library for SDL          | https://github.com/libsdl-org/SDL_ttf   |
| SDL2_mixer     | Audio playback library for SDL          | https://github.com/libsdl-org/SDL_mixer |
| Bullet Physics | Physics engine                          | https://pybullet.org                    |
| Assimp         | 3D model importer                       | https://www.assimp.org                  |
| OpenGL         | Graphics API                            | https://www.opengl.org                  |
| GLM            | OpenGL Mathematics library              | https://github.com/g-truc/glm           |
| Lua 5.2        | Scripting language                      | https://www.lua.org                     |
| ImGui          | Immediate mode graphical user interface | https://github.com/ocornut/imgui        |

## Features
---

- Cross Platform (Windows/Linux/OSX checked)
- Easy and simple architecture
- Render Forward/Deferred
- ADS light system
- Physics World Integration
- GUI with docking integration
- Assimp library for loading models
- Scene management
- 2D / 3D features
- Profiler
- Shadow mapping
- LUA Scripting
- Extensible GLSL Shaders / Nodes editor
- Code editor with LUA/GLSL syntaxis integration
- Sound system integration
- Interface for XBOX/Playstation controller
- Multi-Thread (Thread pool based)

## License and usage terms
---

**Brakeza3D** is released under the GNU General Public License v3.0 (GPL-3.0).

You are free to use, modify and distribute this software, provided that:

- Any derivative work is also licensed under GPL-3.0
- Source code is made available
- Copyright and license notices are preserved
- This software is provided without any warranty.

For full license details, see the LICENSE file or visit:
https://www.gnu.org/licenses/gpl-3.0.html