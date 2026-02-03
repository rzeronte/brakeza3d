---
title: Shader Node Editor
description: Complete reference for the visual shader node editor in Brakeza3D, including all node types and their connections.
QQsidebar_position: 7
---

# Shader Node Editor
---

The **Node Editor** provides a visual way to create shaders without writing GLSL code. Connect nodes to build complex effects intuitively.

## Getting Started
---

1. Open **Window** → **File Browser**
2. Navigate to the **Shaders** section
3. Create a new shader or select an existing one
4. Choose **Node-based** as the shader type
5. The Node Editor will open automatically

## Node Editor Interface
---

| Element | Description |
|---------|-------------|
| **Canvas** | The main area where you place and connect nodes |
| **Context Menu** | Right-click to add new nodes |
| **Node** | A processing unit with inputs and outputs |
| **Pin** | Connection point on a node (input on left, output on right) |
| **Link** | A connection between two pins |

### Navigation

- **Pan**: Middle mouse button + drag, or hold Space + left mouse drag
- **Zoom**: Mouse scroll wheel
- **Select**: Left-click on a node
- **Multi-select**: Ctrl + left-click, or drag a selection box
- **Delete**: Select node/link and press Delete

## Data Types
---

Nodes use different data types for their pins:

| Type | Color | Description |
|------|-------|-------------|
| `float` | Gray | Single value (e.g., time, intensity) |
| `vec2` | Green | Two values (e.g., UV coordinates) |
| `vec3` | Blue | Three values (e.g., RGB color, position) |

:::note
You can only connect pins of compatible types. The editor will reject incompatible connections.
:::

## Node Reference
---

### Input Nodes

These nodes provide data to your shader.

| Node | Output Type | Description |
|------|-------------|-------------|
| **Time** | `float` | Current time in seconds (for animations) |
| **Float Value** | `float` | A constant float value you can configure |
| **UV Coords** | `vec2` | Texture coordinates of the current fragment |
| **Color** | `vec3` | A constant RGB color you can configure |

#### Time Node
Outputs the elapsed time, useful for animated effects.

```
[Time] ─→ float (continuously increasing value)
```

#### Float Value Node
Outputs a configurable constant value.

```
[Float Value] ─→ float (user-defined constant)
```
- **Properties**: `value` - The float value to output

#### UV Coords Node
Outputs the UV texture coordinates.

```
[UV Coords] ─→ vec2 (x: 0-1, y: 0-1)
```

#### Color Node
Outputs a configurable RGB color.

```
[Color] ─→ vec3 (RGB values 0-1)
```
- **Properties**: Click to open color picker

---

### Texture Nodes

| Node | Input | Output | Description |
|------|-------|--------|-------------|
| **Image** | UV (vec2) | Color (vec3), Alpha (float) | Sample an external image texture |
| **Mesh Texture** | UV (vec2) | Color (vec3), Alpha (float) | Sample the object's diffuse texture |
| **Internal Texture** | UV (vec2) | Color (vec3), Alpha (float) | Sample the scene render (postprocessing) |

#### Mesh Texture Node
Samples the texture assigned to the current mesh. Essential for object shaders.

```
UV (vec2) ─→ [Mesh Texture] ─→ Color (vec3)
                            ─→ Alpha (float)
```

---

### Math Nodes (Float)

Operations on single float values.

| Node | Inputs | Output | Operation |
|------|--------|--------|-----------|
| **AddFloat** | A, B (float) | float | A + B |
| **SubtractFloat** | A, B (float) | float | A - B |
| **MultiplyFloat** | A, B (float) | float | A × B |
| **Sin** | Input (float) | float | sin(Input) |
| **Cos** | Input (float) | float | cos(Input) |
| **Smoothstep** | Value (float) | float | smoothstep(edge0, edge1, Value) |

#### Sin / Cos Nodes
Generate wave patterns. Perfect for pulsing effects.

```
Time ─→ [Sin] ─→ float (-1 to 1)
```

#### Smoothstep Node
Smooth interpolation between 0 and 1.
- **Properties**: `edge0`, `edge1` - The interpolation range

---

### Math Nodes (Vec3)

Operations on color/vector values.

| Node | Inputs | Output | Operation |
|------|--------|--------|-----------|
| **Add** | A, B (vec3) | vec3 | A + B |
| **Subtract** | A, B (vec3) | vec3 | A - B |
| **Multiply** | A, B (vec3) | vec3 | A × B (component-wise) |
| **Mix** | A, B (vec3), Factor (float) | vec3 | mix(A, B, Factor) |

#### Mix Node
Blends between two colors based on a factor (0 = A, 1 = B).

```
Color A (vec3) ─→ [Mix] ─→ Result (vec3)
Color B (vec3) ─→
Factor (float) ─→
```

---

### Converter Nodes

| Node | Inputs | Outputs | Description |
|------|--------|---------|-------------|
| **Vec2Compose** | X, Y (float) | Vec2 | Combine two floats into vec2 |
| **Vec2Decompose** | Vec2 | X, Y (float) | Split vec2 into two floats |

---

### UV Manipulation Nodes

| Node | Inputs | Output | Description |
|------|--------|--------|-------------|
| **UV Offset** | UV, Offset (vec2) | vec2 | Shifts UV coordinates |
| **UV Scale** | UV, Scale (vec2) | vec2 | Scales UV coordinates |
| **UV Rotate** | UV (vec2), Angle (float) | vec2 | Rotates UV around center |

#### UV Offset Example
Create a scrolling texture effect:

```
[UV Coords] ─────────────────────→ [UV Offset] ─→ [Mesh Texture]
[Time] ─→ [MultiplyFloat] ─→ [Vec2Compose] ─→
```

---

### Effect Nodes

| Node | Inputs | Output | Description |
|------|--------|--------|-------------|
| **Gradient** | UV (vec2) | vec3 | Creates a gradient from UV coordinates |
| **Grayscale** | Color (vec3) | float | Converts color to grayscale value |
| **Invert** | Color (vec3) | vec3 | Inverts colors (1 - color) |
| **Contrast** | Color (vec3) | vec3 | Adjusts color contrast |
| **Saturation** | Color (vec3) | vec3 | Adjusts color saturation |
| **Vignette** | UV (vec2) | float | Creates vignette mask (dark edges) |

---

### Output Node

Every shader must have exactly one **Output** node.

| Node | Input | Description |
|------|-------|-------------|
| **Output** | Color (vec3) | Final color sent to the screen |

---

## Common Patterns
---

### Pulsing Color

```
[Time] ─→ [Sin] ─→ [Smoothstep] ─→ Factor
[Color A] ─→ [Mix] ─→ [Output]
[Color B] ─→
```

### Scrolling Texture

```
[UV Coords] ─→ UV ─→ [UV Offset] ─→ [Mesh Texture] ─→ [Output]
[Time] ─→ [MultiplyFloat] ─→ X ─→ [Vec2Compose] ─→ Offset
[Float: 0.1] ─→ B            Y ─→
```

### Wavy Distortion

```
[Time] ─→ [Sin] ─→ [MultiplyFloat] ─→ X ─→ [Vec2Compose] ─→ [UV Offset]
[Time] ─→ [Cos] ─→ [MultiplyFloat] ─→ Y ─→
[Float: 0.02] ─→
```

### Color Tint

```
[Mesh Texture] ─→ [Multiply] ─→ [Output]
[Color: Orange] ─→
```

## Troubleshooting
---

| Issue | Solution |
|-------|----------|
| Shader shows magenta | Output node is not connected |
| No visible effect | Check that all required inputs are connected |
| Texture appears black | Ensure UV Coords are connected to texture input |
| Animation not working | Verify Time node is connected somewhere in the graph |

## Performance Tips
---

1. **Minimize texture samples**: Each texture lookup has a cost
2. **Reuse calculations**: Connect one node's output to multiple inputs
3. **Use simple math**: Sin/Cos are more expensive than Add/Multiply
4. **Test on target hardware**: Complex shaders may affect performance on older GPUs
