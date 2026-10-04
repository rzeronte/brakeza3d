---
sidebar_position: 15
title: Text System
description: Render text on screen using TrueType fonts with the TextWriter class in Brakeza3D.
---

# Text System
---

The **TextWriter** class allows you to render text on screen using TrueType fonts (`.ttf`). It's essential for creating HUDs, menus, debug information, and any text-based UI elements.

It also provides a **TextCache** system that lets you render text to an off-screen FBO once and blit it cheaply every frame — ideal for overlays with mostly static content.

## Creating a TextWriter
---

Use `ObjectFactory.TextWriter()` to create a text writer instance:

```lua
textWriter = ObjectFactory.TextWriter("../assets/fonts/Courier.ttf")
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `fontFile` | `string` | Path to a TrueType font file (.ttf) |

:::note
The font file path is relative to the executable. Common fonts are stored in `assets/fonts/`.
:::

:::warning
`Courier.ttf` only covers ASCII characters (0x00–0x7F). Characters with accents, tildes, or non-Latin glyphs will produce corrupted output. Use only ASCII strings with this font.
:::

## TextWriter Methods
---

### writeTextTTFAutoSize

Renders text at a specific position with automatic size calculation based on a ratio.

```lua
textWriter:writeTextTTFAutoSize(x, y, text, color, sizeRatio)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `x` | `int` | X position in pixels (from left) |
| `y` | `int` | Y position in pixels (from top) |
| `text` | `string` | The text to display |
| `color` | `Color` | Text color (RGBA) |
| `sizeRatio` | `float` | Size multiplier (1.0 = normal, 2.0 = double) |

```lua
-- Display FPS in green at top-left
textWriter:writeTextTTFAutoSize(10, 10, "FPS: " .. fps, Color.new(0, 1, 0, 1), 1.0)
```

:::note
`sizeRatio` must be ≥ 1.0 — values below 1.0 result in invisible text due to integer truncation in the size calculation.
:::

---

### writeTextTTF

Renders text with explicit width and height dimensions.

```lua
textWriter:writeTextTTF(x, y, width, height, text, color)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `x` | `int` | X position in pixels |
| `y` | `int` | Y position in pixels |
| `width` | `int` | Width of the text box |
| `height` | `int` | Height of the text box |
| `text` | `string` | The text to display |
| `color` | `Color` | Text color (RGBA) |

```lua
-- Fixed size text box
textWriter:writeTextTTF(100, 100, 200, 50, "Hello!", Color.new(1, 1, 1, 1))
```

---

### writeTextTTFMiddleScreen

Renders text centered on the screen (both horizontally and vertically).

```lua
textWriter:writeTextTTFMiddleScreen(text, color, sizeRatio)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `text` | `string` | The text to display |
| `color` | `Color` | Text color (RGBA) |
| `sizeRatio` | `float` | Size multiplier |

```lua
-- Display "GAME OVER" centered on screen
textWriter:writeTextTTFMiddleScreen("GAME OVER", Color.new(1, 0, 0, 1), 3.0)
```

---

### writeTTFCenterHorizontal

Renders text centered horizontally at a specific Y position.

```lua
textWriter:writeTTFCenterHorizontal(y, text, color, sizeRatio)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `y` | `int` | Y position in pixels (from top) |
| `text` | `string` | The text to display |
| `color` | `Color` | Text color (RGBA) |
| `sizeRatio` | `float` | Size multiplier |

```lua
-- Title centered at top of screen
textWriter:writeTTFCenterHorizontal(50, "My Game", Color.new(1, 1, 1, 1), 2.5)
```

---

### setAlpha / getAlpha

Controls the global transparency of all text rendered by this TextWriter.

```lua
textWriter:setAlpha(0.5)        -- 50% transparent
local alpha = textWriter:getAlpha()
```

| Method | Parameter | Return | Description |
|--------|-----------|--------|-------------|
| `setAlpha()` | `float` (0.0-1.0) | void | Sets global text transparency |
| `getAlpha()` | - | `float` | Gets current transparency value |

---

### setFont

Changes the font used by this TextWriter at runtime.

```lua
textWriter:setFont(newFont)
```

:::warning
This method requires a TTF_Font pointer. For changing fonts, it's recommended to create a new TextWriter instead.
:::

---

## Glyph Atlas Methods
---

The glyph atlas renders text using a pre-built texture atlas instead of creating a GL texture per call. It is significantly faster than the TTF methods and is the recommended system for any HUD or in-game UI that draws text every frame.

The engine's shared TextWriter (obtained via `Components:Render():getTextWriter()`) already has the atlas built. You do not need to create or load anything extra.

To use atlas text with your **own font**, create a TextWriter and build its atlas once:

```lua
local twTitle = ObjectFactory.TextWriter("../assets/fonts/MyFont.ttf")
twTitle:buildGlyphAtlas(512)   -- atlas texture size in pixels
```

There are two ways to draw atlas text:

| Method | When it draws |
|--------|---------------|
| `writeTextAtlasCache` | Queued; everything is drawn together by `flushTextBatch()`. Best for HUDs with many lines |
| `writeTextAtlas` (and the centered variants) | Immediately, into the `"ui"` layer or the active TextCache |

### writeTextAtlasCache

Queues text at a pixel position. Everything queued is drawn, with its own color, when you call `flushTextBatch()`.

```lua
tw:writeTextAtlasCache(x, y, text, color, scale)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `x` | `int` | X position in pixels |
| `y` | `int` | Y position in pixels (top of the text) |
| `text` | `string` | Text to render |
| `color` | `Color` | RGBA color |
| `scale` | `float` | Size multiplier (1.0 = normal, 0.5 = half) |

```lua
local tw = Components:Render():getTextWriter()
tw:writeTextAtlasCache(10, 50, "Gold: " .. gold, Color.new(1, 0.85, 0.2, 1), 0.7)
```

---

### flushTextBatch

Draws every queued `writeTextAtlasCache` call into the `"ui"` layer and empties the queue. Call it once per frame, after all your text.

```lua
tw:flushTextBatch()
tw:flushTextBatchToFB("foreground")   -- same, into another layer
```

:::warning
If you forget `flushTextBatch`, queued text never appears. If a Lua error aborts your function before reaching it, the text of that frame is lost too — wrap HUD drawing in `pcall` and flush after it.
:::

---

### measureTextWidthAtlas

Returns the width in pixels that a text would take with this writer's atlas. Use it to center, right-align or fit text.

```lua
local w = tw:measureTextWidthAtlas(text, scale)
```

```lua
-- Right-align a label at the right edge of the window
local label = "Score: " .. score
local sw = Components:Window():getWidth()
tw:writeTextAtlasCache(sw - tw:measureTextWidthAtlas(label, 0.7) - 10, 10, label, Color.new(1, 1, 1, 1), 0.7)
```

---

### writeTextAtlas

Draws text immediately at a pixel position. Inside a `beginTextCache / endTextCache` block it draws into the cache, with cache-local coordinates.

```lua
tw:writeTextAtlas(x, y, text, color, scale)
```

Same parameters as `writeTextAtlasCache`.

---

### writeTextAtlasMiddleScreen

Draws text centered on screen both horizontally and vertically (immediate).

```lua
tw:writeTextAtlasMiddleScreen(text, color, scale)
```

---

### writeTextAtlasCenterHorizontal

Draws text centered horizontally at a given Y position (immediate).

```lua
tw:writeTextAtlasCenterHorizontal(y, text, color, scale)
```

To center queued text, measure it yourself:

```lua
local text = "PRESS SPACE TO CONTINUE"
local x = (Components:Window():getWidth() - tw:measureTextWidthAtlas(text, 0.6)) / 2
tw:writeTextAtlasCache(x, 20, text, Color.new(1, 1, 1, 1), 0.6)
```

### Full example — HUD with atlas text

```lua
local tw

function onStart()
    tw = Components:Render():getTextWriter()
end

function postUpdate()
    local gold = tonumber(Components:Scripting():getGlobalScriptVar("ResourceManager", "gold")) or 0

    tw:writeTextAtlasCache(10, 10, "Gold: " .. gold,  Color.new(1, 0.85, 0.2, 1), 0.65)
    tw:writeTextAtlasCache(10, 30, "FPS: "  .. Components:Render():getFps(), Color.new(0.6, 0.6, 0.6, 1), 0.55)

    tw:flushTextBatch()
end
```

:::tip
For panels, menus and anything with a layout, [UI Widgets](./18-ui-widgets.md) are usually a better fit than placing text by hand: they handle fonts, alignment, word wrap and lists for you.
:::

---

## TextCache System
---

Every call to `writeTextTTFAutoSize` creates and destroys a GL texture internally. For overlays with mostly static content (help screens, key bindings, tech trees, etc.) this cost adds up quickly.

The **TextCache** API lets you render a block of text to a persistent off-screen FBO once, then blit the result cheaply every frame with a single draw call.

### How it works

1. Call `beginTextCache` to open a named cache and clear it.
2. Issue any number of `writeTextTTFAutoSize` / `writeTextTTF` calls — they render into the cache instead of the screen.
3. Call `endTextCache` to close the cache and restore the screen framebuffer.
4. Call `drawTextCache` every frame to blit the cached image onto the screen.

Only rebuild the cache when its content actually changes.

---

### beginTextCache

Opens a named text cache for recording. Clears it and redirects all subsequent `writeText*` calls into an off-screen FBO of the given size.

```lua
textWriter:beginTextCache(name, width, height)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `name` | `string` | Unique cache identifier |
| `width` | `int` | Cache width in pixels |
| `height` | `int` | Cache height in pixels |

:::note
Coordinates inside the begin/end block are **cache-local**: (0, 0) is the top-left of the cache, not the screen.
:::

---

### endTextCache

Closes the active cache and restores the screen framebuffer.

```lua
textWriter:endTextCache()
```

Always pair this with `beginTextCache`. Forgetting it will leave subsequent draws going to the FBO instead of the screen.

---

### drawTextCache

Blits a previously built cache to the screen at the given position.

```lua
textWriter:drawTextCache(name, x, y)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `name` | `string` | Cache identifier (must match `beginTextCache`) |
| `x` | `int` | X position on screen in pixels |
| `y` | `int` | Y position on screen in pixels |

The cache image is drawn at its original size (width × height passed to `beginTextCache`).

---

### TextCache Example — Static HUD Overlay

```lua
local tw
local cacheDirty    = true
local cacheFrame    = 0
local REBUILD_EVERY = 60   -- max frames between rebuilds

local CACHE_W = 300
local CACHE_H = 400

function onStart()
    tw = ObjectFactory.TextWriter("../assets/fonts/Courier.ttf")
end

function onUpdate()
    local sw = Components:Window():getWidth()
    local x  = sw - CACHE_W - 10
    local y  = 80

    -- Rebuild the cache when content changes or periodically
    cacheFrame = cacheFrame + 1
    if cacheDirty or cacheFrame >= REBUILD_EVERY then
        cacheDirty = false
        cacheFrame = 0

        -- Coordinates inside begin/end are relative to the cache (0,0 = top-left)
        tw:beginTextCache("helpOverlay", CACHE_W, CACHE_H)
            tw:writeTextTTFAutoSize(0,   0, "CAMERA",         Color.new(0, 0.2, 0.8, 1), 0.9)
            tw:writeTextTTFAutoSize(0,  22, "WASD      move", Color.new(0.2, 0.2, 0.2, 1), 0.8)
            tw:writeTextTTFAutoSize(0,  42, "Q/E       zoom", Color.new(0.2, 0.2, 0.2, 1), 0.8)
            tw:writeTextTTFAutoSize(0,  62, "H         reset", Color.new(0.2, 0.2, 0.2, 1), 0.8)
            tw:writeTextTTFAutoSize(0,  90, "SELECTION",      Color.new(0, 0.2, 0.8, 1), 0.9)
            tw:writeTextTTFAutoSize(0, 112, "LClick    select",Color.new(0.2, 0.2, 0.2, 1), 0.8)
            tw:writeTextTTFAutoSize(0, 132, "LDrag     area",  Color.new(0.2, 0.2, 0.2, 1), 0.8)
            tw:writeTextTTFAutoSize(0, 152, "RClick    move",  Color.new(0.2, 0.2, 0.2, 1), 0.8)
        tw:endTextCache()
    end

    -- One blit per frame regardless of how many lines are inside
    tw:drawTextCache("helpOverlay", x, y)

    -- Dynamic content rendered normally on top
    local formation = tostring(Components:Scripting():getGlobalScriptVar("Orders", "formation") or "circle")
    tw:writeTextTTFAutoSize(x, y + CACHE_H + 8, "Formation: " .. formation, Color.new(0, 0.5, 0.1, 1), 0.85)
end
```

---

## Complete Example
---

### HUD Display

```lua
local textWriter
local startTime

function onStart()
    textWriter = ObjectFactory.TextWriter("../assets/fonts/Courier.ttf")
    startTime = Brakeza:getExecutionTime()
end

function onUpdate()
    local screenW = Components:Window():getWidth()
    local screenH = Components:Window():getHeight()

    -- Title centered at top
    textWriter:writeTTFCenterHorizontal(
        20,
        "BRAKEZA3D DEMO",
        Color.new(1, 1, 1, 1),
        2.0
    )

    -- FPS counter (top-right)
    local fps = Components:Render():getFps()
    textWriter:writeTextTTFAutoSize(
        screenW - 120, 10,
        "FPS: " .. fps,
        Color.new(0, 1, 0, 1),
        1.0
    )

    -- Timer (top-left)
    local elapsed = math.floor(Brakeza:getExecutionTime() - startTime)
    textWriter:writeTextTTFAutoSize(
        10, 10,
        "Time: " .. elapsed .. "s",
        Color.new(1, 1, 0, 1),
        1.0
    )

    -- Instructions (bottom-center)
    textWriter:writeTTFCenterHorizontal(
        screenH - 40,
        "Press WASD to move",
        Color.new(0.7, 0.7, 0.7, 1),
        1.0
    )
end
```

### Fade In/Out Text

```lua
local textWriter
local alpha = 0
local fadeIn = true

function onStart()
    textWriter = ObjectFactory.TextWriter("../assets/fonts/Courier.ttf")
end

function onUpdate()
    local dt = Brakeza:getDeltaTime()

    if fadeIn then
        alpha = alpha + dt * 0.5
        if alpha >= 1 then alpha = 1; fadeIn = false end
    else
        alpha = alpha - dt * 0.5
        if alpha <= 0 then alpha = 0; fadeIn = true end
    end

    textWriter:setAlpha(alpha)
    textWriter:writeTextTTFMiddleScreen(
        "Press START",
        Color.new(1, 1, 1, 1),
        2.0
    )
end
```

### Multi-line Menu

```lua
local textWriter
local menuItems = {"New Game", "Load Game", "Options", "Exit"}
local selectedIndex = 1

function onStart()
    textWriter = ObjectFactory.TextWriter("../assets/fonts/Courier.ttf")
end

function onUpdate()
    local input   = Components:Input()
    local screenH = Components:Window():getHeight()

    if input:isCharFirstEventDown("W") then
        selectedIndex = selectedIndex - 1
        if selectedIndex < 1 then selectedIndex = #menuItems end
    end
    if input:isCharFirstEventDown("S") then
        selectedIndex = selectedIndex + 1
        if selectedIndex > #menuItems then selectedIndex = 1 end
    end

    local startY = screenH / 2 - (#menuItems * 30) / 2

    for i, item in ipairs(menuItems) do
        local color
        if i == selectedIndex then
            color = Color.new(1, 1, 0, 1)
            item = "> " .. item .. " <"
        else
            color = Color.new(0.7, 0.7, 0.7, 1)
        end
        textWriter:writeTTFCenterHorizontal(startY + (i - 1) * 40, item, color, 1.5)
    end
end
```

## Tips and Best Practices
---

1. **Create once, use many**: Create your TextWriter in `onStart()` and reuse it in `onUpdate()`.

2. **Use TextCache for static content**: Any overlay that doesn't change every frame (key bindings, tech trees, help screens) should use `beginTextCache` / `endTextCache` / `drawTextCache` to avoid per-frame GL texture allocation.

3. **Rebuild only when dirty**: Track a `dirty` flag and rebuild the cache only when content actually changes. Add a periodic max interval (e.g. every 60 frames) as a safety net.

4. **Cache coordinates are local**: Inside a `beginTextCache` / `endTextCache` block, (0, 0) is the top-left of the cache, not the screen. `drawTextCache(name, x, y)` places the result at screen position (x, y).

5. **ASCII only with Courier.ttf**: The bundled `Courier.ttf` covers only ASCII (0x00–0x7F). Use only ASCII strings; non-ASCII characters render as garbage.

6. **Screen coordinates**: (0, 0) is the top-left corner of the window.

7. **Available fonts**: Check `assets/fonts/` for included fonts:
   - `Courier.ttf` — Monospace font (good for debug info, ASCII only)
   - Other TTF fonts you add to the project.

## Method Summary
---

| Method | Description |
|--------|-------------|
| `writeTextTTFAutoSize(x, y, text, color, ratio)` | Text at position with auto-sizing |
| `writeTextTTF(x, y, w, h, text, color)` | Text with explicit dimensions |
| `writeTextTTFMiddleScreen(text, color, ratio)` | Text centered on screen |
| `writeTTFCenterHorizontal(y, text, color, ratio)` | Text centered horizontally |
| `setAlpha(alpha)` | Set global transparency |
| `getAlpha()` | Get current transparency |
| `setFont(font)` | Change font (advanced) |
| `beginTextCache(name, w, h)` | Start recording text into a named off-screen FBO |
| `endTextCache()` | Stop recording and restore screen framebuffer |
| `drawTextCache(name, x, y)` | Blit a cached text FBO to the screen |
| `buildGlyphAtlas(size)` | Build the glyph atlas for this writer's font (needed for custom fonts) |
| `writeTextAtlasCache(x, y, text, color, scale)` | Queue colored atlas text (drawn by `flushTextBatch`) |
| `flushTextBatch()` / `flushTextBatchToFB(fb)` | Draw all queued atlas text (once per frame) |
| `measureTextWidthAtlas(text, scale)` | Width in pixels of a text with the atlas |
| `writeTextAtlas(x, y, text, color, scale)` | Immediate atlas text (draws into the active TextCache inside a cache block) |
| `writeTextAtlasMiddleScreen(text, color, scale)` | Immediate atlas text centered on screen |
| `writeTextAtlasCenterHorizontal(y, text, color, scale)` | Immediate atlas text centered horizontally |
