---
title: Assets folder
description: Assets folder structure in Brakeza3D for organizing models, textures, scripts, shaders, and other resources.
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