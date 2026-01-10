---
title: 2.3) Projects
---

# 2.3) Projects
---

A `Project` is a container for scenes and scripts. It can be stored on disk as a `JSON` file and loaded at any time.

The `Project` is the highest-level entity in **Brakeza3D**. In general, you will have one project per application or game you are working on.


## Project settings window
---

The `Project setup` widget allows us to configure the global `Scripts` and the post-processing `Shaders` that are applied to the scene.

In this section, you will find a drop-down tree with three options:

| Option              | Description                                                                                                                                                                                                                                                  |
|---------------------|--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| **Project scripts** | Global scripts. These scripts control the flow of your application. Even if you change the `Scene`, `Project` scripts will remain associated until you load another Project. This type of script shares its variables globally with the rest of the scripts. |
| **Scene scripts**   | Scene scripts. Every time you open a scene, you will find its associated scripts here. They are also global in nature, meaning their variables are accessible by other scripts.                                                                              |
| **Scene shaders**   | Brakeza3D includes a post-processing shader chain. These shaders are executed in the order they are added and allow you to achieve visual effects across the entire scene.                                                                                   |


| Project setup #01       | Project setup #02       | Project setup #03       |
|-------------------------|-------------------------|-------------------------|
| ![](/img/editor/12.png) | ![](/img/editor/13.png) | ![](/img/editor/14.png) |

:::note
Each of these options can receive items via drag and drop. Use the file browser (either Scripts or Shaders) and drag them into the Project setup widget.
:::

You can enable/disable the widget from the main menu: `Window` -> `Project setup`.

## Projects browser window
---

The `Projects browser` widget allows you to navigate through the projects folder. From this section, you can load or delete existing projects.

![Projects browser](/img/editor/15.png)

For each `Project`, you can perform the following actions using the available buttons:

- Load project
- Save project
- Remove project