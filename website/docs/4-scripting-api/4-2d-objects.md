---
sidebar_position: 4
slug: 2d-objects
title: 4.4) 2D Objects
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
    400,                                            -- width
    350                                             -- height
);
```

You can resize and/or reposition an Image2D object at any time:

```lua
img:setScreenPosition(300, 300)                     -- change screen position
img:setSize(100, 100)                               -- change size
```


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