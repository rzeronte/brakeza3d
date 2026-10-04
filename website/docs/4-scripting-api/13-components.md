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
| `drawFillSubmesh(obj, name, r, g, b, a)`  | Tints a specific submesh with a translucent color for the current frame (`name = ""` tints every submesh of the object). `a` is the opacity |
| `PreloadImage2D(path)`                    | Loads an image into the image cache ahead of time, so the first time it is drawn there is no hitch (e.g. loading-screen backgrounds) |
| `getSubmeshCenter(obj, name)`             | Returns the world-space center of the given submesh                                                         |
| `getTextWriter()`                         | Returns the engine's shared TextWriter instance                                                             |
| `DrawFilledRectToFB(x, y, w, h, color, fb)` | Draws a filled rectangle into a named framebuffer instead of the screen                                  |
| `DrawImage2DToFB(path, x, y, w, h, fb)`  | Draws an image into a named framebuffer instead of the screen                                               |
| `DrawCircle2DToFB(x, y, size, r, g, b, a, waves, speed, thickness, additive, fb)` | Draws a 2D circle into a named framebuffer        |
| `drawGroundCircleToFB(obj, r, g, b, a, radius, fb)` | Draws a ground circle into a named framebuffer                                                   |
| `drawGroundDecalToFB(obj, tex, r, g, b, a, radius, fb)` | Projects a decal into a named framebuffer                                                    |

### Highlighting part of a model

`drawFillSubmesh` paints a translucent color over one submesh of an object (for example one building
of a city model, or one piece of a vehicle). It lasts one frame, so call it every frame while the
highlight should be visible. It is a fill, not an outline: combine it with `drawOutlineSubmesh` if you
want both.

```lua
function onUpdate()
    local city = Brakeza:getObjectByName("city")
    -- 40% green tint over the hovered building
    Components:Render():drawFillSubmesh(city, "BUILDING_12", 0.0, 1.0, 0.2, 0.4)
end
```

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
| `drawWidget(name, data, fb, scale)` | `string, table, string?, float?` | `nextY, hovered, clickedId, tooltipId` | Draws a widget at its JSON-defined position; `fb` selects the target framebuffer (default `"foreground"`), `scale` overrides the JSON scale (0 = use JSON value) |
| `drawWidgetAtPos(name, x, y, data, fb, scale)` | `string, float, float, table, string?, float?` | `nextY, hovered, clickedId, tooltipId` | Draws a widget at explicit pixel position `(x, y)`; other parameters same as `drawWidget` |
| `loadWidget(filePath)` | `string` | void | Loads a single widget JSON from the given file path into the UI manager |
| `loadWidgets(dir)` | `string` | void | Loads all widget JSON files found in the given directory |
| `unloadWidget(name)` | `string` | void | Unloads the named widget, freeing its resources |
| `setWidgetAlpha(alpha)` | `float` | void | Sets the global alpha multiplier applied to all widgets (0.0 = fully transparent, 1.0 = fully opaque) |
| `getHoveredWidgetCursor()` | — | `string` | Returns the cursor name string for the currently hovered widget element (empty string when nothing is hovered) |
| `flushTooltip(deltaTime)` | `float` | void | Advances the tooltip system timer by `deltaTime` seconds; call once per frame to drive tooltip show/hide transitions |
| `reloadWidgets()` | — | void | Reloads all widget JSON files from disk without restarting |
| `getWidgetSize(name)` | `string` | `width, height` | Declared size of a widget in window fractions (`0, 0` if it does not exist). Useful to center or align it from Lua |
| `invalidateWidget(name)` | `string` | `bool` | Forces a cacheable widget to re-render its cached image on the next draw |
| `invalidateAllWidgets()` | — | void | Same as `invalidateWidget` for every widget (e.g. after a context change) |
| `setUIDesignResolution(w, h)` | `float, float` | void | Resolution the UI was designed for: widget text scales with the window from it. `0, 0` disables it (text keeps its pixel size) |
| `isUIScrollHovered()` | — | `bool` | `true` while the mouse is over a scrollable widget area (use it to avoid zooming the camera with the wheel) |

#### drawWidget data table

The `data` table is keyed by element **id** (as defined in the widget JSON). Each value is a table with the fields that element needs:

| Element type | Accepted fields |
|---|---|
| `text` | `text` (string), `color` (Color) |
| `image` | `path` (string) |
| `rect` | `color` (Color) |
| `progressbar` | `value` (float), `max` (float), `color` (Color) |
| `icons` | `list` (array of image path strings) |
| `button` | `text` (string), `color` (Color), `key` (string, selects one of the button `options`), `path` (image), `tooltip` (string) |
| `widget` / `array` | `count` (int, number of rows of an array) — child elements read their own prefixed keys, see below |

Elements whose id is not present in the data table are rendered with their JSON defaults.

`image` and `button` values also accept `tooltip` (string), shown when the element is hovered.

`drawWidget` and `drawWidgetAtPos` each return four values:
- **`nextY`** — the Y pixel coordinate immediately below the widget (useful for stacking multiple widgets)
- **`hovered`** — the `id` of the element currently under the mouse cursor, or `""` if none
- **`clickedId`** — the `id` of the button element that was clicked this frame, or `""` if none
- **`tooltipId`** — the `id` of the element whose tooltip is active, or `""` if none

```lua
local render = Components:Render()

function postUpdate()
    -- drawWidget uses the position defined in the widget JSON
    local nextY, hovered, clicked, tooltip = render:drawWidget("exampleCard", {
        avatar   = { path = "../assets/images/me.png" },
        name     = { text = "Soldier",  color = Color.new(1, 1, 1, 1) },
        subtitle = { text = "Moving" },
        hpBar    = { value = 75, max = 100 },
    })

    -- Draw a second card at an explicit position, below the first
    render:drawWidgetAtPos("exampleCard", 10, nextY + 4, {
        name  = { text = "Gold: " .. gold, color = Color.new(1, 0.85, 0.2, 1) },
        hpBar = { value = gold, max = 1000 },
    })
end
```

#### Widget lifecycle helpers

```lua
local render = Components:Render()

-- Load a single widget at startup
render:loadWidget("../assets/ui/hud/unitCard.json")

-- Or bulk-load an entire folder
render:loadWidgets("../assets/ui/hud/")

-- Fade all widgets out
render:setWidgetAlpha(0.0)

-- Restore
render:setWidgetAlpha(1.0)

-- Update cursor when hovering widget elements
local cursor = render:getHoveredWidgetCursor()
if cursor ~= "" then
    Components:Window():LoadCursorImage("../assets/ui/cursors/" .. cursor .. ".png")
end

-- Drive tooltip timers (call once per frame)
render:flushTooltip(brakeza:getDeltaTime())

-- Unload a widget that is no longer needed
render:unloadWidget("unitCard")
```

:::note
Call `reloadWidgets()` after editing a widget JSON at runtime to pick up the changes without reloading the scene.
:::

#### Widget JSON reference

All widgets in `assets/ui/` (subfolders included) are loaded at startup; a widget is referenced by its
file name without extension. Positions and sizes are **fractions**: the widget's `posX`/`posY`/`width`/
`height` are fractions of the window, and element `x`/`y`/`w`/`h` are fractions of the widget.

Widget-level keys:

| Key | Description |
|-----|-------------|
| `posX`, `posY`, `width`, `height`, `scale` | Placement used by `drawWidget` and widget size |
| `bgColor`, `bgImage`, `bgImageAlpha` | Background color and/or image |
| `bgSlice`, `bgSliceScale` | 9-slice borders `[left, top, right, bottom]` in pixels: corners keep their size, edges stretch (frames that resize cleanly) |
| `borderColor`, `borderWidth` | Optional border |
| `font` | `.ttf`/`.otf` path for every text of the widget (nested widgets inherit it) |
| `refHeight` | When set, text scales with the window height relative to this value |
| `cacheable` | `true` (default) renders the widget into a cached image and only redraws it when its data changes. Use `false` for widgets that change every frame |
| `cursor` | Cursor name reported by `getHoveredWidgetCursor()` while hovering the widget |

Element types: `text`, `image`, `rect`, `progressbar`, `icons`, `button`, `widget` (a nested widget),
`array` (a list of a nested widget) and `scroll` (a scrollable list). Common element keys are `id`,
`type`, `x`, `y`, `w`, `h`, `alpha` and `enabled` (images and buttons also take a static `tooltip`).

| Key | Applies to | Description |
|-----|------------|-------------|
| `fontScale`, `textColor`, `textAlign` | text | Size, default color and alignment (`left`, `center`, `right`) |
| `font` | text | Font for this text only (overrides the widget font) |
| `wrap`, `w`, `maxLines`, `lineSpacing` | text | Word-wrap the text to width `w`, up to `maxLines` lines |
| `yAuto`, `paddingLeft`, `paddingTop`, `paddingBottom` | any | Stack the element right below the previous `yAuto` element instead of using `y` (layouts that grow with wrapped text) |
| `imagePath`, `imageScale` | image | Static image and its scale inside the box |
| `barW`, `barH`, `barBg`, `barOk`, `barMid`, `barLow` | progressbar | Bar size and colors (ok / mid / low by percentage) |
| `options` | button | List of states `{ key, text, image, tooltip, color }`; the data `key` picks one (e.g. an on/off toggle with two icons) |
| `size`, `btnBg`, `btnHover`, `btnPressed`, `sound` | button | Icon size, background colors per state and hover sound |
| `widgetRef` | widget, array, scroll | Name of the child widget |
| `arrayCount`, `arrayAlign`, `arrayOffset` | array | Default rows, `vertical`/`horizontal`, gap between rows |
| `arrayPaginate`, `arrayPagerWidget`, `arrayPagerTop` | array | Paginate long lists with a pager widget, optionally above the list |
| `arrayPrefix` | array | Prefix for row keys (default `""`) |

**Data for nested widgets.** A `widget` element with id `card` reads the keys prefixed with
`card_`. An `array` element reads its row count from its own id (`{ count = N }`) and row `i`
(0-based) reads the keys prefixed with `"<i>_"`. Click and hover ids of array rows come back
prefixed the same way (`"2_buyBtn"`).

```json
{
  "posX": 0.01, "posY": 0.3, "width": 0.1, "height": 0.22,
  "elements": [
    { "id": "title",    "type": "text",  "x": 0.04, "y": 0.03, "fontScale": 0.42 },
    { "id": "desc",     "type": "text",  "yAuto": true, "paddingTop": 0.12, "paddingLeft": 0.04,
      "w": 0.92, "wrap": true, "maxLines": 4, "fontScale": 0.35 },
    { "id": "unitList", "type": "array", "yAuto": true, "widgetRef": "exampleCard",
      "arrayAlign": "vertical" }
  ]
}
```

```lua
local data = {
    title    = { text = "SQUAD" },
    desc     = { text = "A long description that wraps automatically to the panel width." },
    unitList = { count = 2 },
    ["0_name"]  = { text = "Scout" },   ["0_hpBar"] = { value = 40, max = 80 },
    ["1_name"]  = { text = "Builder" }, ["1_hpBar"] = { value = 90, max = 120 },
}
Components:Render():drawWidget("squadPanel", data)
```

:::tip
The **UITest** scene (TutorialsProject) runs `Demos/GlobalScripts/UIWidgetsDemo.lua`, a minimal example
of a plain widget, a nested widget and an array, using the sample widgets shipped in `assets/ui/`.
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
| `isMouseInWindow()`               | Returns true while the mouse cursor is inside the application window (e.g. to stop edge-scrolling when it leaves) |
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