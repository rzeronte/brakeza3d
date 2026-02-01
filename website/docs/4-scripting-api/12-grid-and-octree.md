---
sidebar_position: 13
title: Grid and Octree
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
eye:BuildOctree(1) -- crea una árbol de octanos de un solo nivel de profundidad, solo 8 hijos
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

Mediante el método setTravel(x1, y1, z1, x2, y2, z2), podremos ajustar el viaje para el próximo camino
solicitado mediante makeTravelCubesGrid.

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

## Click object detection
---

Brakeza3D ofrece de forma nativa la funcionalidad de click sobre un Object3D. Para ello po

# Complete LUA Code Examples

https://github.com/rzeronte/brakeza3d/blob/master/doc/examples-lua-code.md_
