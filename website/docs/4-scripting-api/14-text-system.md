---
sidebar_position: 15
title: Text System
description: Render text on screen using TrueType fonts with the TextWriter class in Brakeza3D.
---

# Text System
---

The **TextWriter** class allows you to render text on screen using TrueType fonts (`.ttf`). It's essential for creating HUDs, menus, debug information, and any text-based UI elements.

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

    -- Fade logic
    if fadeIn then
        alpha = alpha + dt * 0.5
        if alpha >= 1 then
            alpha = 1
            fadeIn = false
        end
    else
        alpha = alpha - dt * 0.5
        if alpha <= 0 then
            alpha = 0
            fadeIn = true
        end
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
    local input = Components:Input()
    local screenH = Components:Window():getHeight()

    -- Navigation
    if input:isCharFirstEventDown("W") then
        selectedIndex = selectedIndex - 1
        if selectedIndex < 1 then selectedIndex = #menuItems end
    end
    if input:isCharFirstEventDown("S") then
        selectedIndex = selectedIndex + 1
        if selectedIndex > #menuItems then selectedIndex = 1 end
    end

    -- Render menu items
    local startY = screenH / 2 - (#menuItems * 30) / 2

    for i, item in ipairs(menuItems) do
        local color
        if i == selectedIndex then
            color = Color.new(1, 1, 0, 1)  -- Yellow for selected
            item = "> " .. item .. " <"
        else
            color = Color.new(0.7, 0.7, 0.7, 1)  -- Gray for others
        end

        textWriter:writeTTFCenterHorizontal(
            startY + (i - 1) * 40,
            item,
            color,
            1.5
        )
    end
end
```

## Tips and Best Practices
---

1. **Create once, use many**: Create your TextWriter in `onStart()` and reuse it in `onUpdate()`

2. **Performance**: Text rendering happens every frame. Avoid creating new TextWriter instances in `onUpdate()`

3. **Screen coordinates**: Remember that (0,0) is the top-left corner

4. **Color alpha**: The Color's alpha component affects transparency independently of `setAlpha()`

5. **Available fonts**: Check `assets/fonts/` for included fonts:
   - `Courier.ttf` - Monospace font (good for debug info)
   - Other TTF fonts you add to the project

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
