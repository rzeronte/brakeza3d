# Camera

Brakeza3D incluye un objeto específico para referirse a la cámara: `Camera3D`.

Al fin y al cabo es un `Object3D`, por lo que podrás moverlo y rotarlo como tal.

Incluye además algunas opciones de configuración como lo son el `FOV` y tamaños del `Frustum`, que
afectarán a como y qué veámos en pantalla.

A través del objeto `Camera3D` obtendremose la matrices de `vista` y `proyección`:

- `getViewMatrix`: mat4 con los datos de la matriz de vista.
- `getProjectionMatrix`: mat4 con los datos de la matriz de proyección.

# Movimiento de la cámara

Como ya hemos mencionado, la cámara es un `Objeto3D`, por lo que podremos moverlo a través del 
método `setPosition(Vertex3D)` que ya hemos visto.

No obstante, la cámara ya implementa mecanismos de movimiento y velocidad que nos facilitarán la
manipulación de algunos aspectos.

# Velocidad

Existen tres parámetros que determinarán nuestra velocidad:
 
- `speed`: Fuerza del movimiento hacia adelante/atrás
- `strafe`: Fuerza del movimiento hacia izquierda/derecha.
- `jump`: Fuerza del movimiento hacia arriba.

Podrás manipular estos valores desde la GUI. 

En cada frame y en función de dichos valores, se actualiza la propiedad ``velocity``, cuyo tipo es `Vertex3D` y el cual será
sumado al valor actual de la posición de la cámara.


# FreeLook

Si activas la opcion ``Free Look`` en Brakeza3D ( Render > Free Look > On), podrás girar el ratón con libertad y moverte con las teclas
 `A`,`S`,`W`,`D`.



