---
sidebar_position: 14
title: Components
description: Complete API reference for Brakeza3D components including Window, Render, Input, Camera, Collisions, and Scripting.
---

# Components
---

Below is a detailed list of the methods available through each component.


## Component Window
---

The `Window` component is responsible for initializing the operating system window and managing the
framebuffers that are displayed on screen.

Through your LUA scripts, you can access the following methods:

| Function                       | Description                                              |
|--------------------------------|----------------------------------------------------------|
| `getWidth()`                   | Returns the window width in pixels                       |
| `getHeight()`                  | Returns the window height in pixels                      |
| `getWidthRender()`             | Returns the render width in pixels                       |
| `getHeightRender()`            | Returns the render height in pixels                      |
| `isWindowMaximized()`          | Returns whether the window is currently maximized        |
| `setWindowTitle(string)`       | Sets the window title                                    |
| `ToggleFullScreen()`           | Switches the window between fullscreen and windowed mode |
| `LoadCursorImage(string path)` | Loads the specified image as the mouse cursor            |
| `setClearColor(r, g, b, a)`   | Sets the background clear color of the scene framebuffer |
| `setImGuiMouse()`              | Restores mouse control to ImGui (call when stopping play mode) |



## Component Render
---

The `Render` component is responsible for handling global illumination, shader management among other
rendering-related tasks.

Through your LUA scripts, you can access the following methods:

| Function                           | Description                                                                  |
|------------------------------------|------------------------------------------------------------------------------|
| `getSceneLoader()`                        | Returns the scene loader component used to load and manage scenes                                           |
| `setGlobalIlluminationDirection()`        | Sets the direction of the global illumination light                                                         |
| `setGlobalIlluminationAmbient()`          | Sets the ambient color or intensity of the global illumination                                              |
| `setGlobalIlluminationDiffuse()`          | Sets the diffuse color or intensity of the global illumination                                              |
| `setGlobalIlluminationSpecular()`         | Sets the specular color or intensity of the global illumination                                             |
| `getSceneShaderByLabel()`                 | Returns a scene shader by its assigned label                                                                |
| `clearSceneShaders()`                     | Removes all scene-level post-processing shaders from the current render pipeline                            |
| `getFps()`                                | Returns frames per second                                                                                   |
| `MakeScreenShot(string path)`             | Makes a PNG screenshot at the given path                                                                    |
| `DrawLine()`                              | Draws a line in the scene, typically for debugging or visualization purposes                                |
| `getSelectedObject()`                     | Returns the selected object when exactly **one** object is selected, otherwise `nil`                       |
| `getSelectedObjects()`                    | Returns a table with **all** currently selected objects (empty table when nothing is selected)              |
| `setSelectedObject(Object3D)`             | Replaces the current selection with a single object                                                         |
| `addToSelection(Object3D)`                | Adds an object to the selection group (toggles it out if already present)                                   |
| `removeFromSelection(Object3D)`           | Removes a specific object from the selection group                                                          |
| `clearSelection()`                        | Empties the selection group                                                                                  |
| `hasMultipleSelected()`                   | Returns `true` when more than one object is selected                                                        |
| `isObjectInSelection(Object3D)`           | Returns `true` if the given object is currently part of the selection group                                 |
| `getLastRightClickedObject()`             | Returns the object under the cursor when right mouse button was last released, or `nil`                     |
| `clearRightClickedObject()`               | Clears the stored right-click object (call after consuming it)                                              |
| `getLastLeftClickedObject()`              | Returns the object under the cursor when left mouse button was last clicked, or `nil`                       |
| `clearLeftClickedObject()`                | Clears the stored left-click object (call after consuming it)                                               |
| `getLastLeftClickedSubmeshName()`         | Returns the submesh name of the last left-clicked object                                                    |
| `DrawLine2D(x1, y1, x2, y2, color, w)`   | Draws a 2D line on screen between two pixel coordinates                                                     |
| `DrawFilledRect(x, y, w, h, color)`       | Draws a filled rectangle on screen in pixel coordinates                                                     |
| `DrawImage2D(path, x, y, w, h)`           | Draws an image on screen from a file path; cached after first load                                          |
| `DrawImage2DFromImage(img, x, y, w, h)`   | Draws an Image object directly on screen                                                                    |
| `DrawCircle3D(center, radius, r, g, b, a)`| Draws a 3D circle at the given world position                                                               |
| `drawGroundCircle(obj, r, g, b, a, radius)`| Draws a circle on the ground under an object, masked by G-Buffer geometry                                  |
| `drawGroundDecal(obj, tex, r, g, b, a, radius)` | Projects a decal texture onto the ground under an object                                             |
| `drawAxisQuad(obj, r, g, b, a, halfSize)` | Draws a flat quad aligned to an axis under an object                                                        |
| `drawOutlineSubmesh(obj, name, r, g, b, a, thickness)` | Draws a colored outline around a specific submesh                                                |
| `getSubmeshCenter(obj, name)`             | Returns the world-space center of the given submesh                                                         |
| `getTextWriter()`                         | Returns the engine's shared TextWriter instance                                                             |
| `DrawFilledRectToFB(x, y, w, h, color, fb)` | Draws a filled rectangle into a named framebuffer instead of the screen                                  |
| `DrawImage2DToFB(path, x, y, w, h, fb)`  | Draws an image into a named framebuffer instead of the screen                                               |
| `DrawCircle2DToFB(x, y, size, r, g, b, a, waves, speed, thickness, additive, fb)` | Draws a 2D circle into a named framebuffer        |
| `drawGroundCircleToFB(obj, r, g, b, a, radius, fb)` | Draws a ground circle into a named framebuffer                                                   |
| `drawGroundDecalToFB(obj, tex, r, g, b, a, radius, fb)` | Projects a decal into a named framebuffer                                                    |

### Drawing to a named framebuffer

All `*ToFB` methods accept a framebuffer name as their last parameter. The valid names are fixed and correspond to the engine's internal layer stack:

| Name | Layer | Composited |
|------|-------|------------|
| `"background"` | Bottom layer — drawn before the 3D scene | Yes |
| `"scene"` | Scene layer — same layer as 3D geometry | Yes |
| `"ui"` | UI layer — drawn on top of the scene | Yes |
| `"global"` | Global composite layer | Yes |
| *(any other value)* | Foreground layer (default fallback) | Yes |

```lua
local render = Components:Render()

-- Draw a minimap background into the UI layer
render:DrawImage2DToFB("../assets/ui/minimap_bg.png", 10, 10, 256, 256, "ui")
render:DrawFilledRectToFB(unitX, unitY, 4, 4, Color.new(0, 1, 0, 1), "ui")

-- Draw a ground selection circle into the scene layer (receives lighting context)
render:drawGroundCircleToFB(unit, 0, 1, 0, 0.8, 1.5, "scene")
```

### UI Widget System

Widgets are reusable UI panels defined as JSON files in `assets/ui/`. Each widget contains a list of typed elements (text, image, rect, progressbar, icons, button). The editor (UIManager window) lets you design widgets visually; from Lua you draw them at runtime.

| Method | Parameters | Return | Description |
|--------|------------|--------|-------------|
| `drawWidget(name, x, y, data)` | `string, float, float, table` | `nextY, clickedId` | Draws a widget at (x, y); returns the Y position below the widget and the id of any clicked button |
| `reloadWidgets()` | — | void | Reloads all widget JSON files from disk without restarting |

#### drawWidget data table

The `data` table is keyed by element **id** (as defined in the widget JSON). Each value is a table with the fields that element needs:

| Element type | Accepted fields |
|---|---|
| `text` | `text` (string), `color` (Color) |
| `image` | `path` (string) |
| `rect` | `color` (Color) |
| `progressbar` | `value` (float), `max` (float), `color` (Color) |
| `icons` | `list` (array of image path strings) |
| `button` | `text` (string), `color` (Color) |

Elements whose id is not present in the data table are rendered with their JSON defaults.

`drawWidget` returns two values:
- **`nextY`** — the Y pixel coordinate immediately below the widget (useful for stacking multiple widgets)
- **`clickedId`** — the `id` of the button element that was clicked this frame, or `""` if none

```lua
local render = Components:Render()

function postUpdate()
    local nextY, clicked = render:drawWidget("unitCard", 10, 10, {
        name     = { text = "Soldier",          color = Color.new(1, 1, 1, 1) },
        hp_bar   = { value = 75, max = 100,     color = Color.new(0.2, 0.8, 0.2, 1) },
        portrait = { path = "../assets/ui/soldier.png" },
        btn_stop = { text = "Stop",             color = Color.new(0.9, 0.3, 0.3, 1) },
    })

    if clicked == "btn_stop" then
        -- handle button press
    end

    -- Stack a second widget directly below the first
    render:drawWidget("resourceBar", 10, nextY + 4, {
        gold_label = { text = "Gold: " .. gold, color = Color.new(1, 0.85, 0.2, 1) },
    })
end
```

:::note
Call `reloadWidgets()` after editing a widget JSON at runtime to pick up the changes without reloading the scene.
:::

### Multi-selection example

```lua
function onUpdate()
    local render   = Components:Render()
    local selected = render:getSelectedObjects()

    -- Nudge all selected objects upward
    for i = 1, #selected do
        selected[i]:addToPosition(Vertex3D.new(0, 0.01, 0))
    end

    -- Check a specific object
    local obj = Brakeza:getObjectByName("Enemy_01")
    if obj and render:isObjectInSelection(obj) then
        print("Enemy_01 is in the group")
    end

    -- Build a selection programmatically
    if render:hasMultipleSelected() then
        render:clearSelection()
    end
end
```


## Component Input
---

The Input component provides an interface to access keyboard, mouse, and game controller input methods.

| Function                   | Description                                         |
|----------------------------|-----------------------------------------------------|
| `setKeyboardEnabled(bool)` | Enables or disables camera movement via keyboard    |
| `setMouseEnabled(bool)`    | Enables or disables mouse look/rotation             |
| `setPadEnabled(bool)`      | Enables or disables gamepad look/rotation           |
| `isKeyboardEnabled()`      | Returns whether keyboard camera movement is enabled |
| `isMouseEnabled()`         | Returns whether mouse look/rotation is enabled      |
| `isPadEnabled()`           | Returns whether gamepad look/rotation is enabled    |

Through your LUA scripts, you can access the following methods:

| Function                          | Description                                                                  |
|-----------------------------------|------------------------------------------------------------------------------|
| `isKeyEventDown()`                | Returns true when a key-down event is detected                               |
| `isKeyEventUp()`                  | Returns true when a key-up (key release) event is detected                   |
| `isCharPressed()`                 | Checks if a character key is currently pressed                               |
| `isCharFirstEventDown()`          | Returns true only on the first press of a character key, ignoring key repeat |
| `isAnyControllerButtonPressed()`  | Returns true if any game controller button is pressed                        |
| `isMouseMotion()`                 | Returns true when mouse movement is detected                                 |
| `isClickLeft()`                   | Returns true while the left mouse button is being held down                  |
| `isClickRight()`                  | Returns true while the right mouse button is being held down                 |
| `isClickRightUp()`                | Returns true for one frame when the right mouse button is released           |
| `getRelativeRendererMouseX()`     | Returns the mouse X position relative to the renderer viewport               |
| `getRelativeRendererMouseY()`     | Returns the mouse Y position relative to the renderer viewport               |
| `getRawMouseX()`                  | Returns the raw mouse X position in window coordinates                       |
| `getRawMouseY()`                  | Returns the raw mouse Y position in window coordinates                       |
| `getMouseMotionXRel()`            | Returns the relative horizontal mouse movement since the last frame          |
| `getMouseMotionYRel()`            | Returns the relative vertical mouse movement since the last frame            |
| `isLeftMouseButtonPressed()`      | Returns true while the left mouse button is being held down                  |
| `isRightMouseButtonPressed()`     | Returns true while the right mouse button is being held down                 |
| `getMouseWheelY()`                | Returns the mouse wheel scroll delta for the current frame                   |
| `isGameControllerAvailable()`     | Returns true if a game controller is connected and available                 |
| `isMouseButtonDown(button)`       | Returns true while the given button is held (0=left, 1=middle, 2=right)      |
| `isMouseButtonUp(button)`         | Returns true for one frame when the given button is released                 |
| `consumeLeftClick()`              | Consumes the current left click so other systems don't process it            |
| `isMiddleMouseButtonPressed()`    | Returns true while the middle mouse button is held down                      |
| `getControllerButtonA()`          | Returns the state of the controller A button                                 |
| `getControllerButtonB()`          | Returns the state of the controller B button                                 |
| `getControllerButtonX()`          | Returns the state of the controller X button                                 |
| `getControllerButtonY()`          | Returns the state of the controller Y button                                 |
| `getControllerAxisTriggerLeft()`  | Returns the analog value of the left trigger axis                            |
| `getControllerAxisTriggerRight()` | Returns the analog value of the right trigger axis                           |
| `getControllerAxisLeftX()`        | Returns the horizontal axis value of the left analog stick                   |
| `getControllerAxisLeftY()`        | Returns the vertical axis value of the left analog stick                     |
| `getControllerAxisRightX()`       | Returns the horizontal axis value of the right analog stick                  |
| `getControllerAxisRightY()`       | Returns the vertical axis value of the right analog stick                    |
| `getControllerPadUp()`            | Returns true if the D-Pad Up button is pressed                               |
| `getControllerPadDown()`          | Returns true if the D-Pad Down button is pressed                             |
| `getControllerPadLeft()`          | Returns true if the D-Pad Left button is pressed                             |
| `getControllerPadRight()`         | Returns true if the D-Pad Right button is pressed                            |
| `getControllerShoulderLeft()`     | Returns true if the left shoulder button is pressed                          |
| `getControllerShoulderRight()`    | Returns true if the right shoulder button is pressed                         |
| `getControllerButtonBack()`       | Returns true if the Back / Select button is pressed                          |
| `getControllerButtonGuide()`      | Returns true if the Guide / Home button is pressed                           |
| `getControllerButtonStart()`      | Returns true if the Start button is pressed                                  |



## Component Camera
---

| Function                       | Description                            |
|--------------------------------|----------------------------------------|
| `getCamera()`                  | Returns the camera's `Object3D`        |
| `getGLMMat4ViewMatrix()`       | Returns the camera's view matrix       |
| `getGLMMat4ProjectionMatrix()` | Returns the camera's projection matrix |
| `worldToScreen(pos, w, h)`     | Converts a world-space position to screen pixels (see below) |

### worldToScreen

Projects a world-space `Vertex3D` onto the screen and returns a `Vertex3D` where:

- `x` — screen pixel column (from left)
- `y` — screen pixel row (from top)
- `z > 0` — the point is **in front of** the camera and visible; `z ≤ 0` means it is behind the camera

```lua
local cam   = Components:Camera()
local sw    = Components:Window():getWidth()
local sh    = Components:Window():getHeight()
local screen = cam:worldToScreen(unit:getPosition(), sw, sh)

if screen.z > 0 then
    -- unit is on screen — draw UI above it
    render:DrawFilledRect(screen.x - 2, screen.y - 20, 4, 4, Color.new(1,1,0,1))
end
```

:::warning
Always check `z > 0` before using `x` / `y`. When the object is behind the camera the projected coordinates are invalid.
:::


## Component Collisions
---

| Function               | Description                                             |
|------------------------|---------------------------------------------------------|
| `setEnabled()`         | Enables or disables the component or object             |
| `isRayCollisionWith()` | Checks if a ray collides with the object/component      |
| `setEnableDebugMode()` | Enables or disables debug mode for the object/component |


## Component Scripting
---

The Scripting component provides an interface to control the execution flow of scripts within your application.

Through your LUA scripts, you can access the following methods:

| Function                | Description                                            |
|-------------------------|--------------------------------------------------------|
| `PlayLUAScripts()`      | Starts the execution of all active LUA scripts         |
| `StopLUAScripts()`      | Stops the execution of all running LUA scripts         |
| `ReloadLUAScripts()`    | Reloads all LUA scripts and restarts their execution   |
| `AddSceneLUAScript()`          | Adds a LUA script that is executed at the scene level                   |
| `AddProjectLUAScript()`        | Adds a global LUA script executed at the project level                  |
| `getGlobalScriptVar(name, var)`| Returns the value of a variable from another script's environment       |
| `setGlobalScriptVar(name, var, val)` | Sets a variable in another script's environment                   |
| `RunProjectScriptsOnStart()`   | Re-runs `onStart()` on all project-level scripts                        |
| `loadJSON(path)`               | Reads a JSON file and returns it as a Lua table                         |
| `saveJSON(path, table)`        | Serializes a Lua table to a JSON file at the given path                 |


## SceneLoader
---

The SceneLoader is responsible for loading, saving, and managing scenes. You can access it through the Render component using `getSceneLoader()`.

Through your LUA scripts, you can access the following methods:

| Function                  | Description                                                                                      |
|---------------------------|--------------------------------------------------------------------------------------------------|
| `clearWorld()`                                                      | Removes everything: all objects, scripts, shaders and loaded scenes (full reset)             |
| `cleanWorld()`                                                      | Removes only runtime-created objects, keeping objects loaded from scene files                |
| `LoadScene(string path)`                                            | Loads a scene from the specified JSON file path (destructive, replaces current world)        |
| `loadSceneAdditive(path, scripts, shaders, camera, renderSettings)` | Loads a scene additively alongside existing ones                                             |
| `unloadScene(string name)`                                          | Unloads a specific scene and its objects, scripts and shaders                                |
| `reloadScene(string name)`                                          | Reloads a specific scene without affecting other loaded scenes                               |
| `setSceneActive(string name, bool active)`                          | Shows or hides a loaded scene (objects, physics)                                             |
| `SaveScene(string path)`                                            | Saves the current scene state to the specified JSON file path                                |

### Usage Example

```lua
local sceneLoader = brakeza:Render():getSceneLoader()

-- Load a scene (clears everything first)
sceneLoader:LoadScene("../assets/scenes/MyScene.json")

-- Load an interior scene additively (objects only)
sceneLoader:loadSceneAdditive("../assets/scenes/Interior.json")

-- Load additively with scripts and shaders too
sceneLoader:loadSceneAdditive("../assets/scenes/Interior.json", true, true, false, false)

-- Hide the exterior while inside
sceneLoader:setSceneActive("MyScene", false)

-- Unload the interior when leaving
sceneLoader:unloadScene("Interior")

-- Reset everything (full world clear)
sceneLoader:clearWorld()

-- Remove only runtime-spawned objects (projectiles, effects, etc.)
sceneLoader:cleanWorld()

-- Save current scene
sceneLoader:SaveScene("../assets/scenes/MyScene.json")
```

:::note
- `clearWorld()` removes **everything** and is used when switching projects or doing a full reset
- `cleanWorld()` preserves scene-loaded objects and only removes objects created at runtime via scripts
- `unloadScene()` is scene-aware: only removes objects, scripts and shaders belonging to that specific scene
:::