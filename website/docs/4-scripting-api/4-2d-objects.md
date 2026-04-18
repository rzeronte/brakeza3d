---
sidebar_position: 4
slug: 2d-objects
title: 2D Objects
description: 2D objects in Brakeza3D including Image2D, Image2DAnimation, and text rendering with TTF fonts.
---

# 2D objects
---
Although **Brakeza3D** is a 3D engine, it also offers **robust 2D capabilities**. You can work with 2D images, create smooth 2D animations, and handle flat elements just as easily as 3D objects, making it versatile for projects that mix both dimensions or focus solely on 2D experiences.

---

## Image2D
---

Image2D is a 2D object that draws an image on screen:

```lua
img = ObjectFactory.Image2D(
    "../assets/images/logo_small.png",              -- file
    10,                                             -- screen xpos
    10,                                             -- screen ypos
    1.0,                                            -- width scale  (0.0–1.0, 1.0 = full screen width)
    1.0                                             -- height scale (0.0–1.0, 1.0 = full screen height)
);
```

You can resize and/or reposition an Image2D object at any time:

```lua
img:setScreenPosition(300, 300)                     -- change screen position
img:setSize(0.5, 0.5)                               -- half screen size
```

### Video playback on Image2D

An Image2D can play a video file (MP4, AVI, etc.) as an overlay using the non-blocking mode.
The video updates every engine frame and the Image2D renders it automatically.

```lua
-- Load and start a video
img:loadVideo("../assets/videos/intro.mp4")

-- Check whether playback has ended
if img:isVideoFinished() then
    img:stopVideo()
end

-- Check whether a video is currently active
if img:hasVideo() then
    print("video is playing")
end
```

#### Full overlay example

```lua
local overlay = nil
local done    = false

function onStart()
    overlay = ObjectFactory.Image2D("", 0, 0, 1.0, 1.0)
    overlay:setScreenPosition(0, 0)
    overlay:loadVideo("../assets/videos/intro.mp4")
end

function onUpdate()
    if done then return end
    if overlay ~= nil and overlay:isVideoFinished() then
        overlay:stopVideo()
        Brakeza:removeObject(overlay)
        overlay = nil
        done = true
    end
end
```

### Blocking cutscene

Use `PlayVideoCutscene` when you need a full-screen video that **blocks** execution until it finishes
(e.g. intro or between-level cinematics):

```lua
function onStart()
    ObjectFactory:PlayVideoCutscene("../assets/videos/cutscene.mp4")
    -- execution resumes here only after the video ends
end
```

:::note
Video files must be placed inside `assets/videos/`. Supported formats depend on the FFmpeg build
included with the engine (MP4/H.264 recommended).
:::


## Image2DAnimation
---
Draws an animation on screen. Essentially, it is a collection of Image2D objects.

It is designed to load animations from a spritesheet, i.e., an image grid where each cell is a frame of
the animation. You must specify the frame width, frame height, and number of frames so the function can
properly extract the sprite.

You can configure the display speed (fps) of the animation:

```lua
animation = ObjectFactory.Image2DAnimation(
    "../assets/sprites/explosion_a.png",
    int x,                                          -- screen xpos
    int y,                                          -- screen ypos
    int sw,                                         -- sprite width
    int sh,                                         -- sprite height
    int frames,                                     -- num frames
    int fps                                         -- frames per second
)
```

## Text on Screen
---

**Brakeza3D** allows writing text on screen from Lua scripts using the TextWriter object. You need a TTF font
file, then create the object and start using it.

Methods include:

| Function                                              | Positioning                  | Size Handling                        | Description                                                     |
|-------------------------------------------------------|------------------------------|--------------------------------------|-----------------------------------------------------------------|
| `writeTextTTF(x, y, width, height, text, Color)`      | At coordinates `(x, y)`      | Fixed width and height               | Draws text at the specified position with explicit dimensions   |
| `writeTextTTFAutoSize(x, y, text, Color, sizeRatio)`  | At coordinates `(x, y)`      | Auto-calculated based on `sizeRatio` | Draws text at position, automatically adjusting dimensions      |
| `writeTextTTFMiddleScreen(text, Color, sizeRatio)`    | Centered on screen           | Auto-calculated based on `sizeRatio` | Draws text centered both horizontally and vertically            |
| `writeTTFCenterHorizontal(y, text, Color, sizeRatio)` | Centered horizontally at `y` | Auto-calculated based on `sizeRatio` | Draws text centered horizontally at the given vertical position |

Example of use:

```lua
function onStart()
    textWriter = ObjectFactory.TextWriter("../assets/fonts/Courier.ttf")
end
```

```lua
function onUpdate()
     textWriter:writeTTFCenterHorizontal(
        15,                                         -- screen ypos
        "Centered H!",                              -- text
        Color.new(0, 1, 0, 1),                      -- color
        1.5                                         -- ratio size
    )
```

```lua
    textWriter:writeTextTTF(
        100,                                        -- screen xpos
        100,                                        -- screen ypos
        100,                                        -- width
        100,                                        -- height
        "Hello",                                    -- text
        Color.new(1, 0, 0, 1)                       -- color
    )
end
```