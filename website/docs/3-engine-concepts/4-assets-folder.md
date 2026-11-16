---
title: Assets folder
description: Assets folder structure in Brakeza3D, and how a project can keep its own assets folder on top of the engine's core assets.
---

# Assets folder
---

**Brakeza3D** expects to find each type of resource in its corresponding root folder. Inside the assets
folder, you will find:

| Folder             | Description                         |
|--------------------|-------------------------------------|
| assets/animations  | Animated FBX files.                 |
| assets/fonts       | TTF font files.                     |
| assets/icons       | UI icon files.                      |
| assets/images      | General image files.                |
| assets/maps        | BSP map files.                      |
| assets/models      | FBX model files.                    |
| assets/projects    | JSON project files.                 |
| assets/scenes      | JSON scene files.                   |
| assets/scripts     | LUA script files.                   |
| assets/shaders     | CustomShader files (.vs and/or .fs) |
| assets/sounds      | WAV or MP3 sound files.             |
| assets/sprites     | Grid-formatted image files.         |
| assets/textures    | Texture files used by FBX models.   |
| assets/screenshots | Auto-saved screenshots from render  |

**Brakeza3D** allows working with relative paths, but always relative to the main folder of each element.
That is, you can store your scripts in subfolders, but they must always remain within the root scripts
folder, and the same applies to other asset types.

## Project assets folder

The `assets/` folder next to the engine is the **core**: the resources that ship with Brakeza3D (examples,
tutorials, engine UI). A game built with the engine does not have to live there. A project can have its
**own assets folder**, with the same structure by type, and still reuse everything from the core without
copying it.

```
brakeza3d/
├── assets/                      <- core assets
│   ├── models/
│   └── ...
└── projects/
    └── MyGame/
        └── assets/              <- your project assets
            ├── models/
            ├── scenes/
            ├── scripts/
            └── sounds/
```

To enable it, add the optional `assetsFolder` key to your project file
(`assets/projects/<Name>Project.json`):

```json
{
  "name": "MyGame",
  "assetsFolder": "../projects/MyGame/assets/",
  "gravity": { "x": 0, "y": -9.8, "z": 0 }
}
```

The path is relative to the executable, like any other path in the engine. Without this key, the project
uses only the core assets and nothing changes.

### How a path is resolved

You keep writing the same literal paths in your scenes, scripts and JSON files:

```
../assets/models/city.fbx
```

You do **not** have to rewrite them to point to your project folder. When the engine opens a file, it looks
for it in this order:

1. `<assetsFolder>/models/city.fbx`, in your project.
2. `../assets/models/city.fbx`, in the core.

The project always wins, and the core is the fallback. This has two practical consequences:

- **Reuse.** Anything you do not have in your project is taken from the core, so a project only contains
  its own resources.
- **Override.** A resource in your project replaces a core resource with the same name and path, for that
  project only. The core is never modified.

Paths that do not start with `../assets/` (for example `../GLSL/...` or `../config/...`) are not assets and
are used as they are.

### What is resolved

All the engine loaders go through this mechanism: scenes, projects, Lua scripts, models and animations,
textures, images, sounds, fonts, videos, shaders (including their `.json` data-types file), UI widgets and
mesh instance files. Both the project and the core folders are listed when the editor browses a folder or
loads all the widgets of a directory.

**Writing is not redirected.** When the editor saves a file, it is written exactly where the path says.

### Models and texture paths

FBX files store the path of their textures at export time, and some exporters write an **absolute path**
of the machine that exported them (for example `C:\Users\you\brakeza3d\assets\textures\X.png`). That path
does not exist after the assets move to a project folder. To keep those models working, if a texture is
not found, the engine takes the part of the path that follows the last `assets/` and looks for it under the
assets root, project first. The recommended solution is still to export models with **relative** texture
paths.

## Lua scripts

Lua scripts get the same behaviour without changes:

- `io.open(path, mode)` resolves the path before opening it.
- `require` looks first in `<assetsFolder>/scripts/` and then in the core scripts folder.
- `Assets.resolve(path)` returns the path the engine would actually open.
- `Assets.projectRoot()` returns the project assets folder, or an empty string if the project does not
  have one.

```lua
local real = Assets.resolve("../assets/sounds/alarm.wav")
print(real)                       -- ../projects/MyGame/assets/sounds/alarm.wav (if it exists there)
print(Assets.projectRoot())       -- ../projects/MyGame/assets/
```
