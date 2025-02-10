# Index
- [1) Basics](#1-basics)
- [2) Basic Objects types](#2-basic-objects-types)
- [3) Camera3D object](#3-camera3d-object)
- [4) Basic 2D object types](#4-basic-2d-object-types)
- [5) Basic 3D object types](#5-basic-3d-object-types)
- [6) Collision between objects](#6-collision-between-objects)
- [7) Light objects types](#7-light-objects-types)
- [8) Particle emitter](#8-particle-emitter)
- [9) Input system](#9-input-system)
- [10) Global illumination](#10-global-illumination)
- [11) Sound system](#11-sound-system)
- [12) Scripting system](#12-scripting-system)
- [13) Shaders system](#13-shaders-system)

---

>[Back to index](#index)

# 1) Basics

Brakeza3D es un sencillo motor 2D/3D multiplataforma para desarrollo de videojuegos.

Incluye el motor de render, un scene graph, físicas de colisiones, scripting y una sencilla GUI.

Aunque el motor está escrito en C++, en esta documentación aprenderemos a crear nuestros propios
proyectos usando LUA como lenguaje de scripting.

 
- [Conceptos principales](#conceptos-principales)
- [Componentes](#componentes)
- [GUI](#gui)

---

## Conceptos principales

En Brakeza3D existen cuatro conceptos principales:

- `Proyectos`
- `Scripts`
- `Escenas`
- `Objetos`

### 1) Proyectos

Un proyecto es un contenedor de `escenas` y `scripts`. Se pueden almacenar en disco como fichero `JSON` y cargarlo en cualquier momento.

El proyecto es la figura de mayor nivel en Brakeza3D. Por lo general tendrás un `Proyecto`, por cada aplicación en
la que estés trabajando.

### 2) Scripts

Ficheros de código fuente LUA para interactuar con Brakeza3D. Los scripts ejecutarán código que dará vida a tu aplicación.

Pueden vincularse a `proyectos`, `escenas` u `objetos` para implementar la lógica deseada.

Se pueden almacenar en disco como fichero `LUA` para cargarlo posteriormente. Puedes crear tantos scripts como necesites.

En la sección de scripting profundizaremos en este tipo de ficheros.

### 3) Escenas

Una escena es un contenedor de `objetos` y `scripts`.

En una escena podemos encontrar:

- `Objetos`
- `Scripts`
- `Posición de cámara`
- `Configuración de iluminación ADS`
- `Shaders`

Se pueden almacenar en disco y recuperar en cualquier momento.

### 4) Objetos

Entidades existente en nuestro mundo 3D. En líneas generales, cualquier cosa susceptible
de ser representada en pantalla será un objeto: *un modelo 3D*, *una luz*, *emisor de partículas*...

Todos los objetos comparten las propiedades básicas de un elemento 3D ya sea nombre, posición, rotación, tamaño, pero cada tipo de objeto específico ampliará estas segun su caso.

Desde código podremos activar, mover, rotar, eliminar, escalar y muchas otras acciones sobre los objetos.

---

### GUI

Brakeza3D incorpora una sencilla GUI que nos facilitará algunas de las operaciones más tediosas.

Dispondrás de un sencillo editor de escena totalmente integrado con el sistema de scripting.

Si bien puedes utilizar tu editor de código favorito para manipular los scripts, la GUI será de gran utilidad
para integrar tus scripts, manipular tus objetos, configurar la iluminación, guardar y cargar escenas, etc.

---

### Componentes

Brakeza3D agrupa sus funcionalidades en `Componentes`. Cada componente representa un aspecto
fundamental del núcleo de Brakeaz3D.

- `ComponentWindow`: Gestiona la ventana del sistema operativo.
- `ComponentScripting`: Gestiona el sistema de scripting.
- `ComponentCamera`: Gestiona la cámara.
- `ComponentCollisions`: Gestiona el motor de físicas y colisiones.
- `ComponentInput`: Gestiona la entrada por teclado o gamepad.
- `ComponentSound`: Gestiona la reproducción de sonidos.
- `ComponentRender`: Gestiona el render con OpenGL

Podrás acceder a todos ellos mediante el ``ComponentsManager`` desde tus scripts LUA. Trataremos
cada uno en detalle en el futuro.

---

>[Back to index](#index)

# 2) Basic Objects types

- [El objeto Object3D](#el-objeto-object3d)
- [Interactuando con un Object3D](#interactuando-con-un-object3d)
- [Scripting sobre objetos](#scripting-sobre-objetos)
- [Objetos derivados de Object3D](#objetos-derivados-de-object3d)
- [Tipos básicos auxiliares](#tipos-básicos-auxiliares)

---

### El objeto Object3D

El objeto más básico en Brakeza3D es el `Object3D`. Cualquier elemento en la escena, será `Object3D`.

Las propiedades principales de un `Object3D` son:

- Posición (`Vertex3D`): Posición en el mundo del objeto.
- Rotación (`M3`): Rotacion del objeto.
- Escala (`float`): Factor de escala del modelo.
- Activado (`bool`): Determina si el objeto está activado o desactivado.

---

### Interactuando con un Object3D

Podemos interactuar un Object3D.

- `getLabel()`: Obtenemos el nombre del objeto.
- `setLabel()`: Establecemos un nuevo nombre de objeto.
- `getPosition()`: Obtenemos la posición del objeto en formato `Vertex3D`.
- `setPosition(Vertex3D)`: Establecemos una nueva posición para el objeto.
- `addToPosition(Vertex3D)`: Suma un Vertex3D a la posición actual.
- `getRotation()`: Obtenemos la rotación del objeto en formato `M3`.
- `setRotation(M3)`: Establecemos una nueva rotación para el objeto.
- `getScale()`: Obtenemos el factor de escala del modelo.
- `setScale(float)`: Establecemos un nuevo factor de escala.
- `setEnable(bool)`: Activa o desactiva el objeto en la escena.
- `setRemoved(bool)`: Elimina en el siguiente frame el objeto de la escena.
- `AxisForward()`: Devuelve el actual eje de rotación hacia el frente.
- `AxisUp()`: Devuelve el actual eje de rotación hacia arriba.
- `AxisRight()`: Devuelve el actual eje de rotación hacia la derecha.
- `getTypeObject()`: Devuelve un string con el tipo de objeto.
- `lookAt(Object3D)`: Rota el objeto hacia otro objeto dado.
- `getM3ModelMatrix()`: Obtiene la matrix de objeto.
---

### Scripting sobre objetos

Cualquier `Object3D` puede ser vinculado a uno o varios scripts LUA. Veremos esto en profundidad en el apartado dedicado
al scripting.

---

### Objetos derivados de Object3D

Brakeza3D incluye multitud de objetos 3D listos para utilizarse. Todos extienden de
`Object3D`.

Por ejemplo tenemos objetos que pueden cargar modelos 3D, otros nos permitirán cargar una imagen en pantalla,
otros cargarán sprites animados, etc. A continuación enumero una lista de algunos de los más destacados:

- `Mesh3D`: Modelo 3D con triángulos y texturas
- `Mesh3DAnimation`: Similar a Mesh3D, pero para modelos animados.
- `Image2D`: Dibuja una imagen en pantalla.
- `Image3D`: Plano 3D sobre el que se visualiza una textura de imagen.
- `Image2DAnimation`: Conjunto de `Image2D` que conforma una animación.
- `ParticleEmitter`: Emisor de partículas.
- `LightPoint`: Punto de luz.
- ...

A lo largo de la documentación nos detendremos en algunos de estos objetos para su mayor
comprensión y detalle.

---

### Tipos básicos auxiliares

Existen además varios tipos de dato que facilitan la implementación de tareas en 3D y estarán
disponibles a través de tus scripts LUA.

- [Vertex3D](#vertex3d)
- [M3](#m3-matriz-3x3)
- [AABB3D](#aabb3d-axis-aligned-bounding-box)
- [Point2D](#point2d)
- [Color](#color)

---
### Vertex3D

Un punto en espacio o un vector de origen centro de coordenadas. 

| Propiedad | Tipo  | Descripción  |
|-----------|-------|--------------|
| x         | float | Coordenada X |
| y         | float | Coordenada Y |
| z         | float | Coordenada Z |

| Método                         | Descripción                          |
|--------------------------------|--------------------------------------|
| `+ Vertex3D => Vertex3D`       | Suma de vectores (`add`)             |
| `- Vertex3D => Vertex3D`       | Resta de vectores (`sub`)            |
| `* Vertex3D => float`          | Producto cruz (`cross product`)      |
| `% Vertex3D => Vertex3D`       | Producto punto (`dot product`)       |
| `getNormalize() => Vertex3D`   | Normaliza el vector                  |
| `getModule() => float`         | Devuelve la longitud del vector      |
| `getInverse() => Vertex3D`     | Devuelve el vector invertido         |
| `getScaled(float) => Vertex3D` | Devuelve el vertice escalado         |
| `distance(Vertex3D) => float`  | Devuelve la distancia a otro vértice |

---

### M3 (Matriz 3x3)

Matrix 3x3 (floats) que representa una rotación.

| Propiedad | Tipo        | Descripción                           |
|-----------|-------------|---------------------------------------|
| float[9]  | float array | Almacena los valores de la matriz 3x3 |

| Método                                    | Descripción                                       |
|-------------------------------------------|---------------------------------------------------|
| `+ M3 => M3`                              | Suma de matrices (`add`)                          |
| `- M3 => M3`                              | Resta de matrices (`sub`)                         |
| `* M3 => M3`                              | Multiplicación de matrices (`mul`)                |
| `* Vertex3D => M3`                        | Multiplicación de matriz por vector (`Vertex3D`)  |
| `getMatrixIdentity() => M3`               | Devuelve la matriz identidad                      |
| `getTranspose() => M3`                    | Devuelve la matriz traspuesta                     |
| `X() => Vertex3D`                         | Devuelve el eje X                                 |
| `Y() => Vertex3D`                         | Devuelve el eje Y                                 |
| `Z() => Vertex3D`                         | Devuelve el eje Z                                 |  
| `getMatrixRotationForEulerAngle(p, y, r)` | Crea la matriz de rotación para los angulos dados |

---

### AABB3D (Axis-Aligned Bounding Box)

Caja contendora de geometría, alineada a los ejes de coordenadas.

| Propiedad | Tipo       | Descripción               |
|-----------|------------|---------------------------|
| min       | `Vertex3D` | Vértice mínimo de la caja |
| max       | `Vertex3D` | Vértice máximo de la caja |

| Método                            | Descripción                                       |
|-----------------------------------|---------------------------------------------------|
| `isColliding(AABB3D) => bool`     | Determina si se superpone con otro `AABB3D`       |
| `size() => Vertex3D`              | Devuelve el tamaño de la caja                     |
| `getCenter() => Vertex3D`         | Devuelve el centro de la caja                     |
| `isPointInside(Vertex3D) => bool` | Determina si un `Vertex3D` está dentro de la caja |

---

### Point2D
Punto de coordenadas en 2D.

| Propiedad | Tipo  | Descripción  |
|-----------|-------|--------------|
| x         | float | Coordenada X |
| y         | float | Coordenada Y |

---

### Color

Color formato `RGBA`

| Propiedad | Tipo  | Descripción            |
|-----------|-------|------------------------|
| r         | float | Componente roja (0-1)  |
| g         | float | Componente verde (0-1) |
| b         | float | Componente azul (0-1)  |
| a         | float | Componente alfa (0-1)  |

---

>[Back to index](#index)

# 3) Camera3D object

Brakeza3D incluye un objeto específico para referirse a la cámara: `Camera3D`. Al fin y al cabo es
un `Object3D`, por lo que podrás moverlo y rotarlo como tal. Incorpora algunas opciones de configuración como lo son 
el `FOV` y tamaños del `Frustum`, que afectarán a como y qué veámos en pantalla.

- [Matrices de vista y proyección](#matrices-de-vista-y-proyección)
- [Movimiento y rotación de la cámara](#movimiento-y-rotación-de-la-cámara)
- [ComponentCamera](#componentcamera)
- [FOV](#fov)

---

### Matrices de vista y proyección

A través del objeto `Camera3D` obtendremose la matrices de `vista` y `proyección`:

- `getM3ViewMatrix`: `M3` con los datos de la matriz de vista.
- `getM3ProjectionMatrix`: `M3` con los datos de la matriz de proyección.

### Movimiento y rotación de la cámara

La cámara es un `Objeto3D`, por lo que podremos moverlo a través del
método `setPosition(Vertex3D)` que ya hemos visto.

De igual forma podrás rotar la cámara con `setRotation(M3)`.

### ComponentCamera

Internamente Brakeza3D gestiona las operaciones de cámara a través del componente `ComponentCamera`.

Podrás obtener acceso a este componente desde tus scripts LUA y este te dará acceso a la cámara:

```lua
camera = componentsManager:getComponentCamera():getCamera()
camera:setPosition(Vertex3D.new(10, 10, 10))
```

### FOV

Puedes ajustar el FOV horizontal desde tus scripts de la siguiente manera:

```lua
camera = componentsManager:getComponentCamera():getCamera()
camera:setFOV(140)
```

---

>[Back to index](#index)

# 4) Basic 2D object types

Brakeza3D permite incorporar a la pantalla elementos `2D` o `2.5D`

- [Image2D](#image2d)
- [Image2DAnimation](#image2danimation)
- [BillboardAnimation](#billboardanimation)
- [BillboardAnimation8Directions](#billboardanimation8directions)
- [Textos en pantalla](#textos-en-pantalla)

---

### Image2D

Es un objeto `2D`. Dibuja una imagen en pantalla.

```lua
    img = Image2D.create(10, 10, "../assets/images/logo_small.png")
    img:setEnabled(true)
    img:updatePosition(300, 300)
    brakeza:addObject3D(img, 'imagen')
```

### Image2DAnimation

Dibuja una animación en pantalla, es en esencia, una colección de `Image2D`.

Está diseñado para cargar la animación desde formato **spritesheet**, es decir, una imagen con un grid, donde cada celda
es una imagen de la animación. Es por este motivo que deberemos de indicar el **ancho** y **alto** de cada frame y el **nº de frames**
en la imagen, para que la función ajuste adecuadamente el sprite.

Podrás configurar la velocidad (**fps**) a la que se muestra una animación.
```lua
    -- Image2DAnimation.create(x, y, spriteFile, width, height, numFrames, fps)
    img = Image2DAnimation.create(0, 0, "../assets/sprites/explosion_a.png", 128, 128, 15, 24)
    img:setEnabled(true)
    brakeza:addObject3D(img, 'imagen')
```

---

### BillboardAnimation

Es un objeto `2.5D`. El billboard es un plano 3D que siempre mira a cámara.

Podrás moverlo, escalarlo, rotarlo, etc.

También está diseñado para cargar la animación desde formato **spritesheet**.

Fionalmente se dibujará la animación en el plano, según la configuración indicada.

```lua
    -- BillboardAnimation.create(position, width, height, spriteFile, spriteWidth, spriteHeight, numFrames, fps)
    animation = BillboardAnimation.create(Vertex3D.new(10, 10, 10), 100, 100, "../assets/sprites/explosion_a.png", 128, 128, 15, 24)
    animation:setEnabled(true)
    brakeza:addObject3D(animation, 'my_animation')
```

El objeto `BillboardAnimation` admite la incorporación de múltiples animaciones. Podrás
seleccionar la animación deseada con el método `setAnimation`.

```lua
    BillboardAnimation.create(position, width, height, spriteFile, spriteWidth, spriteHeight, numFrames, fps)
    animation = BillboardAnimation.create(
        Vertex3D.new(10, 10, 10),
        100,
        100,
        "../assets/sprites/explosion_a.png",
        128,
        128,
        15,
        24
    )
    animation:addAnimation(
        "../assets/sprites/explosion_b.png",
        128,
        128,
        15,
        24
    )
    animation:setAnimation(1) -- Seleccionamos la segunda animacion por su índice
    brakeza:addObject3D(animation, 'my_animation')
```

---

### BillboardAnimation8Directions

Es un tipo de `Billboard animation` en el que se almacenan *ocho direcciones*.

**Según el ángulo a cámara, el billbaord cambiará su animación automáticamente.**
Su comportamiento es similar a lo que vemos en los enemigos de mítico 'Doom'.

Está diseñado para cargar las imágenes de las animaciones desde una carpeta, en la
que cada fichero debe respetar un patrón: `[direccion]_[frame].png`.

Supongamos que deseamos cargar una animación con dos frames. Estos serían los ficheros esperados:

| Direccion | Frame 0 | Frame 1 | Frame 2 |
|-----------|---------|---------|---------|
| 01        | 1_0.png | 1_1.png | 1_2.png |
| 02        | 2_0.png | 2_1.png | 2_2.png |
| 03        | 3_0.png | 3_1.png | 3_2.png |
| 04        | 4_0.png | 4_1.png | 4_2.png |
| 05        | 5_0.png | 5_1.png | 5_2.png |
| 06        | 6_0.png | 6_1.png | 6_2.png |
| 07        | 7_0.png | 7_1.png | 7_2.png |
| 08        | 8_0.png | 8_1.png | 8_2.png |

Así sucesivamente, si nuestra animación tiene más frames.

```lua
    img = BillboardAnimation8Directions.create("../assets/sprites/explosion", 15, 24)
    img:setEnabled(true)
    brakeza:addObject3D(img, 'imagen')
```

---

### Textos en pantalla

Brakeza3D permite escribir textos en pantalla a través de tus scripts LUA mediante el
objeto `TextWriter`.

Necesitarás disponer de una fuente, en formato `TTF`, crear el objeto y ya podrás utilizarlo.

Sus métodos son:

- `writeTextTTF(x, y, width, height, text, Color)`: Dibuja texto en la posición `x,y` de tamaño `widh*height`.
- `writeTextTTFAutoSize(x, y, text, Color, sizeRatio)`: Dibuja texto en la posición `x,y`, calculando dimensiones automáticamente.
- `writeTextTTFMiddleScreen(text, Color, sizeRatio)`: Dibuja texto centrado en pantalla.
- `writeTTFCenterHorizontal(y, text, Color, sizeRatio)`: Dibuja texto centrado horizontalmente.

```lua
function onStart()
    textWriter = TextWriter.create("../assets/fonts/Courier.ttf")
end

function onUpdate()
     textWriter:writeTTFCenterHorizontal(
        15,
        "Centrado horizontal!",
        Color.new(0, 1, 0, 1),
        1.5
    )

    textWriter:writeTextTTF(100, 100, 100, 100, "Hola!", Color.new(1, 0, 0, 1))
end

```

---

>[Back to index](#index)

# 5) Basic 3D object types


- [Image3D](#image3d)
- [Mesh3D](#mesh3d)
- [Mesh3DAnimation](#mesh3danimation)
- [Dibujar líneas](#dibujar-lineas)

---

### Image3D

Un plano con la textura de una imagen.

```lua
-- Image3D.create(position, width, height, imageFile)
image3d = Image3D.create(Vertex3D.new(10, 10, 10), 10, 10, "../assets/sprites/explosion_a.png")
image3d:setEnabled(true)
brakeza:addObject3D(image3d, 'my_image3d')
```
---

### Mesh3D

Modelo 3D, incorpora geometría y texturas.

```lua
-- Mesh3D.create(position, modelFile)
eye = Mesh3D.create(Vertex3D.new(0, 0, 10), "../assets/models/eye.fbx")
eye:setEnabled(true)
eye:setScale(10)
brakeza:addObject3D(eye, 'modelo')
```

Los objetos ``Mesh3D`` disponen de algunas características especiales, como los `Grid3D` o
los `Octrees`, que veremos en el capítulo dedicado a scripting.

---

### Mesh3DAnimation

Modelo 3D animado.

```lua
-- Mesh3DAnimation.create(position, animatedModelFile)
man = Mesh3DAnimation.create(Vertex3D.new(0, -10, 40), "../assets/animations/walking.fbx")
man:setBelongToScene(false)
man:setScale(0.01)
brakeza:addObject3D(man, 'myMeshAnimated')
```

Si el modelo tuviese más de una animación, la primera de ellas será la seleccionada por
defecto. Desde la GUI podrás seleccionar cualquier otra animación como activa.
Si deseas hacerlo desde tus scripts LUA:

```lua
man = Mesh3DAnimation.create(Vertex3D.new(0, -10, 40), "../assets/animations/walking.fbx")
man:setIndexCurrentAnimation(2) -- animacion nº3 seleccionada
brakeza:addObject3D(man, 'myMeshAnimated')
```

Si deseas cambiar la velocidad de la animación, podrás hacerlo también desde la UI o desde tus
scripts LUA:

```lua
man = Mesh3DAnimation.create(Vertex3D.new(0, -10, 40), "../assets/animations/walking.fbx")
man:setAnimationSpeed(0.5) -- rango 0.0 - 1.0
brakeza:addObject3D(man, 'myMeshAnimated')
```

---

### Dibujar lineas

Puedes dibujar líneas 3D a través del `componentRender`

```lua
local render = componentsManager:getComponentRender();
from = this:getPosition()
to = this:getPosition() + this:AxisUp():getScaled(2)
render:drawLine(from, to)
```

---

>[Back to index](#index)

# 6) Collision between objects

Los objetos colisionables permitirán al programador implementar lógica en función de las colisiones
producidas. En Brakeza3D todos los `Object3D` puede trabajar con físicas y colisiones.

Cualquier objeto colisionables lanzará una llamada al método `onCollision` de tus scripts LUA por cada colisión producida.

```lua
function onCollision(with)
    print("Collision with " .. with:getLabel())
end
```

- [Modos de colisión](#modos-de-colisión)
- [Forma del colisionador](#forma-del-colisionador)
- [Desactivar colisiones](#desactivar-colisiones)
- [Modo de colisión KINEMATIC](#modo-de-colisión-kinematic)

---

### Modos de colisión

Existen dos modos de funcionamiento para el sistema de colisiones:

- `GHOST`: Objeto colisionable no reactivo a físicas.
- `RIGIDBODY`: Objeto colisionable reactivo a físicas.

Puedes configurar el modo de las colisiones de un objeto desde la GUI o desde tus scripts LUA.

Para hacerlo mediante tus scripts puedes utilizar el método ``setupGhostCollider`` o
``setupRigidBodyCollider``.

```lua
eye = Mesh3D.create(Vertex3D.new(0, 0, 10), "../assets/models/eye.fbx")
eye:setCollisionsEnabled(true)
eye:setupGhostCollider(CollisionShape.SIMPLE_SHAPE); -- Ghost
brakeza:addObject3D(eye, 'myOneEye')

eye2 = Mesh3D.create(Vertex3D.new(0, 0, 10), "../assets/models/eye.fbx")
eye2:setCollisionsEnabled(true)
eye2:setupRigidBodyCollider(CollisionShape.SIMPLE_SHAPE); -- RigidBody
brakeza:addObject3D(eye2, 'myTwoEye')
```

En modo de colisión ``RIGIDBODY`` podemos indicarle si la geometría será **estática**. Los elementos estáticos no
se moverán (su masa es automáticamente 0). Es el único modo de colisión permitido para geometrías no convexas.

```lua
eye2 = Mesh3D.create(Vertex3D.new(0, 0, 10), "../assets/models/eye.fbx")
eye2:setColliderStatic(true) -- estático!
eye2:setupRigidBodyCollider(CollisionShape.SIMPLE_SHAPE); -- RigidBody
brakeza:addObject3D(eye2, 'myTwoEye')
```

### Forma del colisionador

Cualquier ``Object3D`` puede trabajar con la forma de colisionador denominada `CollisionShape.SIMPLE_SHAPE`.
**Es el modo por defecto**. La forma de colisión será un paralelogramo de un tamaño configurable.
Su performance es muy buena.

No obstante, todos aquellos objetos que dependen de `Mesh3D` podrán crearse con una forma de colisionador
denominada `CollisionShape.TRIANGLE3D_MESH_SHAPE` creando una una malla de colisión exacta a la geometría del modelo. Es más precisa, pero conlleva un mayor costo
computacional.

Puedes manipular los modos de colisión tanto desde la GUI como desde tus scripts LUA.

```lua
eye = Mesh3D.create(Vertex3D.new(0, 0, 10), "../assets/models/eye.fbx")
eye:setCollisionsEnabled(true)
eye:setupGhostCollider(CollisionShape.TRIANGLE3D_MESH_SHAPE); -- Ghost
brakeza:addObject3D(image3d, 'myOneEye')
```

---

### Desactivar colisiones

Puedes desactivar las colisiones para un objeto con el método ``setCollisionsEnabled``. Esto eliminará cualquier forma
de colisión configurada previamente.

---

### Movimiento en objetos colisionadores

Tanto los ``GHOST`` como los ``RIGIDBODY`` pueden ser movidos por la escena.

- `setLinearVelocity(Vertex3D)`

### Aplicar fuerzas a objetos rigidos

Podemos aplicar fuerzas a los objetos ``RIGIDBODY`` gracias a los siguientes métodos:

- `applyCentralImpulse(Vertex3D f)`
- `applyCentralForce(Vertex3D f)`
- `applyImpulse(Vertex3D f, Vertex3D r)`

### Modo de colisión KINEMATIC


---

> [Back to index](#index)

# 7) Light objects types

Todos los tipos de luz son ``Object3D``, por lo que su movimiento y rotación es similar a cualquier otro objeto.

- [LightPoint3D](#lightpoint3d)
- [SpotLight3D](#spotlight3d)

---

### LightPoint3D

Punto de luz que emite en todas las direcciones.

Un punto de luz dispone de algunos atributos que afectarán como la luz funciona:

#### Componentes ADS

- `Componente ambiente (vec3)`: Intensidad de color de la luz ambiente sobre el objeto.
- `Componente difuso (vec)`: Intensidad de color de la luz sobre la textura.
- `Componente especular (dev)`: Intensidad de color de reflejos especulares.

Podrás manipular estos atributos desde la GUI o desde tus scripts LUA. Para hacerlo desde tus scripts,
podrás usar los métodos:

- `setAmbient(Color)`
- `setColor(Color)`
- `setColorSpecular(Color)`

#### Constantes

- `Constant (float)`
- `Constante Linear (float)`
- `Constante Quadratic (float)`

También podrás manipular estos atributos desde la GUI o desde tus scripts LUA. Para hacerlo desde tus scripts,
podrás usar los métodos:

- `setConstant(float)`
- `setLinear(float)`
- `setCuadratic(float)`

Ejemplo de instanciación desde script LUA:

```lua
-- LightPoint3D.create(position)
lightp = LightPoint3D.create(Vertex3D.new(5, 5, 5))
brakeza:addObject3D(lightp, 'myLightPoint')
```

---

### SpotLight3D

Punto de luz que emite en una dirección dada. Es básicamente un *foco* de luz.

El objeto `SpotLight3D` hereda directamente de `LightPoint3D`, por lo que posee todas
sus propiedades, sin embargo, añade algunas importantes:

- `direction`: `Vertex3D` con la dirección en la que apunta el foco
- `cut`: `float` Constante de tamaño (tamaño del foco)
- `cutOff`: `float` Constante de corte de luz (difusión del borde del foco)

Podrás manipular dichos valores mediante los métodos:

- `setDirection(Vertex3D)`
- `setCut(float)`
- `setCutOff(float)`

Ejemplo de instanciación desde script LUA:

```lua
-- SpotLight3D.create(position, direction)
lights = SpotLight3D.create(Vertex3D.new(5, -10, 5), Vertex3D.new(0, 0, 1))
lights:setColor(Color.new(255, 255, 0, 255))
brakeza:addObject3D(lights, 'mySpotLight')
```

---

> [Back to index](#index)

# 8) Particle emitter

Brakeza3D incorpora un objeto dedicado a emitir partículas, nos referimos a `ParticleEmitter`.

Es un emisor de partículas emite imagenes 2D según la configuración dada.

Para configurar un `ParticleEmitter` debes configurar una serie de parámetros:

- `Posición`: Posición 3D del emisor de partículas.
- `Tiempo de vida`: TTL del emisor de partículas en segundos.
- `Color inicio`: (Opcional). Color inicial de la partícula al nacer.
- `Color final`: (Opcional). Color final de la partícula al morir.
- `Contexto`: Contexto del emisor de partículas.
- `Imagen`: Fichero de imagen.

El argumento más destacable es el `contexto`. El `contexto` incluye multitud de parámetros
que podremos manipular para alterar el comportamiento de nuestras partículas. Los contextos
de emisores de partículas se implementan a través de un objeto: ``ParticlesContext``:

Las propiedades de un ``ParticlesContext`` son:

- `gravity (float)`: Factor gravitatorio de las partículas.
- `particlesByFrame (float)`: Número de partículas creadas por segundo
- `particleLifespan (float)`: Tiempo de vida de cada partícula.
- `angleRange (int)`: Ángulo del cono de emisión de partículas.
- `minVelocity (int)`: Velocidad mínima de la partícula al nacer.
- `maxVelocity (int)`: Velocidad máxima de la particula al ser nacer.
- `alphaMin (int)`: Canal alpha mínimo de la partícula al nacer.
- `alphaMax (int)`: Canal alpha máximo de la partícula al nacer.
- `positionNoise (int)`: Ruido en la posición de nacimiento de la partícula.
- `velocityNoise (int)`: Ruido en el movimiento de la partícula.
- `decelerationFactor (float)`: Factor de deceleración de la partícula.

Podrás manipular las propiedades de los ``ParticlesContext`` desde la UI.

Ejempl de creación de un emisor de partículas desde código:

```lua
print("Load ParticleEmitter")
particles = ParticleEmitter.create(
    Vertex3D.new(10, 10, 10),       -- position
    100,                            -- ttl
    Color.new(255, 255, 0, 255),    -- color from
    Color.new(255, 255, 255, 255),  -- color to
    ParticlesContext.new(
        0.0,            -- gravity
        2,              -- particlesByFrame              
        1.0,            -- particleLifespan
        25.0,           -- angleRange
        1,              -- minVelocity
        10,             -- maxVelocity
        125.0,          -- alphaMin
        255.0,          -- alphaMax
        5,              -- positionNoise
        10,             -- velocityNoise
        0.99            -- decelerationFactor
    ),
    "../assets/images/logo_small.png"       --image
)
```

---

> [Back to index](#index)

# 9) Input system

---

>[Back to index](#index)

# 10) Global illumination

Brakeza3D incluye una `luz por defecto` en la escena. Puedes verlo como *el sol*,

Al fin y al cabo, es una luz direccional, por lo que podrás ajustar algunos de sus
parámetros para conseguir la luz base deseada.

Al ser una luz global, **carece de posición**.

Puedes manipular las componentes de la iluminación global mediante la UI o a través
de tus scripts LUA.

Se configura mediante el `componente render`:

```lua
    render = componentsManager:getComponentRender()
```

- `setGlobalIlluminationDirection(Vertex3D)`: `Dirección` de iluminación global.
- `setGlobalIlluminationAmbient(Vertex3D)`: Componente `ambiente` de iluminación global.
- `setGlobalIlluminationDiffuse(Vertex3D)`: Componente `difuso` de iluminación global.
- `setGlobalIlluminationSpecular(Vertex3D)`: Componente `especuilar` de de iluminación global.

`Nota`: Las componentes ambiente, difusa y especular, las manipulamos a través del tipo `Vertex3D` y no a través de colores,
simplemente por conveniencia, ya que se esperan valores de rango `0-1` y no `0-255` como hace un `Color`.

Ejemplo para configurar la componente `ambiente`:

```lua
render = componentsManager:getComponentRender()
render:setGlobalIlluminationAmbient(Vertex3D.new(1, 0, 0)) -- rojo
```

---

>[Back to index](https://github.com/rzeronte/brakeza3d/blob/master/doc/00-index.md)

# 11) Sound system

Brakeza3D ofrece una interfaz básica para el manejo del sonido en tus juegos. Podemos acceder a
las funcionalidades de este sistema a través de ``ComponentSound``.

```lua
...
systemSound = componentsManager:getComponentSound()
...
```
 
- [Formatos de sonido](#formatos-de-sonido)
- [Canales](#canales)
- [Carga de ficheros en memoria](#carga-de-ficheros-en-memoria)
- [Reproducior música](#reproducir-música)
- [Reproducir sonidos](#reproducir-sonidos)
- [Carga automática](#carga-automática)

---

### Formatos de sonido

Podrás reproducir ficheros ``WAV`` y/o `MP3`.

---

### Canales

Brakeza3D maneja el sonido através de la librería `SDL_mixer`, el cual soporta 16 canales de audio.

Si bien la gestión de estos canales es automática cuando trabajemos desde nuestros scripts, Brakeza3D asigna uno
exclusivamente para la ``música``. El resto queda disponible para la emisión de `sonidos`.

Esta separación nos permitirá manejar los volumenes de la música y los sonidos por separado.

---
### Carga de ficheros en memoria

No es recomendable `cargar de disco`` un sonido en tiempo real justo en el momento que necesite ser reproducido, es por
este motivo por el que se recomienda pre-cargar los sonidos que posteriormente vayamos a utilizar.

El ``ComponenteSonido`` nos ofrece la posibilidad de cargar un sonido a memoria, el cual podrá ser reproducido tantas veces
como sea necesario.

---
### Reproducir música

Precarga de un fichero destinado a reproducirse como `música`. La música se reproduce en búcle de forma
automática.

```lua
...
systemSound = componentsManager:getComponentSound()
systemSound:addMusic("../assets/sounds/music_demo.mp3", "music_demo")
...
```
---

Reproducir una música precargada:

```lua
...
systemSound = componentsManager:getComponentSound()
systemSound:playMusic("music_demo")
...
```
---
### Reproducir sonidos

Precarga de un fichero destinado a reproducirse como `sonido`:

```lua
...
systemSound = componentsManager:getComponentSound()
systemSound:addSound("../assets/sounds/music_demo.mp3", "sound_demo")
...
```

---

Reproducir una sonido precargado:

```lua
...
systemSound = componentsManager:getComponentSound()
systemSound:playSound("sound_demo")
...
```

---

### Volumen

Puedes manipular el volumen de la música y el resto de sonidos de forma separada.

Manipular el volumen de la música:

```lua
...
systemSound = componentsManager:getComponentSound()
systemSound:setMusicVolume(75) -- Rango [0 - 128]
...
```

Manipular el volumen de los sonidos:

```lua
...
systemSound = componentsManager:getComponentSound()
systemSound:setSoundsVolume(100) -- Rango [0 - 128]
...
```

---

### Carga automática

Brakeza3D ofrece un sistema de pre-carga inicial de sonidos y música a través de
un fichero de configuración JSON.

Podrás encontrar este fichero en ``config/sounds.json`` .

````json
{
  "sounds": [
    { "file": "radio_beep", "label": "musicTest", "type": "music" },
    { "file": "radio_beep.wav", "label": "soundTest", "type": "sound" }
  ]
}
````

Automáticamente podrás acceder a los sonidos y músicas definidos en este fichero.

---

> [Back to index](https://github.com/rzeronte/brakeza3d/blob/master/doc/00-index.md)

# 12) Scripting system

Brakeza3D incorpora un sistema de scripting utilizando LUA como lenguaje.

Ya sea desde la UI o desde código, podrás asociar ``scripts`` al ``proyecto``,
``escena`` y/o ``objetos``.

---

- [Estados del sistema de scripting](#estados-del-sistema-de-scripting)
- [Ciclo de vida de un objeto](#ciclo-de-vida-de-un-objeto)
- [Scripts LUA](#scripts-lua)
- [Vinculación de scripts](#vinculación-de-scripts)
- [Variables](#variables)
- [Variables globales](#variables-globales)
- [Variables locales](#variables-locales)
- [Gestión de escenas](#gestión-de-escenas)
- [DeltaTime](#deltatime)
- [Terminar la ejecución](#terminar-la-ejecución)
- [Autocargar proyectos o escenas](#autocargar-proyectos-o-escenas)
- [Ejemplos en código](#ejemplos-en-codigo)

---

### Estados del sistema de scripting

En el búcle principal de Brakeza3D, se ejecutan una serie de acciones constantemente. Una de ellas
es el sistema de scripting. Este sistema puede estar en ON/OFF/PAUSE.

Si está en ON, los objetos ejecutarán su ciclo de vida implementado en los scripts.

El sistema de scripting puede ser ejecutado (PLAY), para iniciar la ejecución de los scripts.

Finalmente, podrá ser detenido (STOP) para evitar que el sistema de scripting continue.

También podrás resetear (RELOAD) el estado para volver al inicio los scripts y para refrescarlos desde disco.

---

### Ciclo de vida de un objeto

Los objetos disponen de su propio ciclo de vida el cual debemos de entender para trabajar con nuestros objetos cargados, es el siguiente:

- **onStart**: Instante en que se inicia la ejecución. Cuando activamos el sistema de scripting (PLAY)

- **onUpdate**: Instante actual, es decir, cada frame.

- **onEnd**: Instante en el que se detiene la ejecución (STOP)

Una template básica de script LUA para Brakeza3D sería:

```lua
function onStart()
    -- código a ejecutar al inicio del sprint, una única vez.
end

function onUpdate()
    -- código a ejecutar en cada frame.
end
```
---

### Scripts LUA

Los scripts de LUA son elementos que podremos vincular a elementos del sistema. En ellos implementaremos la lógica y comportamiento de objetos en Brakeza3D.

Podemos diferenciar dos grandes tipos de scripts:

- **Scripts de Objeto**: Se asocian a **objetos**. Un mismo script puede vincularse a multitud de objetos.
- **Scripts globales**: Se asocian a la **escena** o al **proyecto**, no a ningún objeto específico, son de caracter general.

Se diferencian principalmente por el **scope de sus variables**.

Los scripts globales comparten variables libremente entre ellos, mientras que los scripts de objeto instancian sus variables por cada objeto al
que han sido vinculados.

---

### Vinculación de scripts

Podrás realizar estas operaciones desde la UI con el ratón y drag/drop, sin embargo,
en ocasiones necesitarás poder hacerlo dinámicamente desde código.

Disponemos de un objeto ``ScriptLUA`` que encapsula la lógica de carga de un script.


Los `Object3D` disponen de un método `attachScript` que nos permitirá vincularlos:

```lua
print("Load LightPoint3D")
lightp = LightPoint3D.create(Vertex3D.new(5, -10, 5))
brakeza:addObject3D(lightp, 'myLightPoint')

script = ScriptLUA.create("../../scripts/MoveForwardObject.lua")
if script ~= nil then
    lightp:attachScript(script)
end
```

De igual forma el ``ComponentScripting`` dispone de ``addSceneLUAScript`` que te permitirá vincular
un script a la escena:

```lua
script = ScriptLUA.create("../../scripts/global_script.lua")
if script ~= nil then
    componentsManager:getComponentScripting():addSceneLUAScript(script)
end
```

Es importante destacar, que *la única* forma de vincular scripts al ``proyecto`` es mediante la UI, no desde código.

---

### Variables

Cualquier script LUA puede definir variables que te ayudarán a implementar tu lógica. Puedes utilizar la GUI
para gestionar con facilidad las variables de un script.

Físicamente se almacenan en un fichero ``JSON`` del mismo nombre que el script.

```json
{
	"name":	"global_script_example.lua",
	"types": [
        {
          "name": "var1",
          "type": "string",
          "value": "hello my friend!"
        },
        {
          "name": "var2",
          "type": "int",
          "value": 10
        },
        {
          "name": "var3",
          "type": "float",
          "value": 0.3
        },
        {
          "name": "var4",
          "type": "Vertex3D",
          "value": {
            "x": 0,
            "y": 2,
            "z": 0
          }
        }
    ]
}
```
Puedes utilizar los tipos `int`, `float`, `string` y `Vertex3D`.

---

### Variables globales

Las variables definidas en scripts vinculadas a ``proyectos`` y/o `escenas` serán globales.

Podrás acceder directamente a las variables globales desde cualquier otro
script.

```lua
function onUpdate()
    var1 = var1 .. "!" -- ejemplo de variable global
    print("Value of var1: " .. var1)
end

```

---

### Variables locales

Las variables definidas en scripts vinculadas a ``Object3D`` serán locales,
es decir, se instancian individualmente por cada objeto.

Puedes acceder a las variables locales de otro objeto mediante tus scripts LUA
de la siguiente forma:

```lua
o = brakeza:getSceneObjectByLabel("MyObject")
position = o:getLocalScriptVar("offset") -- obtenemos un vertex3D
print("Read variable 'offset' from object: ".. o:getLabel())
print("Value for 'offset': " .. position.x .. ", " .. position.y .. ", " .. position.z)

print("Read variable 'count' from object: ".. o:getLabel())
print("Value for 'count': " .. o:getLocalScriptVar("count")) -- obtenemos int
```

---

### Gestión de escenas

Puedes cargar y salvar escenas tanto desde GUI como desde tus scripts LUA

```lua
function onStart()
    ...
    componentsManager:getComponentRender():getSceneLoader():loadScene("../scenes/scene_example.json")
    ...
    componentsManager:getComponentRender():getSceneLoader():saveScene("../scenes/scene_example.json")
    ...
end
```

---

### Deltatime

El `DeltaTime` es el tiempo que tarda en renderizarse un frame. Es una medida crucial en el desarrollo de juegos,
ya que permite que los movimientos y animaciones sean consistentes independientemente de la velocidad de renderización.

Puedes obtenerlo desde tus scripts LUAde la siguiente manera:

```lua
...
print("DeltaTime: " .. brakeza:getDeltaTime()) -- delta en segundos
print("DeltaTimeInMicro: " .. brakeza:getDeltaTimeMicro()) -- delta en microsegundos
print("Execution Time: " .. brakeza:getExecutionTime()) -- tiempo total de ejecución
...  
function onUpdate()
    local speed = 5.0  -- unidades por segundo
    local movement = speed * brakeza:getDeltaTime()
    myObject:addToPosition(Vertex3D.new(movement, 0, 0))
end
```

---

### Terminar la ejecución

Si deseas terminar la aplicación desde código, puedes hacerlo de la siguiente forma:

```lua
 brakeza:finish()
```

---

### Autocargar proyectos o escenas

Cuando desees empaquetar tu juego o aplicación, desearás que Brakeza3D carge y ejecute automáticamente los scripts
de algún `proyecto` en particular. Podrás indicar esto a Brakeza3D desde la línea de comandos.

```bash
brakeza3d.exe -p MyProject.json
```
o
```bash
brakeza3d.exe --project MyProject.json
```
Con esto el proyecto será ejecutado automáticamente sin UI.

`Nota`: La ruta en la que será buscado el fichero de proyecto es relativa al directorio base de
proyectos: `/assets/projects/`.

---

### Grid3D y Octree

Brakeza3D incorpora las estructuras de datos `Grid3D` y `Octree` integradas en los objetos de tipo
``Mesh3D``.

- `Grid3D`: Crea una rejilla de `X`, `Y`, `Z` dimensiones sobre el AABB del objeto dado.
- `Octree`: Crea un árbol octal, con la profundidad (`maxDepth`) indicada.

Para cargar una rejilla en un objeto `Mesh3D` puedes utilizar el método `buildGrid3D(sizeX, sizeY, sizeZ)`

```lua
eye = Mesh3D.create(Vertex3D.new(0, 0, 10), "../assets/models/eye.fbx")
eye:setScale(10)
eye:buildGrid3D(5, 5, ) -- crea una rejilla de 5x5x5
brakeza:addObject3D(eye, 'modelo')
```

Para cargar una *octree* en un objeto `Mesh3D` puedes utilizar el método `buildOctree(maxDepth)`.
```lua
eye = Mesh3D.create(Vertex3D.new(0, 0, 10), "../assets/models/eye.fbx")
eye:setScale(10)
eye:buildOctree(1) -- crea una árbol de octanos de un solo nivel de profundidad, solo 8 hijos
brakeza:addObject3D(eye, 'modelo')
```
Para volver a ajustar las dimensiones tanto en las rejillas como en los árboles de octanos, simplemente
vuelve a ejecutar ``buildGrid3D`` o ``buildOctree``.

#### Llenado de geometría en Grid3D

Los ``Grid3D`` almacenan un *flag boolean* por cada *celda* que podremos usar a nuestro antojo.
No obstante, Brakeza3D incorpora un mecanismo de relleno de este *flag* llamado `fillGrid3DFromGeometry`,
el cual activará este flag en cada celda, si ningún triángulo atraviesa o está contenido por dicha celda.

```lua
eye = Mesh3D.create(Vertex3D.new(0, 0, 10), "../assets/models/eye.fbx")
eye:setScale(10)
eye:buildGrid3D(5, 5, 5)
eye:fillGrid3DFromGeometry() -- activa los flags para celdas sin geometría
brakeza:addObject3D(eye, 'modelo')
```

Será especialmente útil para aplicar técnicas de ``pathfinding`` disponer de esta información precargada
en un `Grid3D`.

#### Pathfinding en Grid3D

Las rejillas incorporan un algoritmo ``A*`` que permite iterar sobre sus celdas. En combinación con `fillGrid3DFromGeometry`,
podrás conseguir que los caminos esquiven nodos con geometría.

```lua
eye = Mesh3D.create(Vertex3D.new(0, 0, 10), "../assets/models/eye.fbx")
eye:buildGrid3D(5, 5, 5)                    -- construimos un grid de 5x5x5
eye:fillGrid3DFromGeometry()                -- Flags basados en si hay o no geometría en cada celda
brakeza:addObject3D(eye, 'modelo')

eye:getGrid3D():setTravel(0, 0, 0, 5, 5, 5) -- Configuramos un viaje desde (0, 0, 0) hasta (5, 5, 5)
path = eye:getGrid3D():makeTravelCubesGrid() -- Obtenemos un array de CubeGrid3D
```

Mediante el método `setTravel(x1, y1, z1, x2, y2, z2)`, podremos ajustar el viaje para el próximo camino
solicitado mediante `makeTravelCubesGrid`.

El método `makeTravelCubesGrid` devuelve ell camino solicitado si este fué posible mediante un array de `CubeGrid3D`,
cuya estructura es la siguiente:

```C
struct CubeGrid3D {
    AABB3D box;             // Caja contenedora 
    int posX;               // Indice de la posición X en el grid
    int posY;               // Indice de la posición Y en el grid
    int posZ;               // Indice de la posición Z en el grid
    bool passed = true;     // Flag 
};
```
Podemos iterar sobre dicho array para obtener el camino deseado.

```lua
...
path = eye:getGrid3D():makeTravelCubesGrid() -- Obtenemos un array de CubeGrid3D
-- Iteramos sobre el array path y pintamos los índices de cada CubeGrid3D
for i, cube in ipairs(path) do
    -- Imprimimos las coordenadas de cada CubeGrid3D
    print("Cube " .. i .. ": X = " .. cube.posX .. ", Y = " .. cube.posY .. ", Z = " .. cube.posZ)
end
...
```
---

### Ejemplos en codigo

https://github.com/rzeronte/brakeza3d/blob/master/doc/examples-lua-code.md

---

>[Back to index](#index)

# 13) Shaders system

Brakeza3D utiliza GLSL como lenguaje de shading. En la carpeta ``GLSL`` podrás encontrar todos
los shaders existentes.

Shaders 3D:

- `Render` (*/GLSL/Render*): Dibuja un objeto en pantalla con texturas e iluminación.
- `Outliner` (*/GLSL/Outliner*): Outliner para un objeto.
- `Color` (*/GLSL/Color*): Dibuja un objeto en pantalla de un color.
- `Wireframe` (*/GLSL/Wireframe*): Dibuja los triangulos de un objeto en pantalla.
- `Points` (*/GLSL/Points*): Dibuja los vertices de un objeto en pantalla.
- `Line3D` (*/GLSL/Line3D*): Dibuja una o varias líneas 3D en pantalla.

Shaders 2D:

- `Image` (*/GLSL/Image*): Dibuja una imagen en pantalla.
- `Line` (*/GLSL/Line*): Dibuja una línea 2D en pantalla.

Shaders de post-procesamiento:

- `DeepOfField` (*/GLSL/DeepOfField*): Efecto de profundidad de campo.
- `FOG` (*/GLSL/FOG*): Efecto de niebla.
- `DepthMap` (*/GLSL/DepthMap*): Dibuja el mapa de profundidad de la escena.
- `Tint` (*/GLSL/Tint*): Colorea la pantalla de un color y un alpha dado.
- `Shading` (*/GLSL/Shading*): Renderiza un objeto a un buffer, para ser usado como máscara.
- `Particle` (*/GLSL/Particle*): Dibuja una partícula en pantalla. Usado por el objeto **ParticleEmitter**.

Puedes manipular los shaders libremente si lo consideras.

---

### Plantilla para shaders de post-procesado (WIP)

Brakeza3D te permite incorporar tus propios shaders a los objetos o a la escena de igual forma
que lo harías con un script LUA.

Puedes generar estos shaders desde la UI o simplemente crearlos tu mismo y posteriormente vincularlos
desde la GUI.

Dispones de una plantilla para shaders custom:

- `Custom` (*/GLSL/Custom*)
