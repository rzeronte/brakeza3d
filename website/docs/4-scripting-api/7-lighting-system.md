---
sidebar_position: 8
title: Lighting system
description: Lighting system API in Brakeza3D including global illumination methods, LightPoint, and LightSpot objects.
---

# Lighting system
---

This section covers the LUA API for controlling lights in **Brakeza3D**.

:::tip
For conceptual understanding of the ADS model, light types, and shadow mapping, see [Global Illumination Concepts](../engine-concepts/global-illumination).
:::

## Global Illumination API
---

Access global illumination through the Render component:

```lua
local render = Components:Render()
```

### ADS Component Methods

| Method | Parameter | Description |
|--------|-----------|-------------|
| `setGlobalIlluminationAmbient(v)` | `Vertex3D` | Sets the ambient light color/intensity |
| `setGlobalIlluminationDiffuse(v)` | `Vertex3D` | Sets the diffuse light color/intensity |
| `setGlobalIlluminationSpecular(v)` | `Vertex3D` | Sets the specular light color/intensity |
| `setGlobalIlluminationDirection(v)` | `Vertex3D` | Sets the light direction (sun angle) |

:::note
ADS components use `Vertex3D` (values 0-1) instead of `Color` for convenience.
:::

### Examples

```lua
-- Set warm ambient light
Components:Render():setGlobalIlluminationAmbient(Vertex3D.new(0.3, 0.2, 0.1))

-- Set bright white diffuse
Components:Render():setGlobalIlluminationDiffuse(Vertex3D.new(1, 1, 1))

-- Set subtle specular
Components:Render():setGlobalIlluminationSpecular(Vertex3D.new(0.5, 0.5, 0.5))

-- Sun coming from top-right
Components:Render():setGlobalIlluminationDirection(Vertex3D.new(1, -1, 0.5))
```

## LightPoint
---

A point light emits light in all directions from a single position. It inherits from `Object3D`.

### Creating a LightPoint

```lua
local light = ObjectFactory.LightPoint(
    Vertex3D.new(10, 20, 30),       -- position
    Color.new(0.1, 0.1, 0.1),       -- ambient component
    Color.new(0.8, 0.6, 0.4),       -- diffuse component
    Color.new(1.0, 1.0, 1.0)        -- specular component
)
```

### Attenuation Methods

Control how light intensity decreases with distance:

| Method | Parameter | Description |
|--------|-----------|-------------|
| `setConstant(v)` | `float` | Base intensity (default: 1.0) |
| `setLinear(v)` | `float` | Linear falloff factor |
| `setCuadratic(v)` | `float` | Quadratic falloff factor |

```lua
-- Soft, far-reaching light
light:setConstant(1.0)
light:setLinear(0.09)
light:setCuadratic(0.032)

-- Intense, short-range light
light:setConstant(1.0)
light:setLinear(0.7)
light:setCuadratic(1.8)
```

### Color Methods

| Method | Parameter | Description |
|--------|-----------|-------------|
| `setAmbient(c)` | `Color` | Sets ambient contribution |
| `setColor(c)` | `Color` | Sets diffuse color |
| `setColorSpecular(c)` | `Color` | Sets specular color |

```lua
-- Change to red light
light:setColor(Color.new(1, 0, 0, 1))
light:setColorSpecular(Color.new(1, 0.5, 0.5, 1))
```

### Position and Transformation

As an `Object3D`, use standard methods:

```lua
light:setPosition(Vertex3D.new(x, y, z))
light:setEnabled(true)
light:setRemoved(true)  -- delete light
```

## LightSpot
---

A spotlight emits light in a specific direction with a cone shape. It inherits from `LightPoint`.

### Creating a LightSpot

```lua
local spot = ObjectFactory.LightSpot(
    Vertex3D.new(0, 10, 0),         -- position
    Color.new(0.0, 0.0, 0.0),       -- ambient
    Color.new(1.0, 1.0, 1.0),       -- diffuse
    Color.new(1.0, 1.0, 1.0)        -- specular
)
```

### Spotlight-Specific Methods

| Method | Parameter | Description |
|--------|-----------|-------------|
| `setCutOff(v)` | `float` | Inner cone angle (sharp edge) |
| `setOuterCutOff(v)` | `float` | Outer cone angle (soft edge) |

```lua
-- Narrow flashlight beam
spot:setCutOff(12.5)
spot:setOuterCutOff(17.5)

-- Wide floodlight
spot:setCutOff(30.0)
spot:setOuterCutOff(45.0)
```

### Direction Control

Use `LookAt` or rotation to aim the spotlight:

```lua
-- Point at an object
local target = Brakeza:getObjectByName("Enemy")
spot:LookAt(target)

-- Or set position to follow player and look down
spot:setPosition(player:getPosition() + Vertex3D.new(0, 10, 0))
spot:LookAt(player)
```

## Practical Examples
---

### Day/Night Cycle

```lua
local timeOfDay = 0

function onUpdate()
    local dt = Brakeza:getDeltaTime()
    timeOfDay = timeOfDay + dt * 0.1  -- slow cycle

    local render = Components:Render()

    -- Sun angle
    local sunY = math.sin(timeOfDay)
    local sunX = math.cos(timeOfDay)
    render:setGlobalIlluminationDirection(Vertex3D.new(sunX, sunY, 0.3))

    -- Adjust colors based on time
    if sunY > 0 then
        -- Daytime
        render:setGlobalIlluminationAmbient(Vertex3D.new(0.3, 0.3, 0.35))
        render:setGlobalIlluminationDiffuse(Vertex3D.new(1, 0.95, 0.9))
    else
        -- Nighttime
        render:setGlobalIlluminationAmbient(Vertex3D.new(0.05, 0.05, 0.1))
        render:setGlobalIlluminationDiffuse(Vertex3D.new(0.2, 0.2, 0.3))
    end
end
```

### Flickering Torch

```lua
local torch

function onStart()
    torch = ObjectFactory.LightPoint(
        Vertex3D.new(0, 5, 0),
        Color.new(0.1, 0.05, 0.0),
        Color.new(1.0, 0.6, 0.2),
        Color.new(1.0, 0.8, 0.4)
    )
    torch:setLinear(0.14)
    torch:setCuadratic(0.07)
end

function onUpdate()
    local time = Brakeza:getExecutionTime()

    -- Random flicker
    local flicker = 0.8 + math.random() * 0.4
    local pulse = 0.9 + math.sin(time * 10) * 0.1

    local intensity = flicker * pulse
    torch:setColor(Color.new(intensity, intensity * 0.6, intensity * 0.2, 1))
end
```

### Flashlight Following Player

```lua
local flashlight

function onStart()
    flashlight = ObjectFactory.LightSpot(
        Vertex3D.new(0, 0, 0),
        Color.new(0, 0, 0),
        Color.new(1, 1, 0.9),
        Color.new(1, 1, 1)
    )
    flashlight:setCutOff(15)
    flashlight:setOuterCutOff(20)
end

function onUpdate()
    local camera = Components:Camera():getCamera()

    -- Position at camera
    flashlight:setPosition(camera:getPosition())

    -- Match camera rotation
    flashlight:setRotation(camera:getRotation())
end
```

## Method Summary
---

### Global Illumination (via Components:Render())

| Method | Description |
|--------|-------------|
| `setGlobalIlluminationAmbient(Vertex3D)` | Set ambient color |
| `setGlobalIlluminationDiffuse(Vertex3D)` | Set diffuse color |
| `setGlobalIlluminationSpecular(Vertex3D)` | Set specular color |
| `setGlobalIlluminationDirection(Vertex3D)` | Set light direction |

### LightPoint

| Method | Description |
|--------|-------------|
| `setConstant(float)` | Base attenuation |
| `setLinear(float)` | Linear attenuation |
| `setCuadratic(float)` | Quadratic attenuation |
| `setAmbient(Color)` | Ambient color |
| `setColor(Color)` | Diffuse color |
| `setColorSpecular(Color)` | Specular color |

### LightSpot (inherits LightPoint)

| Method | Description |
|--------|-------------|
| `setCutOff(float)` | Inner cone angle |
| `setOuterCutOff(float)` | Outer cone angle |
