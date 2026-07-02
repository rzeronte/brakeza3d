---
sidebar_position: 13
title: Grid and Octree
description: Grid3D and Octree data structures in Brakeza3D for spatial partitioning and A* pathfinding.
---

# Grid3D and Octree
---

**Brakeza3D** includes Grid3D and Octree data structures integrated into Mesh3D objects.

- `Grid3D`: Creates a grid of X, Y, Z dimensions over the AABB of the given object.
- `Octree`: Creates an octree with the specified depth (maxDepth).

To create a grid in a Mesh3D object, use BuildGrid3D(sizeX, sizeY, sizeZ):

```lua
eye = ObjectFactory.Mesh3D(
    "../assets/models/Capsule.fbx",
    Vertex3D.new(x, y, z)
)
eye:BuildGrid3D(5, 5, 5)                                                    -- creates a 5x5x5 grid
```

To create an octree use `BuildOctree(int maxDepth)`:

```lua
eye = ObjectFactory.Mesh3D("../assets/models/Capsule.fbx", Vertex3D.new(x, y, z))
eye:BuildOctree(1) -- creates an octree with a single depth level, only 8 children
```

To readjust the dimensions of both grids and octree structures, simply rerun `buildGrid3D()` or `buildOctree()`.

## Filling Geometry in Grid3D

Grid3D stores a boolean flag for each cell. Brakeza3D provides a method fillGrid3DFromGeometry, which
sets this flag for each cell if no triangle intersects or is contained in it:

```lua
eye = ObjectFactory.Mesh3D("../assets/models/Capsule.fbx", Vertex3D.new(x, y, z))
eye:BuildGrid3D(5, 5, 5)
eye:FillGrid3DFromGeometry()                -- sets flags for empty cells
```

This is especially useful for pathfinding techniques.

## Pathfinding in Grid3D

Grids include an A* algorithm that allows iterating over their cells. Combined with fillGrid3DFromGeometry,
paths can avoid cells with geometry:

```lua
eye = ObjectFactory.Mesh3D("../assets/models/Capsule.fbx", Vertex3D.new(x, y, z))
eye:BuildGrid3D(5, 5, 5)
eye:FillGrid3DFromGeometry()                    -- fill with geometry

eye:getGrid3D():setTravel(0, 0, 0, 5, 5, 5)     -- set travel from (0,0,0) to (5,5,5)
path = eye:getGrid3D():MakeTravelCubesGrid()    -- returns an array of CubeGrid3D
```

Using the method `setTravel(x1, y1, z1, x2, y2, z2)`, you can define the start and end points for the next path request made through `MakeTravelCubesGrid()`.

The method MakeTravelCubesGrid returns the requested path as an array of CubeGrid3D structures:

```cpp
struct CubeGrid3D {
    AABB3D box;             // bounding box
    int posX;               // X index in the grid
    int posY;               // Y index
    int posZ;               // Z index
    bool passed = true;     // Flag
};
```


You can iterate over this array to obtain the desired path:

```lua
...
path = eye:getGrid3D():MakeTravelCubesGrid()            -- we get a travel path

for i, cube in ipairs(path) do
    print("Cube " .. i .. ": X = " .. cube.posX .. ", Y = " .. cube.posY .. ", Z = " .. cube.posZ)
end
...
```

## Retrieving Grid3D and Octree

Use `getGrid3D()` and `getOctree()` to get the data structures attached to a mesh after they have been built.

```lua
local grid   = mesh:getGrid3D()   -- returns the Grid3D, or nil if not built
local octree = mesh:getOctree()   -- returns the Octree, or nil if not built
```

---

## Filling Grid3D from an Image

Instead of filling the grid from mesh geometry, you can paint walkability directly from a PNG image. Black pixels (below `threshold`) become obstacles; everything else is walkable.

```lua
grid:fillGrid3DFromImage(path, threshold)
grid:fillGrid3DFromImage(path, threshold, flipZ, flipX)
```

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `path` | `string` | — | Path to a PNG image |
| `threshold` | `int` | 128 | Pixels with brightness below this are obstacles |
| `flipZ` | `bool` | false | Flip the image along the Z axis |
| `flipX` | `bool` | false | Flip the image along the X axis |

```lua
mesh:BuildGrid3D(512, 1, 512)
mesh:getGrid3D():fillGrid3DFromImage("../assets/nav/walkable.png", 128)
```

---

## Querying Walkability

### isCellWalkable

Returns whether a grid cell at grid indices `(x, z)` is walkable (i.e. not blocked by geometry or image data).

```lua
local walkable = grid:isCellWalkable(gx, gz)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `gx` | `int` | Grid column index |
| `gz` | `int` | Grid row index |

### snapToWalkable

Given a grid position that may be blocked, finds the nearest walkable cell within `maxRadius` cells (spiral search).

```lua
local nx, nz = grid:snapToWalkable(gx, gz, maxRadius)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `gx` | `int` | Starting grid column |
`gz` | `int` | Starting grid row |
| `maxRadius` | `int` | Maximum search radius in cells |

Returns `(gx, gz)` of the nearest walkable cell, or the original position if none is found within the radius.

```lua
-- Convert world position to grid, snap if blocked
local GRID_SIZE  = 512
local WORLD_SIZE = 1024.0
local function toGrid(w) return math.floor((w + WORLD_SIZE * 0.5) / WORLD_SIZE * GRID_SIZE) end

local gx = toGrid(unit:getPosition().x)
local gz = toGrid(unit:getPosition().z)

if not grid:isCellWalkable(gx, gz) then
    gx, gz = grid:snapToWalkable(gx, gz, 5)
end
```

---

## Click Object Detection
---

**Brakeza3D** provides native functionality for detecting clicks on `Object3D` instances using raycasting.

To detect if an object was clicked, you can use the `isRayCollisionWith()` method from the Collisions component. This method casts a ray from one point to another and checks if it intersects with a specific object.

### Basic Click Detection Example

```lua
function onUpdate()
    local input = Components:Input()

    if input:isClickLeft() then
        -- Get mouse position relative to renderer
        local mouseX = input:getRelativeRendererMouseX()
        local mouseY = input:getRelativeRendererMouseY()

        -- Get camera for ray origin
        local camera = Components:Camera():getCamera()
        local rayOrigin = camera:getPosition()

        -- Calculate ray direction from camera through mouse position
        -- (simplified - actual implementation depends on your projection)
        local rayEnd = Vertex3D.new(mouseX, mouseY, 100)

        -- Check collision with a specific object
        local targetObject = Brakeza:getObjectByName("MyObject")
        if Components:Collisions():isRayCollisionWith(rayOrigin, rayEnd, targetObject) then
            print("Object clicked!")
        end
    end
end
```

### Method Reference

| Method | Parameters | Return | Description |
|--------|------------|--------|-------------|
| `isRayCollisionWith()` | `from: Vertex3D, to: Vertex3D, object: Object3D` | `bool` | Returns true if the ray from `from` to `to` intersects with the specified object |

:::note
For click detection to work properly, the target object must have collisions enabled with an appropriate collider shape configured.
:::
