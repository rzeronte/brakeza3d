>[Back to index](https://github.com/rzeronte/brakeza3d/blob/master/doc/00-index.md)

# Camera

Brakeza3D incluye un objeto específico para referirse a la cámara: `Camera3D`.

Al fin y al cabo es un `Object3D`, por lo que podrás moverlo y rotarlo como tal.

Incluye además algunas opciones de configuración como lo son el `FOV` y tamaños del `Frustum`, que
afectarán a como y qué veámos en pantalla.

A través del objeto `Camera3D` obtendremose la matrices de `vista` y `proyección`:

- `getM3ViewMatrix`: `M3` con los datos de la matriz de vista.
- `getM3ProjectionMatrix`: `M3` con los datos de la matriz de proyección.

## Movimiento de la cámara

La cámara es un `Objeto3D`, por lo que podremos moverlo a través del 
método `setPosition(Vertex3D)` que ya hemos visto.

## Componente de Camara

Internamente Brakeza3D gestiona las operaciones de cámara a través de su propio ``component`` interno (`ComponentCamera`)

Podrás obtener acceso a este componente desde tus scripts LUA y este te dará acceso a la cámara:

```lua
    camera = componentsManager:getComponentCamera():getCamera()
```
