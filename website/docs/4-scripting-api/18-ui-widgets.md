---
sidebar_position: 18
title: UI Widgets
description: Build HUDs, menus and panels as JSON widgets, design them in the UI Manager and draw them from Lua in Brakeza3D.
---

# UI Widgets
---

A **widget** is a reusable piece of 2D interface (a panel, a button bar, a list row, a tooltip…) described in a JSON file. You design it visually in the editor's **UI Manager** window and draw it from Lua every frame, passing only the data that changes (texts, values, images, which option is active).

Widgets give you:

- Layout, colors, fonts and images live in JSON, not in code, so they can be retouched without touching scripts.
- Buttons with hover and pressed states, sounds, cursors and click detection.
- Lists (arrays of a row widget), nested widgets, scroll areas and pagination.
- Background images with **9-slice** scaling (frames that fit any panel size).
- Automatic FBO caching of static widgets.

## Files and loading
---

Widgets are stored as `.json` files anywhere under `assets/ui/`, subfolders included. All of them are loaded at startup.

The **widget name is the file name without extension**: `assets/ui/hud/unitCard.json` is drawn as `"unitCard"`. Names must be unique across all subfolders.

```lua
local render = Components:Render()

render:reloadWidgets()                                 -- re-read every loaded folder from disk
render:loadWidget("../assets/ui/extra/popup.json")    -- load one file (and the widgets it references)
render:loadWidgets("../assets/ui/extra/")             -- load a whole folder, recursively
render:unloadWidget("popup")
render:clearWidgets()                                  -- unload everything
```

:::tip
The **UITest** scene (TutorialsProject) runs `Demos/GlobalScripts/UIWidgetsDemo.lua`, a minimal example of a
plain widget, a nested widget and an array using the sample widgets in `assets/ui/` (`exampleCard`,
`exampleNestedWidget`, `exampleArrayWidget`).
:::

## Coordinates and sizes
---

- A widget's `width` and `height` are **fractions of the window** (`0.25` = a quarter of the window width).
- Element `x`, `y`, `w`, `h` (and bar/icon sizes) are **fractions of the widget box**. `x = 0.5` puts the element in the middle of the widget.
- `posX` / `posY` place the widget on screen as fractions of the window. They are used by `drawWidget`, and ignored by `drawWidgetAtPos`.
- `scale` multiplies the whole widget. Nested widgets inherit their parent's scale.
- Text size (`fontScale`) is fixed by default: it does **not** follow the window size, positions do. Set `refHeight` on the widget to make text follow the render size too (see below).

Because positions and sizes are proportional, a widget keeps its layout when the window is resized. Design it at your target resolution.

### Text that follows the render size

`refHeight` is the render height (in pixels) the widget was designed for. When it is set, every text size in the widget is multiplied by `current render height / refHeight`. For example, with `"refHeight": 1080` and a 540 px tall render, text is drawn at half size. Text then grows and shrinks together with the panel.

- `0` (default): fixed text size.
- Nested widgets and array rows inherit the value of their parent unless they set their own, so setting it on the root widget is enough.
- By default the render follows the window, so resizing the window rescales the text. If you lock a render resolution (**Render size** menu), the factor depends only on that resolution: changing the render size rescales the text, resizing the window does not.

Use it for menus and full-screen panels that must look the same at any size. Leave it at `0` for small HUD text that must stay readable in small windows.

## Drawing from Lua
---

```lua
local nextY, clickedId, rightClickedId, hoveredId =
    render:drawWidget(name, data, fb, scale)

local nextY, clickedId, rightClickedId, hoveredId =
    render:drawWidgetAtPos(name, x, y, data, fb, scale)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `name` | `string` | Widget name (file name without `.json`) |
| `x`, `y` | `float` | *(drawWidgetAtPos only)* Top-left corner in window pixels |
| `data` | `table` | Per-element data (see below). Pass `{}` to use the JSON defaults |
| `fb` | `string` | *(optional)* Target layer: `"foreground"` (default), `"ui"`, `"background"`, `"scene"`, `"global"` |
| `scale` | `float` | *(optional)* Extra scale for this call only |

| Return | Description |
|--------|-------------|
| `nextY` | Y pixel just below the widget, useful to stack widgets |
| `clickedId` | `id` of the button left-clicked this frame, or `""` |
| `rightClickedId` | `id` of the button right-clicked this frame, or `""` |
| `hoveredId` | `id` of the button under the mouse, or `""` |

A click on a widget button **consumes** the left click (`Input:consumeLeftClick()`), so world-picking code doesn't also react to it.

```lua
local render

function onStart()
    render = Components:Render()
end

function postUpdate()
    local _, clicked = render:drawWidget("pauseMenu", {
        title   = { text = "PAUSED" },
        hpBar   = { value = hp, max = 100 },
        avatar  = { path = "../assets/images/portraits/hero.png" },
        btnMute = { key = muted and "off" or "on" },
    }, "ui")

    if clicked == "btnResume" then resumeGame() end
    if clicked == "btnMute"   then muted = not muted end

    render:flushTooltip(Brakeza:getDeltaTime())   -- once, after all widgets
end
```

:::tip
Draw widgets from `postUpdate()`, after the 3D scene. Call `flushTooltip(dt)` once per frame after the last `drawWidget`, so tooltips appear on top of everything.
:::

### The data table

`data` is keyed by **element id**. Each value is a table with the fields that element uses. Elements missing from `data` fall back to the widget's JSON `"data"` section, and then to their static JSON content.

| Field | Used by | Description |
|-------|---------|-------------|
| `text` | text, button | Text to show (overrides the static `text`) |
| `color` | text, rect, button, progressbar | `Color` override |
| `path` | image, button | Image path |
| `pathOff` | button | Image for the "off" state |
| `enabled` | button | With `path` + `pathOff`: `true` shows `path`, `false` shows `pathOff` |
| `key` | button | Selects one of the button's static `options` |
| `tooltip` | image, button | Tooltip text override |
| `value`, `max` | progressbar | Current and maximum value |
| `list` | icons | Array of icon keys to show, in order |
| `w`, `h` | rect | Size override, in pixels |
| `count` | array | How many items to draw (overrides `arrayCount`) |
| `page` | array | Page to show when the array is paginated |
| `scrollTo` | scroll | Force the scroll offset, in pixels (`0` = top) |

### Other functions

| Function | Description |
|----------|-------------|
| `getWidgetSize(name)` | Returns `width, height` of the widget as window fractions (`0, 0` if not found). Useful to center it yourself. |
| `setWidgetAlpha(a)` | Global opacity (0–1) for every widget drawn afterwards. Use it for fades, and set it back to `1`. |
| `getHoveredWidgetCursor()` | Name of the cursor requested by the widget/button under the mouse, or `""`. |
| `isUIScrollHovered()` | `true` while the mouse is over a scroll area. Ignore the mouse wheel in your camera code then. |
| `flushTooltip(dt)` | Draws the pending tooltip (0.35 s delay, 0.15 s fade-in). |
| `invalidateWidget(name)` | Forces a cacheable widget to re-render its cached image on the next draw (returns `false` if not found). |
| `invalidateAllWidgets()` | Same for every widget, e.g. after a context change after which the old cached image would be shown. |
| `setUIDesignResolution(w, h)` | Resolution the UI was designed for: at top level, widget text scales with the window from it (`0, 0` = off, fixed text size). Widgets with `refHeight` keep their own factor. |

```lua
-- Center a panel whatever its size in the JSON
local wFrac, hFrac = render:getWidgetSize("inventory")
local sw, sh = Components:Window():getWidth(), Components:Window():getHeight()
render:drawWidgetAtPos("inventory", (sw - sw * wFrac) / 2, (sh - sh * hFrac) / 2, data, "ui")
```

## Widget JSON
---

```json
{
  "scale": 1,
  "posX": 0.30, "posY": 0.20,
  "width": 0.40, "height": 0.50,
  "bgColor": { "r": 0.05, "g": 0.05, "b": 0.08, "a": 0.9 },
  "bgImage": "../assets/images/ui/FRAME.png",
  "bgSlice": [64, 64, 64, 64],
  "font": "../assets/fonts/MyFont.ttf",
  "cacheable": false,
  "elements": [
    { "id": "title", "type": "text", "x": 0.05, "y": 0.04, "fontScale": 0.8,
      "text": "INVENTORY", "textColor": { "r": 1, "g": 0.85, "b": 0.3, "a": 1 } },
    { "id": "items", "type": "array", "x": 0.05, "y": 0.15,
      "widgetRef": "inventoryRow", "arrayCount": 8 }
  ],
  "data": {
    "title": { "text": "INVENTORY" }
  }
}
```

### Widget properties

| Property | Description |
|----------|-------------|
| `width`, `height` | Size of the widget box (window fractions) |
| `posX`, `posY` | Screen position for `drawWidget` (window fractions) |
| `offsetX`, `offsetY` | Extra offset in pixels |
| `scale` | Scale multiplier |
| `bgColor` | Background color of the whole box (alpha `0` = none) |
| `bgImage` | Background image, drawn over `bgColor` and under the elements |
| `bgSlice` | `[left, top, right, bottom]` cuts in **image pixels** (or one number for all four). Enables 9-slice mode |
| `bgSliceScale` | Screen pixels per image pixel for the 9-slice borders (e.g. `0.25` for a 4× hi-res frame) |
| `bgImageAlpha` | Opacity of the background image, 0–1 (multiplies the image's own alpha) |
| `borderColor`, `borderWidth` | Border around the box, in pixels (`0` = none) |
| `font` | `.ttf`/`.otf` for all text in this widget. Nested widgets inherit it unless they set their own |
| `refHeight` | Design render height in pixels. When set, text scales with the current render height. Inherited by nested widgets |
| `cursor` | Cursor name requested while the mouse is over the widget |
| `cacheable` | `true` (default) renders the widget to an FBO and redraws it only when its data changes. Set `false` for interactive widgets (hover/pressed states) |
| `elements` | Ordered list of elements (drawn in this order) |
| `data` | Default data per element id, same fields as the Lua data table |

### 9-slice backgrounds

With `bgSlice`, the image is cut into 9 pieces: corners keep their size, edges stretch along one axis and the center stretches both ways. One frame image then fits panels of any size without distorting the corners.

The image's transparency is respected. Anything you draw under it (`bgColor`, `borderWidth`) shows through the transparent pixels. If your frame has transparent margins or rounded corners, set `bgColor.a` to `0` and `borderWidth` to `0`.

### Element types

Common fields: `id`, `type`, `x`, `y`, `w`, `h`, `enabled` (`false` skips the element), `paddingLeft`, `paddingTop`, `paddingBottom`, and `yAuto` (see [Auto layout](#auto-layout)).

| Type | What it draws | Main fields |
|------|---------------|-------------|
| `text` | A line (or paragraph) of text | `text`, `textColor`, `fontScale`, `textAlign` (`left`/`center`/`right`, within `w`), `wrap`, `lineSpacing`, `maxLines`, `cached` |
| `image` | An image | `imagePath`, `imageScale`, `borderColor`, `borderWidth`, `tooltip` |
| `rect` | A filled rectangle | `color`, `wPct`/`hPct` (size as window fraction), `alignH`, `alignV`, `borderColor`, `borderWidth` |
| `progressbar` | A bar with a `value/max` label | `barW`, `barH`, `barBg`, `barOk` (≥ 66 %), `barMid` (≥ 33 %), `barLow`, `barFontScale`, `textOffsetX/Y`. Only drawn when it receives data |
| `icons` | A row of icons picked by key | `mapping` (`{ "key": "image path" }`), `size`, `gap`. Data: `list` |
| `button` | A clickable button | See [Buttons](#buttons) |
| `widget` | Another widget, nested | `widgetRef` |
| `array` | A list of another widget | See [Arrays](#arrays-lists) |
| `scroll` | A clipped area that scrolls with the mouse wheel | See [Scroll areas](#scroll-areas) |

**Wrapped text**: with `"wrap": true` and a `w`, the text breaks into lines that fit the width. `maxLines` cuts it and ends the last line with `...`.

### Buttons

| Field | Description |
|-------|-------------|
| `text`, `textColor`, `fontScale` | Label |
| `imagePath`, `imagePathOff` | Icon for the on/off states (chosen with the data field `enabled`) |
| `btnBg`, `btnHover`, `btnPressed` | Background color per state |
| `borderColor`, `borderColorHover`, `borderColorPressed`, `borderWidth` | Border per state |
| `btnImgOffsetX/Y`, `btnTextOffsetX/Y` | Nudge the icon / label (fractions of the button) |
| `size` | Icon size. `0` stretches the image over the whole button when there is no label |
| `tooltip` | Tooltip text |
| `sound` | Sound id played once when the mouse enters the button |
| `cursor` | Cursor name while hovered |
| `options` | Named variants (see below) |

**Options** let a button switch between static variants from Lua without sending texts or paths every frame. Each option has `key`, and optionally `text`, `image`, `color`, `tooltip` and `tooltipWidget`. Select one with `{ key = "..." }`. Without a key, the first option is used.

```json
{ "id": "btnSound", "type": "button", "x": 0.8, "y": 0.1, "w": 0.1, "h": 0.1,
  "options": [
    { "key": "on",  "image": "../assets/images/ui/sound_on.png",  "tooltip": "Mute" },
    { "key": "off", "image": "../assets/images/ui/sound_off.png", "tooltip": "Unmute" }
  ] }
```

```lua
render:drawWidget("toolbar", { btnSound = { key = muted and "off" or "on" } })
```

A plain `tooltip` is shown as a small text box next to the cursor. `tooltipWidget` shows a whole widget as tooltip instead. That widget receives the option's `image`, `text` and `tooltip` as the elements `icon`, `title` and `desc`.


### Nested widgets

A `widget` element draws another widget inside this one, at its `x`, `y`. Data for its elements is passed with the prefix `<elementId>_`:

```lua
-- "cardSlot" is a widget element whose widgetRef has "name" and "hp" elements
render:drawWidget("sidePanel", {
    cardSlot_name = { text = "Barracks" },
    cardSlot_hp   = { value = 800, max = 1000 },
})
```

A nested widget with no data at all is skipped. You can switch between several nested slots by only sending data to the one you want visible.

### Arrays (lists)

An `array` element repeats the widget `widgetRef`, vertically or horizontally.

| Field | Description |
|-------|-------------|
| `widgetRef` | Row/item widget |
| `arrayCount` | Default number of items (override with `count` in data) |
| `arrayAlign` | `"vertical"` (default) or `"horizontal"` |
| `arrayOffset` | Extra space between items (fraction of the parent height) |
| `arrayDistribute` | `true` spreads the items across the whole window width/height |
| `arrayPrefix` | Prefix for item data keys (default: none) |
| `bgColor`, `borderColor`, `borderWidth` | Background behind the list and border per item |
| `arrayPaginate`, `arrayPagerWidget` | Pagination, see below |

Item `i` (0-based) receives the data keys that start with `<arrayPrefix><i>_`:

```lua
local d = { items = { count = #inventory } }
for i, it in ipairs(inventory) do
    local p = "r" .. (i - 1) .. "_"          -- widget JSON: "arrayPrefix": "r"
    d[p .. "name"] = { text = it.name }
    d[p .. "qty"]  = { text = "x" .. it.qty }
end
local _, clicked = render:drawWidget("inventory", d, "ui")
-- a button "use" inside item 3 returns clicked == "3_use"
```

Use different `arrayPrefix` values when a widget has more than one array, so their data doesn't mix.

**Pagination**: with `"arrayPaginate": true`, `arrayCount` becomes the page size. Pass the total in `count` and the current page in `page`. A pager bar (widget `arrayPager` by default, or `arrayPagerWidget`) is drawn below the list with the buttons `__prev` / `__next` and a `pageLabel` text. Handle those click ids to change page. Set `"arrayPagerTop": true` to draw the pager **above** the list instead (vertical lists only); the items move down to make room for it.

### Scroll areas

A `scroll` element draws `widgetRef` clipped to its `w × h` box. The mouse wheel scrolls it when the content is taller than the box.

| Field | Description |
|-------|-------------|
| `widgetRef` | Content widget (usually one with an array or `yAuto` elements) |
| `w`, `h` | Visible area |
| `scrollStep` | Pixels per wheel notch (default 40) |
| `scrollbar`, `scrollbarColor` | Thin scrollbar on the right edge (on by default) |

Data for the content uses the prefix `<scrollId>_`, like nested widgets. Send `{ scrollTo = 0 }` to jump back to the top. A widget that contains a scroll area is never cached.

### Auto layout

Elements with `"yAuto": true` ignore their `y` and are stacked one below the other in element order. `paddingTop` and `paddingBottom` add space around each one. Wrapped text, arrays and nested widgets report their real height, so a paragraph of any length pushes the next elements down.

## The UI Manager (editor)
---

Open it from the toolbar (UI Manager button) or from the main menu **UI Widgets**, which lists every widget file with an **Edit widget** entry.

- **Loaded widgets**: list of all widgets. Create a new one with **New**, save with **Save widget**.
- **Widget setup** sections:
  - **Layout**: size, position and scale.
  - **Background**: color, image, BG image alpha, Stretch / 9-slice mode with the four cuts and the border scale, and a preview with the cut lines.
  - **Border**.
  - **Text**: widget font and reference render height (**Use current** fills it with the current render height).
  - **Behavior**: caching and cursor.
- **Elements**: add, reorder, delete and edit elements. Each type shows its own fields: static content, button options and colors, array and pagination settings, bar colors, icon mapping…
- **Preview**: live render of the widget with zoom and pan, plus editable **preview data** to see it with sample texts and values.

Changes are applied live in the running game, so you can tune a HUD while playing.
