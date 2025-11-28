# Index
- [1) Basics](#1-basics)
- [2) Basic Objects types](#2-basic-objects-types)
- [3) Camera3D object](#3-camera3d-object)
- [4) Basic 2D object types](#4-basic-2d-object-types)
- [5) Basic 3D object types](#5-basic-3d-object-types)
- [6) Collision between objects](#6-collision-between-objects)
- [6.5) Advanced Physics System](#65-advanced-physics-system)
- [7) Light objects types](#7-light-objects-types)
- [8) Particle emitter](#8-particle-emitter)
- [9) Input system](#9-input-system)
- [10) Global illumination](#10-global-illumination)
- [11) Sound system](#11-sound-system)
- [12) Scripting system](#12-scripting-system)
- [13) Shaders system](#13-shaders-system)
- [14) Assets folder](#14-assets-folder)
- [15) TextWriter system](#15-textwriter-system)
- [16) Scene management](#16-scene-management)

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

Entidades existentes en nuestro mundo 3D. En líneas generales, cualquier cosa susceptible
de ser representada en pantalla será un objeto: *un modelo 3D*, *una luz*, *emisor de partículas*...

Todos los objetos comparten las propiedades básicas de un elemento 3D ya sea nombre, posición, rotación, tamaño, pero cada tipo de objeto específico ampliará estas según su caso.

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
- `ComponentInput`: Gestiona la entrada por teclado, mouse o gamepad.
- `ComponentSound`: Gestiona la reproducción de sonidos y música.
- `ComponentRender`: Gestiona el render con OpenGL

Podrás acceder a todos ellos mediante él ``ComponentsManager`` desde tus scripts LUA. Trataremos
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

El objeto más básico en Brakeza3D es él `Object3D`. Cualquier elemento en la escena, será `Object3D`.

Las propiedades principales de un `Object3D` son:

- Posición (`Vertex3D`): Posición en el mundo del objeto.
- Rotación (`M3`): Rotacion del objeto.
- Escala (`float`): Factor de escala del modelo.
- Activado (`bool`): Determina si el objeto está activado o desactivado.

---

### Interactuando con un Object3D

Podemos interactuar un Object3D.

**Transformaciones básicas:**
- `getLabel()`: Obtenemos el nombre del objeto.
- `setLabel(string)`: Establecemos un nuevo nombre de objeto.
- `getPosition()`: Obtenemos la posición del objeto en formato `Vertex3D`.
- `setPosition(Vertex3D)`: Establecemos una nueva posición para el objeto.
- `addToPosition(Vertex3D)`: Suma un Vertex3D a la posición actual.
- `getRotation()`: Obtenemos la rotación del objeto en formato `M3`.
- `setRotation(M3)`: Establecemos una nueva rotación para el objeto.
- `getScale()`: Obtenemos el factor de escala del modelo.
- `setScale(float)`: Establecemos un nuevo factor de escala.
- `setDrawOffset(Vertex3D)`: Establece un offset de dibujo.

**Control de estado:**
- `setEnabled(bool)`: Activa o desactiva el objeto en la escena.
- `setRemoved(bool)`: Elimina en el siguiente frame el objeto de la escena.
- `setBelongToScene(bool)`: Indica si el objeto pertenece a la escena.

**Orientación:**
- `AxisForward()`: Devuelve el actual eje de rotación hacia el frente.
- `AxisUp()`: Devuelve el actual eje de rotación hacia arriba.
- `AxisRight()`: Devuelve el actual eje de rotación hacia la derecha.
- `lookAt(Object3D)`: Rota el objeto hacia otro objeto dado.

**Información:**
- `getTypeObject()`: Devuelve un string con el tipo de objeto.
- `getModelMatrix()`: Obtiene la matriz de modelo (glm::mat4).
- `getM3ModelMatrix()`: Obtiene la matriz de modelo (M3).

**Scripting:**
- `attachScript(string)`: Vincula un script al objeto.
- `getLocalScriptVar(string)`: Obtiene una variable local del script.
- `reloadScriptsEnvironment()`: Recarga el entorno de scripts del objeto.

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
- `BillboardAnimation` (Image3DAnimation): Billboard animado con sprite sheet.
- `BillboardAnimation8Directions` (Image3DAnimation8Directions): Billboard animado con 8 direcciones.
- `ParticleEmitter`: Emisor de partículas.
- `LightPoint3D` (LightPoint): Punto de luz.
- `SpotLight3D` (LightSpot): Luz focal/cónica.

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
- [vec2, vec3, vec4 (GLM)](#vectores-glm)

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
| `getScaled(float, float, float) => Vertex3D` | Escala en cada eje |
| `distance(Vertex3D) => float`  | Devuelve la distancia a otro vértice |

**Ejemplo:**
```lua
v1 = Vertex3D.new(1.0, 2.0, 3.0)
v2 = Vertex3D.new(4.0, 5.0, 6.0)

-- Suma de vectores
v3 = v1 + v2  -- (5.0, 7.0, 9.0)

-- Normalizar
normalized = v1:getNormalize()

-- Escalar
scaled = v1:getScaled(2.0)  -- (2.0, 4.0, 6.0)
```

---

### M3 (Matriz 3x3)

Matrix 3x3 (floats) que representa una rotación.

| Propiedad | Tipo        | Descripción                           |
|-----------|-------------|---------------------------------------|
| m         | float[3][3] | Array bidimensional de valores float  |

| Método                                       | Descripción                                    |
|----------------------------------------------|------------------------------------------------|
| `* M3 => M3`                                 | Multiplicación de matrices                     |
| `* Vertex3D => Vertex3D`                     | Multiplica matriz por vector                   |
| `getMatrixRotationForEulerAngles(x,y,z)`     | Crea matriz desde ángulos de Euler            |
| `setMatrixRotationForEulerAngles(x,y,z)`     | Establece rotación desde ángulos de Euler     |
| `getMatrixIdentity() => M3`                  | Devuelve matriz identidad                      |
| `getTranspose() => M3`                       | Devuelve la transpuesta de la matriz          |
| `X() => Vertex3D`                            | Vector columna X de la matriz                  |
| `Y() => Vertex3D`                            | Vector columna Y de la matriz                  |
| `Z() => Vertex3D`                            | Vector columna Z de la matriz                  |

**Ejemplo:**
```lua
-- Crear una rotación desde ángulos de Euler (en radianes)
rotation = M3.new()
rotation:getMatrixRotationForEulerAngles(0, math.pi / 4, 0)  -- Rotar 45° en Y

-- Aplicar rotación a un objeto
objeto:setRotation(rotation)
```

---

### AABB3D (Axis-Aligned Bounding Box)

Caja delimitadora alineada con los ejes. Útil para detección de colisiones y particionamiento espacial.

| Propiedad | Tipo     | Descripción                  |
|-----------|----------|------------------------------|
| min       | Vertex3D | Vértice mínimo de la caja    |
| max       | Vertex3D | Vértice máximo de la caja    |
| vertices  | array    | Array de 8 vértices de la caja |
| size      | Vertex3D | Tamaño de la caja (max - min) |

| Método                        | Descripción                                |
|-------------------------------|--------------------------------------------|
| `isColliding(AABB3D) => bool` | Comprueba si colisiona con otra caja      |
| `setScale(Vertex3D)`          | Escala la caja                            |
| `updateVertices()`            | Actualiza los 8 vértices de la caja       |
| `getPlanes()`                 | Obtiene los 6 planos de la caja           |
| `getCenter() => Vertex3D`     | Obtiene el centro de la caja              |
| `isPointInside(Vertex3D) => bool` | Comprueba si un punto está dentro    |

**Ejemplo:**
```lua
-- Crear AABB
aabb = AABB3D.new()
aabb.min = Vertex3D.new(-1, -1, -1)
aabb.max = Vertex3D.new(1, 1, 1)
aabb:updateVertices()

-- Comprobar si un punto está dentro
punto = Vertex3D.new(0.5, 0.5, 0.5)
if aabb:isPointInside(punto) then
    print("El punto está dentro de la caja")
end
```

---

### Point2D

Punto en espacio 2D.

| Propiedad | Tipo  | Descripción  |
|-----------|-------|--------------|
| x         | int   | Coordenada X |
| y         | int   | Coordenada Y |

---

### Color

Representa un color RGBA.

| Método                               | Descripción                     |
|--------------------------------------|---------------------------------|
| `new() => Color`                     | Constructor por defecto (negro) |
| `new(r, g, b, a) => Color`           | Constructor con valores RGBA    |
| `setRed(float)`                      | Establece componente roja       |
| `setGreen(float)`                    | Establece componente verde      |
| `setBlue(float)`                     | Establece componente azul       |

**Ejemplo:**
```lua
-- Color rojo con alpha 1.0
rojo = Color.new(1.0, 0.0, 0.0, 1.0)

-- Color verde semitransparente
verde = Color.new(0.0, 1.0, 0.0, 0.5)
```

---

### Vectores GLM

Tipos vectoriales de GLM para uso con shaders personalizados.

**vec2:**
```lua
v = vec2.new(1.0, 2.0)
print(v.x, v.y)
```

**vec3:**
```lua
v = vec3.new(1.0, 2.0, 3.0)
print(v.x, v.y, v.z)
```

**vec4:**
```lua
v = vec4.new(1.0, 2.0, 3.0, 4.0)
print(v.x, v.y, v.z, v.w)
```

Estos tipos son útiles principalmente para configurar uniforms de shaders personalizados.

---

>[Back to index](#index)

# 3) Camera3D object

La cámara es un objeto especial que determina desde qué punto de vista se renderiza la escena.

### Métodos principales

| Método                                    | Descripción                                |
|-------------------------------------------|--------------------------------------------|
| `setFOV(float)`                           | Establece el campo de visión (Field of View) |
| `getM3ViewMatrix() => M3`                 | Obtiene la matriz de vista                 |
| `getM3ProjectionMatrix() => M3`           | Obtiene la matriz de proyección            |
| `setRotationFromEulerAngles(x, y, z)`     | Establece rotación desde ángulos de Euler  |

La cámara hereda de `Object3D`, por lo que también dispone de todos los métodos de posición, rotación, etc.

### Acceso a la cámara

```lua
-- Obtener la cámara desde el ComponentsManager
componentsManager = ComponentsManager.get()
camera = componentsManager:getComponentCamera():getCamera()

-- Mover la cámara
camera:setPosition(Vertex3D.new(0, 5, 10))

-- Rotar la cámara
camera:setRotationFromEulerAngles(math.pi / 4, 0, 0)

-- Cambiar FOV
camera:setFOV(60.0)
```

---

>[Back to index](#index)

# 4) Basic 2D object types

Brakeza3D ofrece objetos 2D para renderizar imágenes en pantalla.

### Image2D

Renderiza una imagen estática en pantalla en coordenadas 2D.

**Constructor:**
```lua
image = Image2D.create(x, y, "ruta/imagen.png")
```

**Métodos:**
- `updatePosition(x, y)`: Actualiza la posición 2D de la imagen.

**Ejemplo:**
```lua
-- Crear una imagen en la posición (100, 100)
logo = Image2D.create(100, 100, "../assets/images/logo.png")
brakeza:addObject3D(logo, "logo")

-- Mover la imagen
logo:updatePosition(200, 150)
```

---

### Image2DAnimation

Sprite animado en 2D usando un sprite sheet.

**Constructor:**
```lua
anim = Image2DAnimation.create(x, y, "ruta/spritesheet.png", anchoFrame, altoFrame, numFrames, fps)
```

**Parámetros:**
- `x, y`: Posición en pantalla
- `anchoFrame`: Ancho de cada frame en el sprite sheet
- `altoFrame`: Alto de cada frame
- `numFrames`: Número total de frames
- `fps`: Frames por segundo de la animación

**Métodos:**
- `updatePosition(x, y)`: Actualiza la posición 2D.

**Ejemplo:**
```lua
-- Crear animación de 8 frames, cada uno de 32x32 píxeles, a 10 fps
personaje = Image2DAnimation.create(100, 100, "../assets/sprites/walk.png", 32, 32, 8, 10)
brakeza:addObject3D(personaje, "personaje2D")
```

---

>[Back to index](#index)

# 5) Basic 3D object types

- [Mesh3D](#mesh3d)
- [Mesh3DAnimation](#mesh3danimation)
- [Image3D](#image3d)
- [BillboardAnimation (Image3DAnimation)](#billboardanimation-image3danimation)
- [BillboardAnimation8Directions](#billboardanimation8directions-image3danimation8directions)

---

## Mesh3D

Modelo 3D estático cargado desde archivo FBX.

**Constructor:**
```lua
mesh = Mesh3D.create(posicion, "ruta/modelo.fbx")
```

**Métodos principales:**

**Carga de geometría:**
- `AssimpLoadGeometryFromFile(string)`: Carga un modelo 3D desde archivo.

**Estructuras espaciales:**
- `buildOctree(profundidad)`: Construye un octree para optimización espacial.
- `buildGrid3D(sizeX, sizeY, sizeZ)`: Construye una rejilla 3D.
- `fillGrid3DFromGeometry()`: Rellena el grid marcando celdas con geometría.
- `getOctree() => Octree`: Obtiene el octree del modelo.
- `getGrid3D() => Grid3D`: Obtiene el grid 3D del modelo.

**Ejemplo básico:**
```lua
-- Cargar modelo
castillo = Mesh3D.create(Vertex3D.new(0, 0, 0), "../assets/models/castle.fbx")
brakeza:addObject3D(castillo, "castillo")

-- Escalar
castillo:setScale(2.0)

-- Rotar
rot = M3.new()
rot:getMatrixRotationForEulerAngles(0, math.pi / 2, 0)
castillo:setRotation(rot)
```

**Ejemplo con Octree:**
```lua
modelo = Mesh3D.create(Vertex3D.new(0, 0, 0), "../assets/models/terrain.fbx")
modelo:buildOctree(5)  -- Profundidad 5
brakeza:addObject3D(modelo, "terreno")

-- Acceder al octree
octree = modelo:getOctree()
```

---

## Mesh3DAnimation

Modelo 3D animado con skeletal animation (huesos).

**Constructor:**
```lua
animMesh = Mesh3DAnimation.create(posicion, "ruta/modelo_animado.fbx")
```

**Métodos de animación:**
- `setIndexCurrentAnimation(index)`: Establece la animación por índice.
- `setAnimationByName(nombre)`: Establece la animación por nombre.
- `setAnimationSpeed(velocidad)`: Establece la velocidad de reproducción.
- `isAnimationEnds() => bool`: Comprueba si la animación ha terminado.
- `setLoop(bool)`: Establece si la animación se repite.
- `isLoop() => bool`: Comprueba si está en loop.

**Ejemplo:**
```lua
-- Cargar personaje animado
personaje = Mesh3DAnimation.create(Vertex3D.new(0, 0, 0), "../assets/animations/character.fbx")
brakeza:addObject3D(personaje, "personaje")

-- Configurar animación
personaje:setAnimationByName("Walk")
personaje:setLoop(true)
personaje:setAnimationSpeed(1.0)

-- Cambiar a otra animación cuando termine
if personaje:isAnimationEnds() then
    personaje:setAnimationByName("Idle")
end
```

---

## Image3D

Plano 3D (billboard) con una textura de imagen.

**Constructor:**
```lua
billboard = Image3D.create(posicion, ancho, alto, "ruta/imagen.png")
```

**Ejemplo:**
```lua
-- Crear árbol billboard
arbol = Image3D.create(Vertex3D.new(5, 0, 0), 2.0, 3.0, "../assets/images/tree.png")
brakeza:addObject3D(arbol, "arbol")
```

---

## BillboardAnimation (Image3DAnimation)

Billboard animado usando un sprite sheet. Siempre mira hacia la cámara.

**Constructor:**
```lua
anim = BillboardAnimation.create(
    posicion,           -- Vertex3D
    ancho,              -- float
    alto,               -- float
    "spritesheet.png",  -- string
    anchoSprite,        -- int (ancho de cada frame en el sprite)
    altoSprite,         -- int (alto de cada frame)
    numFrames,          -- int
    fps                 -- int
)
```

**Métodos:**
- `addAnimation(nombre, frameInicio, frameFin)`: Añade una animación nombrada.
- `setAnimation(nombre)`: Activa una animación por nombre.
- `updateBillboardSize(ancho, alto)`: Actualiza el tamaño del billboard.

**Ejemplo:**
```lua
-- Crear explosión animada
explosion = BillboardAnimation.create(
    Vertex3D.new(0, 2, 0),
    1.0, 1.0,
    "../assets/sprites/explosion.png",
    64, 64,  -- Cada frame es 64x64
    16,      -- 16 frames totales
    30       -- 30 fps
)
brakeza:addObject3D(explosion, "explosion")

-- Definir animaciones
explosion:addAnimation("explode", 0, 15)
explosion:setAnimation("explode")
```

---

## BillboardAnimation8Directions (Image3DAnimation8Directions)

Billboard animado con 8 direcciones. Detecta automáticamente hacia dónde mira la cámara y muestra el sprite correspondiente.

**Constructor:**
```lua
anim8dir = BillboardAnimation8Directions.create(
    posicion,           -- Vertex3D
    ancho,              -- float
    alto,               -- float
    "carpeta/sprites/", -- string (carpeta con los sprites direccionales)
    numFrames,          -- int
    fps                 -- int
)
```

**Estructura de carpeta esperada:**
La carpeta debe contener sprites nombrados con las direcciones: N, NE, E, SE, S, SW, W, NW

**Ejemplo:**
```lua
-- Crear enemigo con 8 direcciones
enemigo = BillboardAnimation8Directions.create(
    Vertex3D.new(0, 0, 0),
    1.0, 1.5,
    "../assets/sprites/enemy/",  -- Debe contener: N.png, NE.png, E.png, etc.
    8,   -- 8 frames por dirección
    10   -- 10 fps
)
brakeza:addObject3D(enemigo, "enemigo")
```

El sistema automáticamente seleccionará el sprite correcto basándose en la posición relativa entre el objeto y la cámara.

---

>[Back to index](#index)

# 6) Collision between objects

Brakeza3D integra Bullet Physics para detección de colisiones y simulación física.

### ComponentCollisions

Acceso al componente de colisiones:

```lua
componentsManager = ComponentsManager.get()
collisions = componentsManager:getComponentCollisions()
```

**Métodos:**
- `isRayCollisionWith(origen, direccion, objeto) => bool`: Lanza un rayo y comprueba colisión.
- `setEnableDebugMode(bool)`: Activa/desactiva visualización de debug de física.

**Ejemplo raycast:**
```lua
collisions = componentsManager:getComponentCollisions()

origen = Vertex3D.new(0, 10, 0)
direccion = Vertex3D.new(0, -1, 0)  -- Hacia abajo

if collisions:isRayCollisionWith(origen, direccion, objetoSuelo) then
    print("El rayo colisiona con el suelo")
end
```

### Configuración básica de colisión en objetos

Todos los `Object3D` tienen métodos básicos de colisión:

**Control de colisiones:**
- `setCollisionsEnabled(bool)`: Activa/desactiva colisiones.
- `isCollisionsEnabled() => bool`: Comprueba si están activas.
- `removeCollisionObject()`: Elimina el cuerpo de colisión.
- `sleepCollider()`: Pone el collider en modo sleep.

**Formas de colisión (CollisionShape):**
- `SIMPLE_SHAPE`: Formas simples (cápsula, caja, esfera).
- `TRIANGLE_MESH_SHAPE`: Malla de triángulos (para geometría compleja).

**Métodos de setup:**
- `setupGhostCollider(CollisionShape)`: Configura como trigger (sin física, solo detección).
- `setupRigidBodyCollider(CollisionShape)`: Configura como rigid body (con física).

**Ejemplo básico:**
```lua
-- Crear objeto con colisión de caja simple
caja = Mesh3D.create(Vertex3D.new(0, 5, 0), "../assets/models/box.fbx")
caja:setupRigidBodyCollider(SIMPLE_SHAPE)
caja:setCollisionsEnabled(true)
brakeza:addObject3D(caja, "caja")
```

Para física avanzada, ver la sección [6.5) Advanced Physics System](#65-advanced-physics-system).

---

### CollisionInfo

Información sobre una colisión detectada.

**Propiedades/Métodos:**
- `with`: Objeto con el que colisionó.
- `getSource()`: Obtiene el objeto fuente de la colisión.
- `getObject()`: Obtiene el objeto que colisionó.
- `getBoneIndexMapping() => int`: Para animaciones con huesos, devuelve el índice del hueso.

---

>[Back to index](#index)

# 6.5) Advanced Physics System

Brakeza3D integra **Bullet Physics** proporcionando un sistema completo de física para tus objetos.

- [Configuración de colliders](#configuración-de-colliders)
- [Rigid Bodies (Cuerpos rígidos)](#rigid-bodies-cuerpos-rígidos)
- [Ghost Bodies (Triggers)](#ghost-bodies-triggers)
- [Fuerzas e impulsos](#fuerzas-e-impulsos)
- [Velocidad y movimiento](#velocidad-y-movimiento)
- [Propiedades físicas](#propiedades-físicas)
- [Factores de restricción](#factores-de-restricción)
- [Character controller](#character-controller)

---

## Configuración de colliders

Todos los `Object3D` pueden tener un collider de física.

### Tipos de colliders

**1. Simple Shape (Formas primitivas):**
```lua
objeto:setupRigidBodyCollider(SIMPLE_SHAPE)
-- o
objeto:setupGhostCollider(SIMPLE_SHAPE)
```

Para objetos con forma simple, el motor usará automáticamente cápsulas, cajas o esferas según la geometría.

**2. Triangle Mesh (Malla de triángulos):**
```lua
objeto:setupRigidBodyCollider(TRIANGLE_MESH_SHAPE)
```

Para geometría compleja. Más preciso pero más costoso computacionalmente.

### Métodos de configuración

- `setupRigidBodyCollider(shape)`: Cuerpo rígido con física completa.
- `setupGhostCollider(shape)`: Trigger sin física (solo detecta colisiones).
- `setColliderStatic(bool)`: Establece el collider como estático (inmóvil).
- `UpdateShapeCollider()`: Actualiza la forma del collider.
- `setScalingCollider(Vertex3D)`: Escala el collider.
- `setCapsuleColliderSize(radio, altura)`: Configura tamaño de cápsula.
- `moveCollider(Vertex3D)`: Mueve el collider a una posición.

**Ejemplo:**
```lua
-- Suelo estático con triangle mesh
suelo = Mesh3D.create(Vertex3D.new(0, 0, 0), "../assets/models/terrain.fbx")
suelo:setupRigidBodyCollider(TRIANGLE_MESH_SHAPE)
suelo:setColliderStatic(true)
brakeza:addObject3D(suelo, "suelo")

-- Objeto dinámico con forma simple
caja = Mesh3D.create(Vertex3D.new(0, 10, 0), "../assets/models/box.fbx")
caja:setupRigidBodyCollider(SIMPLE_SHAPE)
brakeza:addObject3D(caja, "caja")
```

---

## Rigid Bodies (Cuerpos rígidos)

Los rigid bodies simulan física completa: gravedad, colisiones, inercia, etc.

### Control de simulación

- `disableSimulationCollider()`: Desactiva la simulación física.
- `enableSimulationCollider()`: Reactiva la simulación.
- `disableDeactivationCollider()`: Evita que el objeto se duerma (sleep).
- `sleepCollider()`: Pone el objeto en estado sleep.

**Ejemplo:**
```lua
-- Crear objeto físico que nunca se duerme
pelota = Mesh3D.create(Vertex3D.new(0, 20, 0), "../assets/models/ball.fbx")
pelota:setupRigidBodyCollider(SIMPLE_SHAPE)
pelota:disableDeactivationCollider()  -- Siempre activo
brakeza:addObject3D(pelota, "pelota")
```

---

## Ghost Bodies (Triggers)

Los ghost bodies detectan colisiones pero no tienen física. Útiles para triggers, zonas de detección, etc.

**Ejemplo de zona de activación:**
```lua
-- Crear trigger invisible
trigger = Mesh3D.create(Vertex3D.new(0, 0, 10), "../assets/models/box.fbx")
trigger:setupGhostCollider(SIMPLE_SHAPE)
trigger:setScale(3.0)  -- Zona grande
brakeza:addObject3D(trigger, "trigger_puerta")

-- En el script, detectar cuando el jugador entra
-- (esto se haría en el callback de colisiones)
```

---

## Fuerzas e impulsos

Aplica fuerzas para mover objetos de forma física.

### Métodos de fuerza

- `applyCentralForce(Vertex3D)`: Aplica una fuerza continua en el centro de masa.
- `applyCentralImpulse(Vertex3D)`: Aplica un impulso instantáneo en el centro.
- `applyImpulse(impulso, puntoRelativo)`: Aplica impulso en un punto específico.

**Diferencia Force vs Impulse:**
- **Force**: Se aplica continuamente (útil para motores, viento).
- **Impulse**: Se aplica una sola vez (útil para saltos, golpes).

**Ejemplo - Empujar objeto:**
```lua
-- Aplicar fuerza hacia adelante
fuerzaAdelante = Vertex3D.new(0, 0, 100)
objeto:applyCentralForce(fuerzaAdelante)

-- Aplicar impulso para saltar
impulsoSalto = Vertex3D.new(0, 500, 0)
objeto:applyCentralImpulse(impulsoSalto)
```

---

## Velocidad y movimiento

Control directo de velocidades lineales y angulares.

### Métodos de velocidad

- `setLinearVelocity(Vertex3D)`: Establece velocidad lineal.
- `getLinearVelocity() => Vertex3D`: Obtiene velocidad lineal actual.
- `setAngularVelocity(Vertex3D)`: Establece velocidad angular (rotación).

**Ejemplo - Movimiento de vehículo:**
```lua
-- Mover coche hacia adelante a 20 unidades/s
coche:setLinearVelocity(Vertex3D.new(0, 0, 20))

-- Rotar el coche
coche:setAngularVelocity(Vertex3D.new(0, 1, 0))

-- Frenar
velocidadActual = coche:getLinearVelocity()
if velocidadActual:getModule() > 0 then
    coche:setLinearVelocity(Vertex3D.new(0, 0, 0))
end
```

---

## Propiedades físicas

Control de las propiedades físicas del material.

### Métodos de propiedades

- `setMass(float)`: Establece la masa del objeto.
- `setFriction(float)`: Fricción (0.0 = sin fricción, 1.0 = alta fricción).
- `setRestitution(float)`: Rebote (0.0 = no rebota, 1.0 = rebote perfecto).
- `setGravityCollider(Vertex3D)`: Establece gravedad personalizada para este objeto.

### Damping (Amortiguación)

- `setLinearDamping(float)`: Amortiguación lineal (resistencia al movimiento).
- `setAngularDamping(float)`: Amortiguación angular (resistencia a la rotación).

**Ejemplo - Pelota que rebota:**
```lua
pelota = Mesh3D.create(Vertex3D.new(0, 10, 0), "../assets/models/ball.fbx")
pelota:setupRigidBodyCollider(SIMPLE_SHAPE)
pelota:setMass(1.0)
pelota:setRestitution(0.8)  -- Rebota bastante
pelota:setFriction(0.3)     -- Poca fricción
pelota:setLinearDamping(0.1)  -- Poca resistencia al aire
brakeza:addObject3D(pelota, "pelota")
```

**Ejemplo - Objeto pesado:**
```lua
roca = Mesh3D.create(Vertex3D.new(0, 5, 0), "../assets/models/rock.fbx")
roca:setupRigidBodyCollider(SIMPLE_SHAPE)
roca:setMass(100.0)  -- Muy pesada
roca:setFriction(0.9)  -- Mucha fricción
roca:setRestitution(0.0)  -- No rebota
brakeza:addObject3D(roca, "roca")
```

---

## Factores de restricción

Restringe movimiento en ciertos ejes.

### Métodos de factores

- `setLinearFactor(Vertex3D)`: Factor lineal (1.0 = libre, 0.0 = bloqueado) por eje.
- `setAngularFactor(Vertex3D)`: Factor angular (1.0 = libre, 0.0 = bloqueado) por eje.

**Ejemplo - Movimiento solo en plano XZ (sin Y):**
```lua
personaje = Mesh3D.create(Vertex3D.new(0, 0, 0), "../assets/models/character.fbx")
personaje:setupRigidBodyCollider(SIMPLE_SHAPE)
personaje:setLinearFactor(Vertex3D.new(1, 0, 1))  -- X y Z libres, Y bloqueado
personaje:setAngularFactor(Vertex3D.new(0, 1, 0))  -- Solo rota en Y
brakeza:addObject3D(personaje, "personaje")
```

**Ejemplo - Objeto 2D (solo XY):**
```lua
objeto2D = Mesh3D.create(Vertex3D.new(0, 0, 0), "../assets/models/sprite.fbx")
objeto2D:setupRigidBodyCollider(SIMPLE_SHAPE)
objeto2D:setLinearFactor(Vertex3D.new(1, 1, 0))  -- Solo X e Y
objeto2D:setAngularFactor(Vertex3D.new(0, 0, 1))  -- Solo rotación en Z
brakeza:addObject3D(objeto2D, "sprite")
```

---

## Character Controller

Sistema especial para control de personajes con cinemática.

### Métodos de character controller

- `setWalkingDirection(Vertex3D)`: Establece dirección de movimiento.
- `jump(Vertex3D)`: Hace saltar al personaje.
- `onGround() => bool`: Comprueba si el personaje está en el suelo.

**Ejemplo completo - Personaje jugable:**
```lua
-- Crear personaje
player = Mesh3D.create(Vertex3D.new(0, 2, 0), "../assets/models/player.fbx")
player:setupRigidBodyCollider(SIMPLE_SHAPE)
player:setMass(80.0)  -- 80 kg
player:setAngularFactor(Vertex3D.new(0, 1, 0))  -- Solo rota en Y
player:disableDeactivationCollider()  -- Siempre activo
brakeza:addObject3D(player, "player")

-- En el update del script:
function onUpdate(deltaTime)
    local input = componentsManager:getComponentInput()
    local moveSpeed = 5.0
    
    -- Dirección de movimiento
    local direction = Vertex3D.new(0, 0, 0)
    
    if input:isCharPressed('w') then
        direction = direction + player:AxisForward():getScaled(moveSpeed)
    end
    if input:isCharPressed('s') then
        direction = direction + player:AxisForward():getScaled(-moveSpeed)
    end
    if input:isCharPressed('a') then
        direction = direction + player:AxisRight():getScaled(-moveSpeed)
    end
    if input:isCharPressed('d') then
        direction = direction + player:AxisRight():getScaled(moveSpeed)
    end
    
    -- Aplicar movimiento
    player:setWalkingDirection(direction)
    
    -- Saltar
    if input:isCharPressed(' ') and player:onGround() then
        local jumpImpulse = Vertex3D.new(0, 300, 0)
        player:jump(jumpImpulse)
    end
end
```

---

**Consejo:** Para personajes complejos, combina el character controller con restricciones de factores para evitar comportamientos no deseados.

---

>[Back to index](#index)

# 7) Light objects types

Brakeza3D soporta dos tipos de luces dinámicas: **Point Lights** y **Spot Lights**.

- [LightPoint3D (Luz puntual)](#lightpoint3d-luz-puntual)
- [SpotLight3D (Luz focal/cónica)](#spotlight3d-luz-focalcónica)

---

## LightPoint3D (Luz puntual)

Luz omnidireccional que emite en todas direcciones desde un punto.

**Constructor:**
```lua
luz = LightPoint3D.create(posicion)
```

**Métodos de configuración:**

**Atenuación (cómo disminuye la luz con la distancia):**
- `setConstant(float)`: Factor constante de atenuación.
- `setLinear(float)`: Factor lineal de atenuación.
- `setCuadratic(float)`: Factor cuadrático de atenuación.

**Colores:**
- `setColor(Color)`: Color difuso de la luz.
- `setColorSpecular(Color)`: Color especular (brillos).
- `setAmbient(Color)`: Color ambiental.

**Fórmula de atenuación:**
```
attenuation = 1.0 / (constant + linear * distance + quadratic * distance²)
```

**Ejemplo - Antorcha:**
```lua
-- Crear luz naranja cálida
antorcha = LightPoint3D.create(Vertex3D.new(5, 2, 0))

-- Color naranja
antorcha:setColor(Color.new(1.0, 0.6, 0.2, 1.0))
antorcha:setColorSpecular(Color.new(1.0, 0.8, 0.4, 1.0))
antorcha:setAmbient(Color.new(0.1, 0.1, 0.05, 1.0))

-- Atenuación (alcance medio)
antorcha:setConstant(1.0)
antorcha:setLinear(0.09)
antorcha:setCuadratic(0.032)

brakeza:addObject3D(antorcha, "antorcha")
```

**Rangos de atenuación típicos:**

| Distancia | Constant | Linear | Quadratic |
|-----------|----------|--------|-----------|
| 7         | 1.0      | 0.7    | 1.8       |
| 13        | 1.0      | 0.35   | 0.44      |
| 20        | 1.0      | 0.22   | 0.20      |
| 32        | 1.0      | 0.14   | 0.07      |
| 50        | 1.0      | 0.09   | 0.032     |
| 100       | 1.0      | 0.045  | 0.0075    |
| 200       | 1.0      | 0.022  | 0.0019    |

---

## SpotLight3D (Luz focal/cónica)

Luz direccional en forma de cono. Hereda de `LightPoint3D`, por lo que tiene todos sus métodos más específicos.

**Constructor:**
```lua
spotlight = SpotLight3D.create(posicion, direccion)
```

**Métodos adicionales:**
- `setDirection(Vertex3D)`: Establece la dirección del foco.
- `setCutOff(float)`: Ángulo interno del cono (en radianes).
- `setOuterCutOff(float)`: Ángulo externo del cono (suavizado).

**Ángulos de cono:**
- `cutOff`: Límite del cono donde la luz es máxima
- `outerCutOff`: Límite donde la luz llega a 0 (transición suave)

**Ejemplo - Linterna:**
```lua
-- Crear linterna apuntando hacia adelante
linterna = SpotLight3D.create(
    Vertex3D.new(0, 1.5, 0),      -- Posición (altura de la mano)
    Vertex3D.new(0, 0, 1)          -- Dirección (hacia adelante)
)

-- Configurar como luz blanca brillante
linterna:setColor(Color.new(1.0, 1.0, 1.0, 1.0))
linterna:setColorSpecular(Color.new(1.0, 1.0, 1.0, 1.0))

-- Configurar cono (estrecho)
linterna:setCutOff(math.cos(math.rad(12.5)))       -- 12.5 grados
linterna:setOuterCutOff(math.cos(math.rad(17.5)))  -- 17.5 grados

-- Atenuación media
linterna:setConstant(1.0)
linterna:setLinear(0.09)
linterna:setCuadratic(0.032)

brakeza:addObject3D(linterna, "linterna")
```

**Ejemplo - Foco de escenario:**
```lua
-- Foco desde arriba
foco = SpotLight3D.create(
    Vertex3D.new(0, 10, 0),    -- Arriba
    Vertex3D.new(0, -1, 0)     -- Hacia abajo
)

-- Luz blanca intensa
foco:setColor(Color.new(1.0, 1.0, 0.9, 1.0))
foco:setColorSpecular(Color.new(1.0, 1.0, 1.0, 1.0))

-- Cono amplio
foco:setCutOff(math.cos(math.rad(25.0)))
foco:setOuterCutOff(math.cos(math.rad(30.0)))

-- Largo alcance
foco:setConstant(1.0)
foco:setLinear(0.045)
foco:setCuadratic(0.0075)

brakeza:addObject3D(foco, "foco_escenario")
```

**Mover la dirección del spotlight:**
```lua
-- Hacer que el spotlight apunte hacia un objeto
spotlight:lookAt(objetivo)

-- O establecer dirección manualmente
nuevaDireccion = Vertex3D.new(0, -0.5, 1)
spotlight:setDirection(nuevaDireccion:getNormalize())
```

---

**Nota sobre shadow mapping:** Las spotlights pueden proyectar sombras si el shadow mapping está habilitado en la configuración del motor.

---

>[Back to index](#index)

# 8) Particle emitter

El sistema de partículas permite crear efectos visuales como humo, fuego, chispas, etc.

**Constructor:**
```lua
emitter = ParticleEmitter.create(
    posicion,           -- Vertex3D
    ttl,                -- float (tiempo de vida del emisor, -1 = infinito)
    colorInicio,        -- Color
    colorFin,           -- Color
    contextoParticulas, -- ParticlesContext
    "textura.png"       -- string
)
```

### ParticlesContext

Configuración del comportamiento de las partículas.

**Constructor completo:**
```lua
context = ParticlesContext.new(
    gravedad,           -- float
    particulasPorSeg,   -- int
    vidaParticula,      -- int (milisegundos)
    rangoAngulo,        -- int (grados)
    velocidadMin,       -- int
    velocidadMax,       -- int
    alphaMin,           -- int (0-255)
    alphaMax,           -- int (0-255)
    ruidoPosicion,      -- int
    ruidoVelocidad,     -- int
    factorDeceleracion  -- float
)
```

**Propiedades configurables:**
- `GRAVITY`: Gravedad aplicada a las partículas.
- `PARTICLES_BY_SECOND`: Partículas generadas por segundo.
- `PARTICLE_LIFESPAN`: Tiempo de vida de cada partícula (ms).
- `SMOKE_ANGLE_RANGE`: Rango de ángulo de emisión (grados).
- `MIN_VELOCITY` / `MAX_VELOCITY`: Rango de velocidad inicial.
- `MIN_ALPHA` / `MAX_ALPHA`: Rango de transparencia (0-255).
- `POSITION_NOISE`: Ruido en posición inicial.
- `VELOCITY_NOISE`: Ruido en velocidad.
- `DECELERATION_FACTOR`: Factor de desaceleración.

**Contexto por defecto:**
```lua
context = ParticlesContext.defaultParticlesContext()
```

### Ejemplos

**Ejemplo 1 - Humo:**
```lua
-- Configuración de humo
humoContext = ParticlesContext.new(
    -0.5,    -- Gravedad negativa (sube)
    50,      -- 50 partículas/segundo
    3000,    -- Viven 3 segundos
    30,      -- Ángulo de 30 grados
    10, 30,  -- Velocidad 10-30
    50, 150, -- Alpha 50-150 (semitransparente)
    20,      -- Ruido de posición
    10,      -- Ruido de velocidad
    0.98     -- Poca desaceleración
)

-- Crear emisor de humo
humo = ParticleEmitter.create(
    Vertex3D.new(0, 1, 0),
    -1,  -- Infinito
    Color.new(0.5, 0.5, 0.5, 0.8),  -- Gris inicio
    Color.new(0.3, 0.3, 0.3, 0.0),  -- Gris oscuro fin (fade out)
    humoContext,
    "../assets/images/smoke_particle.png"
)
brakeza:addObject3D(humo, "humo")
```

**Ejemplo 2 - Fuego:**
```lua
-- Configuración de fuego
fuegoContext = ParticlesContext.new(
    -1.0,    -- Sube rápido
    100,     -- Muchas partículas
    1500,    -- Vida corta
    25,      -- Ángulo estrecho
    30, 50,  -- Velocidad rápida
    200, 255, -- Muy visible
    15,      -- Poco ruido
    5,
    0.95     -- Desaceleración moderada
)

-- Crear fuego
fuego = ParticleEmitter.create(
    Vertex3D.new(0, 0.5, 0),
    -1,
    Color.new(1.0, 0.8, 0.0, 1.0),  -- Amarillo/naranja
    Color.new(1.0, 0.0, 0.0, 0.0),  -- Rojo que desaparece
    fuegoContext,
    "../assets/images/fire_particle.png"
)
brakeza:addObject3D(fuego, "fuego")
```

**Ejemplo 3 - Explosión temporal:**
```lua
-- Contexto de explosión
explosionContext = ParticlesContext.new(
    2.0,     -- Gravedad fuerte
    500,     -- Muchas partículas
    800,     -- Vida muy corta
    360,     -- Todas direcciones
    50, 100, -- Muy rápido
    255, 255, -- Completamente visible
    30,      -- Mucho ruido
    20,
    0.85     -- Desaceleración rápida
)

-- Explosión que dura 1 segundo
explosion = ParticleEmitter.create(
    Vertex3D.new(enemigo:getPosition()),
    1.0,  -- 1 segundo de vida total
    Color.new(1.0, 0.5, 0.0, 1.0),  -- Naranja brillante
    Color.new(0.3, 0.3, 0.3, 0.0),  -- Gris que desaparece
    explosionContext,
    "../assets/images/spark.png"
)
brakeza:addObject3D(explosion, "explosion")
```

---

>[Back to index](#index)

# 9) Input system

El `ComponentInput` gestiona entrada de teclado, mouse y gamepad.

```lua
componentsManager = ComponentsManager.get()
input = componentsManager:getComponentInput()
```

- [Teclado](#teclado)
- [Mouse](#mouse)
- [Gamepad / Controller](#gamepad--controller)

---

## Teclado

**Métodos:**
- `isKeyEventDown(SDL_Keycode) => bool`: Tecla presionada (evento único).
- `isKeyEventUp(SDL_Keycode) => bool`: Tecla soltada (evento único).
- `isCharPressed(char) => bool`: Carácter presionado (continuo).
- `isCharFirstEventDown(char) => bool`: Primera vez que se presiona.

**Ejemplo:**
```lua
input = componentsManager:getComponentInput()

-- Movimiento continuo con WASD
if input:isCharPressed('w') then
    -- Moverse adelante
end
if input:isCharPressed('s') then
    -- Moverse atrás
end
if input:isCharPressed('a') then
    -- Moverse izquierda
end
if input:isCharPressed('d') then
    -- Moverse derecha
end

-- Acción única (por ejemplo, saltar)
if input:isCharFirstEventDown(' ') then  -- Espacio
    player:jump(Vertex3D.new(0, 500, 0))
end
```

---

## Mouse

**Métodos de estado:**
- `isMouseMotion() => bool`: Comprueba si el mouse se está moviendo.
- `isClickLeft() => bool`: Click izquierdo (evento).
- `isClickRight() => bool`: Click derecho (evento).
- `isLeftMouseButtonPressed() => bool`: Botón izquierdo presionado (continuo).
- `isRightMouseButtonPressed() => bool`: Botón derecho presionado (continuo).

**Métodos de posición:**
- `getRelativeRendererMouseX() => int`: Posición X del mouse relativa al renderer.
- `getRelativeRendererMouseY() => int`: Posición Y del mouse relativa al renderer.
- `getMouseMotionXRel() => int`: Movimiento relativo X desde el último frame.
- `getMouseMotionYRel() => int`: Movimiento relativo Y desde el último frame.

**Ejemplo - Control de cámara con mouse:**
```lua
input = componentsManager:getComponentInput()
camera = componentsManager:getComponentCamera():getCamera()

-- Sensibilidad del mouse
local sensitivity = 0.002

if input:isMouseMotion() then
    -- Obtener movimiento del mouse
    local deltaX = input:getMouseMotionXRel()
    local deltaY = input:getMouseMotionYRel()
    
    -- Rotar cámara
    local currentRot = camera:getRotation()
    local eulerX = deltaY * sensitivity
    local eulerY = deltaX * sensitivity
    
    camera:setRotationFromEulerAngles(eulerX, eulerY, 0)
end

-- Disparo con click izquierdo
if input:isClickLeft() then
    -- Crear proyectil
    print("¡Disparo!")
end
```

---

## Gamepad / Controller

Brakeza3D soporta control completo con gamepad (Xbox, PlayStation, etc.).

### Estado del controller

- `isGameControllerEnabled() => bool`: Comprueba si hay un gamepad conectado.
- `isAnyControllerButtonPressed() => bool`: Comprueba si se presionó algún botón.

### Botones

**Botones de acción (A, B, X, Y):**
- `getControllerButtonA() => bool`
- `getControllerButtonB() => bool`
- `getControllerButtonX() => bool`
- `getControllerButtonY() => bool`

**Bumpers (L1/R1):**
- `getControllerShoulderLeft() => bool`
- `getControllerShoulderRight() => bool`

**Botones de sistema:**
- `getControllerButtonBack() => bool`: Botón back/select
- `getControllerButtonGuide() => bool`: Botón guía/home
- `getControllerButtonStart() => bool`: Botón start

**D-Pad:**
- `getControllerPadUp() => bool`
- `getControllerPadDown() => bool`
- `getControllerPadLeft() => bool`
- `getControllerPadRight() => bool`

### Ejes analógicos

**Stick izquierdo:**
- `getControllerAxisLeftX() => float`: Eje X (-1.0 a 1.0)
- `getControllerAxisLeftY() => float`: Eje Y (-1.0 a 1.0)

**Stick derecho:**
- `getControllerAxisRightX() => float`: Eje X (-1.0 a 1.0)
- `getControllerAxisRightY() => float`: Eje Y (-1.0 a 1.0)

**Triggers (L2/R2):**
- `getControllerAxisTriggerLeft() => float`: Gatillo izquierdo (0.0 a 1.0)
- `getControllerAxisTriggerRight() => float`: Gatillo derecho (0.0 a 1.0)

### Ejemplo completo - Control con gamepad

```lua
input = componentsManager:getComponentInput()

function onUpdate(deltaTime)
    if not input:isGameControllerEnabled() then
        return  -- No hay gamepad conectado
    end
    
    -- Movimiento con stick izquierdo
    local axisX = input:getControllerAxisLeftX()
    local axisY = input:getControllerAxisLeftY()
    
    local moveSpeed = 10.0
    local deadzone = 0.15  -- Zona muerta para evitar drift
    
    if math.abs(axisX) > deadzone or math.abs(axisY) > deadzone then
        local direction = Vertex3D.new(axisX, 0, -axisY)  -- Y invertido
        direction = direction:getScaled(moveSpeed)
        
        player:setWalkingDirection(direction)
    end
    
    -- Rotación de cámara con stick derecho
    local lookX = input:getControllerAxisRightX()
    local lookY = input:getControllerAxisRightY()
    local lookSensitivity = 0.05
    
    if math.abs(lookX) > deadzone or math.abs(lookY) > deadzone then
        -- Rotar cámara...
        camera:addToRotation(lookX * lookSensitivity, lookY * lookSensitivity, 0)
    end
    
    -- Saltar con botón A
    if input:getControllerButtonA() and player:onGround() then
        player:jump(Vertex3D.new(0, 500, 0))
    end
    
    -- Disparar con trigger derecho
    local shootTrigger = input:getControllerAxisTriggerRight()
    if shootTrigger > 0.5 then
        -- Disparar arma
        print("¡Bang! Intensidad: " .. shootTrigger)
    end
    
    -- Cambiar arma con bumpers
    if input:getControllerShoulderLeft() then
        -- Arma anterior
        cambiarArma(-1)
    end
    if input:getControllerShoulderRight() then
        -- Arma siguiente
        cambiarArma(1)
    end
    
    -- Pausa con Start
    if input:getControllerButtonStart() then
        togglePause()
    end
end
```

### Ejemplo - Soporte dual (teclado + gamepad)

```lua
function getMovementInput()
    local direction = Vertex3D.new(0, 0, 0)
    local input = componentsManager:getComponentInput()
    
    -- Gamepad tiene prioridad si está conectado
    if input:isGameControllerEnabled() then
        local axisX = input:getControllerAxisLeftX()
        local axisY = input:getControllerAxisLeftY()
        
        if math.abs(axisX) > 0.15 or math.abs(axisY) > 0.15 then
            direction = Vertex3D.new(axisX, 0, -axisY)
            return direction
        end
    end
    
    -- Fallback a teclado
    if input:isCharPressed('w') then
        direction = direction + Vertex3D.new(0, 0, 1)
    end
    if input:isCharPressed('s') then
        direction = direction + Vertex3D.new(0, 0, -1)
    end
    if input:isCharPressed('a') then
        direction = direction + Vertex3D.new(-1, 0, 0)
    end
    if input:isCharPressed('d') then
        direction = direction + Vertex3D.new(1, 0, 0)
    end
    
    return direction:getNormalize()
end
```

---

>[Back to index](#index)

# 10) Global illumination

Brakeza3D permite configurar una luz direccional global (tipo sol) que afecta a toda la escena.

La iluminación sigue el modelo **Ambient-Diffuse-Specular (ADS)**.

### Configurar iluminación global

```lua
componentsManager = ComponentsManager.get()
render = componentsManager:getComponentRender()
```

**Métodos:**
- `setGlobalIlluminationDirection(Vertex3D)`: Dirección de la luz (normalizado).
- `setGlobalIlluminationAmbient(Vertex3D)`: Color ambiental (RGB como Vertex3D).
- `setGlobalIlluminationDiffuse(Vertex3D)`: Color difuso (luz directa).
- `setGlobalIlluminationSpecular(Vertex3D)`: Color especular (brillos).

### Ejemplo - Luz de día

```lua
render = componentsManager:getComponentRender()

-- Dirección desde arriba y ligeramente lateral
render:setGlobalIlluminationDirection(Vertex3D.new(0.3, -1.0, 0.5):getNormalize())

-- Luz ambiental suave
render:setGlobalIlluminationAmbient(Vertex3D.new(0.3, 0.3, 0.3))

-- Luz difusa clara (amarillo suave)
render:setGlobalIlluminationDiffuse(Vertex3D.new(0.9, 0.9, 0.7))

-- Especular blanco
render:setGlobalIlluminationSpecular(Vertex3D.new(1.0, 1.0, 1.0))
```

### Ejemplo - Atardecer

```lua
-- Sol bajo en el horizonte
render:setGlobalIlluminationDirection(Vertex3D.new(1.0, -0.2, 0.0):getNormalize())

-- Ambiental naranja oscuro
render:setGlobalIlluminationAmbient(Vertex3D.new(0.4, 0.2, 0.1))

-- Difusa naranja/roja
render:setGlobalIlluminationDiffuse(Vertex3D.new(1.0, 0.4, 0.2))

-- Especular naranja
render:setGlobalIlluminationSpecular(Vertex3D.new(1.0, 0.6, 0.3))
```

### Ejemplo - Noche

```lua
-- Luna desde arriba
render:setGlobalIlluminationDirection(Vertex3D.new(0.0, -1.0, -0.3):getNormalize())

-- Ambiental muy oscuro azulado
render:setGlobalIlluminationAmbient(Vertex3D.new(0.05, 0.05, 0.15))

-- Difusa azul muy tenue
render:setGlobalIlluminationDiffuse(Vertex3D.new(0.1, 0.1, 0.3))

-- Especular azul pálido
render:setGlobalIlluminationSpecular(Vertex3D.new(0.2, 0.2, 0.5))
```

---

### Dibuja líneas de debug

El ComponentRender también permite dibujar líneas 3D útiles para debug:

```lua
-- Dibujar línea desde A hasta B en color rojo
puntoA = Vertex3D.new(0, 0, 0)
puntoB = Vertex3D.new(5, 5, 5)
colorRojo = Color.new(1.0, 0.0, 0.0, 1.0)

render:drawLine(puntoA, puntoB, colorRojo)
```

---

>[Back to index](#index)

# 11) Sound system

El `ComponentSound` gestiona la reproducción de sonidos y música.

```lua
componentsManager = ComponentsManager.get()
sound = componentsManager:getComponentSound()
```

**Nota:** ComponentSound hereda de Component, por lo que también tiene `setEnabled(bool)` e `isEnabled()`.

## Métodos principales

### Cargar recursos

- `addSound(nombre, "ruta/sonido.wav")`: Carga un efecto de sonido.
- `addMusic(nombre, "ruta/musica.mp3")`: Carga una pista de música.

### Control de música

- `playSound(nombre)`: Reproduce una pista de música (solo una puede sonar a la vez).
- `stopMusic()`: Detiene la música actual.
- `setMusicVolume(int)`: Volumen de la música (0-128).

### Control de efectos de sonido

- `setSoundsVolume(int)`: Volumen de los efectos de sonido (0-128).

**Nota:** Los sonidos se reproducen automáticamente al ser añadidos. Para música, usa `playSound()`.

## Ejemplo completo

```lua
sound = componentsManager:getComponentSound()

-- Cargar música de fondo
sound:addMusic("menu_music", "../assets/sounds/menu_theme.mp3")
sound:addMusic("game_music", "../assets/sounds/game_theme.mp3")

-- Cargar efectos de sonido
sound:addSound("jump", "../assets/sounds/jump.wav")
sound:addSound("shoot", "../assets/sounds/shoot.wav")
sound:addSound("explosion", "../assets/sounds/explosion.wav")

-- Configurar volúmenes
sound:setMusicVolume(64)   -- Mitad del volumen máximo
sound:setSoundsVolume(100) -- Efectos más altos

-- Reproducir música del menú
sound:playSound("menu_music")

-- Cuando empiece el juego
function startGame()
    sound:stopMusic()
    sound:playSound("game_music")
end

-- Cuando el jugador salte
function playerJump()
    sound:addSound("jump", "../assets/sounds/jump.wav")  -- Se reproduce automáticamente
end

-- Cuando dispare
function playerShoot()
    sound:addSound("shoot", "../assets/sounds/shoot.wav")
end
```

## Control dinámico de volumen

```lua
-- Fade out de música
function fadeOutMusic()
    for i = 64, 0, -2 do
        sound:setMusicVolume(i)
        -- Esperar un frame (esto requeriría un sistema de corrutinas)
    end
    sound:stopMusic()
end

-- Ajustar volumen desde opciones
function setMasterVolume(percentage)  -- 0.0 a 1.0
    local musicVol = math.floor(128 * percentage)
    local soundVol = math.floor(128 * percentage)
    
    sound:setMusicVolume(musicVol)
    sound:setSoundsVolume(soundVol)
end
```

---

>[Back to index](#index)

# 12) Scripting system

Los scripts LUA son el corazón de la lógica de tu aplicación en Brakeza3D.

## Tipos de scripts

Los scripts pueden vincularse a tres niveles:

1. **Scripts de proyecto**: Se ejecutan durante toda la vida del proyecto.
2. **Scripts de escena**: Se ejecutan mientras la escena está activa.
3. **Scripts de objeto**: Se ejecutan en el contexto de un `Object3D` específico.

## Callbacks del ciclo de vida

Cada script puede implementar los siguientes callbacks:

```lua
-- Se ejecuta una vez al inicializar
function onStart()
    -- Inicialización
end

-- Se ejecuta cada frame
function onUpdate(deltaTime)
    -- Lógica principal
    -- deltaTime: tiempo transcurrido desde el último frame
end

-- Se ejecuta al finalizar
function onEnd()
    -- Limpieza
end

-- Se ejecuta cuando ocurre una colisión
function onCollision(collision)
    -- collision es un objeto CollisionInfo
    local otroObjeto = collision:getObject()
    print("Colisión con: " .. otroObjeto:getLabel())
end
```

## Variables globales automáticas

En cada script tienes acceso a:

- `brakeza`: Instancia principal del motor (Brakeza3D).
- `componentsManager`: Gestor de componentes (ComponentsManager).
- `object`: El objeto al que está vinculado el script (solo en scripts de objeto).

## Gestión de variables

### Variables locales del script

Cada objeto puede tener variables locales en sus scripts:

```lua
-- En el script del objeto
function onStart()
    -- Guardar variable local
    object:setLocalScriptVar("vida", 100)
    object:setLocalScriptVar("nombre", "Enemigo1")
end

function onUpdate(deltaTime)
    -- Leer variable local
    local vida = object:getLocalScriptVar("vida")
    
    if vida <= 0 then
        object:setRemoved(true)
    end
end
```

### Variables globales de la escena

```lua
scripting = componentsManager:getComponentScripting()

-- Guardar variable global
scripting:setGlobalScriptVar("puntuacion", 0)

-- Leer variable global
local puntos = scripting:getGlobalScriptVar("puntuacion")
```

## Gestión de scripts

### Crear y cargar scripts

```lua
-- Crear un script desde archivo
script = ScriptLUA.create("../assets/scripts/enemy_ai.lua")

-- Vincular script a un objeto
enemigo:attachScript("../assets/scripts/enemy_ai.lua")

-- Recargar scripts de un objeto
enemigo:reloadScriptsEnvironment()
```

### Añadir scripts a la escena

```lua
scripting = componentsManager:getComponentScripting()
scripting:addSceneLUAScript("../assets/scripts/game_manager.lua")
```

## Ejemplo completo - Script de enemigo

```lua
-- enemy.lua

-- Variables del enemigo
local velocidad = 5.0
local vidaMaxima = 100
local rangoDeteccion = 10.0
local player = nil

function onStart()
    -- Inicializar variables locales
    object:setLocalScriptVar("vida", vidaMaxima)
    object:setLocalScriptVar("estado", "patrulla")
    
    -- Buscar al jugador
    player = brakeza:getSceneObjectByLabel("player")
    
    -- Configurar física
    object:setupRigidBodyCollider(SIMPLE_SHAPE)
    object:setMass(50.0)
    object:setAngularFactor(Vertex3D.new(0, 1, 0))
end

function onUpdate(deltaTime)
    if player == nil then return end
    
    local estado = object:getLocalScriptVar("estado")
    local vida = object:getLocalScriptVar("vida")
    
    -- Comprobar si está muerto
    if vida <= 0 then
        morir()
        return
    end
    
    -- Calcular distancia al jugador
    local posEnemigo = object:getPosition()
    local posPlayer = player:getPosition()
    local distancia = posEnemigo:distance(posPlayer)
    
    -- Máquina de estados
    if estado == "patrulla" then
        if distancia < rangoDeteccion then
            object:setLocalScriptVar("estado", "perseguir")
        else
            patrullar(deltaTime)
        end
    elseif estado == "perseguir" then
        if distancia > rangoDeteccion * 2 then
            object:setLocalScriptVar("estado", "patrulla")
        else
            perseguirJugador(deltaTime)
        end
    end
end

function patrullar(deltaTime)
    -- Moverse en un patrón
    local direccion = object:AxisForward():getScaled(velocidad * 0.5)
    object:setWalkingDirection(direccion)
end

function perseguirJugador(deltaTime)
    -- Mirar al jugador
    object:lookAt(player)
    
    -- Moverse hacia el jugador
    local direccion = object:AxisForward():getScaled(velocidad)
    object:setWalkingDirection(direccion)
end

function recibirDanio(cantidad)
    local vida = object:getLocalScriptVar("vida")
    vida = vida - cantidad
    object:setLocalScriptVar("vida", vida)
    
    print("Enemigo recibió " .. cantidad .. " de daño. Vida: " .. vida)
end

function morir()
    print("Enemigo eliminado")
    
    -- Crear explosión de partículas
    local pos = object:getPosition()
    local explosion = ParticleEmitter.create(
        pos, 1.0,
        Color.new(1.0, 0.0, 0.0, 1.0),
        Color.new(0.5, 0.5, 0.5, 0.0),
        ParticlesContext.defaultParticlesContext(),
        "../assets/images/particle.png"
    )
    brakeza:addObject3D(explosion, "explosion_" .. object:getLabel())
    
    -- Eliminar enemigo
    object:setRemoved(true)
end

function onCollision(collision)
    local otro = collision:getObject()
    
    -- Si colisiona con una bala
    if string.find(otro:getLabel(), "bullet") then
        recibirDanio(25)
        otro:setRemoved(true)
    end
end

function onEnd()
    print("Script de enemigo finalizado")
end
```

## Ejemplo - Game Manager

```lua
-- game_manager.lua (script de escena)

local puntuacion = 0
local enemigosEliminados = 0
local tiempoJuego = 0

function onStart()
    -- Inicializar variables globales
    scripting = componentsManager:getComponentScripting()
    scripting:setGlobalScriptVar("puntuacion", 0)
    scripting:setGlobalScriptVar("oleadaActual", 1)
    
    -- Configurar iluminación
    local render = componentsManager:getComponentRender()
    render:setGlobalIlluminationDirection(Vertex3D.new(0.5, -1.0, 0.5):getNormalize())
    render:setGlobalIlluminationAmbient(Vertex3D.new(0.3, 0.3, 0.3))
    render:setGlobalIlluminationDiffuse(Vertex3D.new(0.8, 0.8, 0.8))
    
    -- Cargar música
    local sound = componentsManager:getComponentSound()
    sound:addMusic("game", "../assets/sounds/game_music.mp3")
    sound:playSound("game")
end

function onUpdate(deltaTime)
    tiempoJuego = tiempoJuego + deltaTime
    
    -- Actualizar UI (esto requeriría TextWriter)
    -- ...
end

function sumarPuntos(cantidad)
    local scripting = componentsManager:getComponentScripting()
    local puntos = scripting:getGlobalScriptVar("puntuacion")
    puntos = puntos + cantidad
    scripting:setGlobalScriptVar("puntuacion", puntos)
end

function enemigoEliminado()
    enemigosEliminados = enemigosEliminados + 1
    sumarPuntos(100)
end
```

---

>[Back to index](#index)

# 13) Shaders system

Brakeza3D ofrece la posibilidad de que incorpores tus propios shaders GLSL.

De igual forma que sucede con los ``Scripts``, podrás vincular `Shaders` a tus ``escenas``
y/o ``Object3D``.


- [Tipos de Shaders](#tipos-de-shaders)
- [Variables de Shaders](#variables-de-shaders-uniforms)
- [Configurar uniforms desde Lua](#configurar-uniforms-desde-lua)
- [Template Shader de postprocesado](#template-shader-de-postprocesado)
- [Shaders internos](#shaders-internos)

---

### Tipos de Shaders

Existen dos tipos de shaders:

- `Postprocesado`: Un shader de postprocesado se aplica después de que la escena ha sido renderizada, modificando
  la imagen final antes de enviarla a la pantalla. Solo pueden ser aplicados a una `scene`.
- `Geometría`: Un shader de geometría procesa vértices y puede modificar la forma de los objetos antes de rasterizarlos. Solo puede ser aplicados a un `Mesh3D`

Podrás crear los shaders desde la UI, Brakeza3D creará automáticamente un fichero `.vs` y `.fs`, vertexshader y fragmenteshader respectivamente, los que podrás
modificar a tu antojo. También se generará un fichero ``json`` con metadatos.

---

### Variables de Shaders (uniforms)

También podrás configurar las variables (*uniforms*) que recibirá cada shader desde la UI.

Podrás configurar tus propias variables: ``int``, ``float``, ``vec2``, ``vec3``, ``vec4``, ``texture``
o bien podrás seleccionar variables pertenecientes al sistema: ``delta_time (float)``, ``execution_time (float)``
o `scene (texture)`.

Además, los shaders de geometría (que solo aplican a `Mesh3D`) te permiten usar tipos del sistema como: ``diffuse`` o ``specular``, con
texturas pertenecientes al modelo en particular.

---

### Configurar uniforms desde Lua

Puedes acceder y modificar los uniforms de los shaders desde tus scripts Lua.

**Obtener shader de la escena:**
```lua
render = componentsManager:getComponentRender()
shader = render:getSceneShaderByLabel("mi_shader_postprocess")
```

**Establecer valores de uniforms:**

El método `setDataTypeValue` acepta diferentes tipos de valores:

```lua
-- Valores numéricos
shader:setDataTypeValue("miFloat", 1.5)
shader:setDataTypeValue("miInt", 42)

-- Vectores GLM
shader:setDataTypeValue("miVec2", vec2.new(0.5, 0.5))
shader:setDataTypeValue("miVec3", vec3.new(1.0, 0.0, 0.0))
shader:setDataTypeValue("miVec4", vec4.new(1.0, 0.5, 0.0, 1.0))
```

**Ejemplo - Shader animado:**
```lua
-- Script de escena que anima un shader de postprocesado

local tiempo = 0

function onStart()
    render = componentsManager:getComponentRender()
    shader = render:getSceneShaderByLabel("efecto_onda")
end

function onUpdate(deltaTime)
    tiempo = tiempo + deltaTime
    
    -- Animar un uniform "time"
    shader:setDataTypeValue("time", tiempo)
    
    -- Cambiar intensidad del efecto
    local intensidad = math.sin(tiempo) * 0.5 + 0.5
    shader:setDataTypeValue("intensity", intensidad)
    
    -- Vector de dirección que rota
    local x = math.cos(tiempo)
    local y = math.sin(tiempo)
    shader:setDataTypeValue("direction", vec2.new(x, y))
end
```

**Ejemplo - Control de color:**
```lua
function cambiarColorShader(r, g, b, a)
    local shader = render:getSceneShaderByLabel("tint")
    shader:setDataTypeValue("tintColor", vec4.new(r, g, b, a))
end

-- Hacer pantalla roja gradualmente
function efectoDanio()
    cambiarColorShader(1.0, 0.0, 0.0, 0.5)
    -- (Luego ir reduciendo el alpha con el tiempo)
end
```

---

### Template Shader de postprocesado

A continuación las templates vacías para el shader de postprocesado.

VertexShader:
````glsl
#version 330 core

layout (location = 0) in vec4 vertex; // <vec2 position, vec2 texCoords>

out vec2 TexCoords;

uniform mat4 model;
uniform mat4 projection;

void main()
{
    TexCoords = vec2(vertex.z, -vertex.w);
    gl_Position = projection * model * vec4(vertex.xy, 0.0, 1.0);
}
````

FragmentShader:

````glsl
#version 330 core

in vec2 TexCoords;

out vec4 FragColor;

uniform sampler2D sceneTexture;

void main()
{
   FragColor = texture(sceneTexture, TexCoords);
}
````

**Ejemplo - Shader de inversión de color:**
````glsl
#version 330 core

in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D sceneTexture;

void main()
{
    vec4 color = texture(sceneTexture, TexCoords);
    FragColor = vec4(1.0 - color.rgb, color.a);  // Invertir RGB
}
````

**Ejemplo - Shader de blur simple:**
````glsl
#version 330 core

in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D sceneTexture;
uniform float blurAmount;  // Configurable desde Lua

void main()
{
    vec2 texelSize = 1.0 / textureSize(sceneTexture, 0);
    vec4 result = vec4(0.0);
    
    // Blur 3x3
    for(int x = -1; x <= 1; x++) {
        for(int y = -1; y <= 1; y++) {
            vec2 offset = vec2(x, y) * texelSize * blurAmount;
            result += texture(sceneTexture, TexCoords + offset);
        }
    }
    
    FragColor = result / 9.0;
}
````

### Template Shader de geometría

A continuación las templates vacías para el shader de geometría.

VertexShader:
````glsl
#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    TexCoords = aTexCoords;

    gl_Position = projection * view * vec4(FragPos, 1.0);
}
````

FragmentShader:

````glsl
#version 330 core

uniform sampler2D diffuse;

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

void main()
{
    vec3 norm = normalize(Normal);
    
    FragColor = texture(diffuse, TexCoords);
}

````

---

### Shaders internos

Puedes encontra los shaders que son utilizados internamente por Brakeza3D en la carpeta `/GLSL`.

- `Render` (*/GLSL/Render*): Render de Mesh3D.
- `Outliner` (*/GLSL/Outliner*): Outliner para un objeto seleccionado.
- `Color` (*/GLSL/Color*): Render de Mesh3D de un color dado.
- `Wireframe` (*/GLSL/Wireframe*): Render de vertices de un Mesh3D.
- `Points` (*/GLSL/Points*): Render de vertices de Mesh3D.
- `Line3D` (*/GLSL/Line3D*): Render de líneas 3D.
- `Image` (*/GLSL/Image*): Render una imagen en pantalla.
- `Line` (*/GLSL/Line*): Dibuja una línea 2D en pantalla.

Shaders de postprocesamiento:

- `DeepOfField` (*/GLSL/DeepOfField*): Efecto de profundidad de campo.
- `FOG` (*/GLSL/FOG*): Efecto de niebla.
- `DepthMap` (*/GLSL/DepthMap*): Dibuja el mapa de profundidad de la escena.
- `Tint` (*/GLSL/Tint*): Colorea la pantalla de un color y un alpha dado.
- `Shading` (*/GLSL/Shading*): Renderiza un objeto a un buffer, para ser usado como máscara.
- `Particle` (*/GLSL/Particle*): Dibuja una partícula en pantalla. Usado por el objeto **ParticleEmitter**.

¡Si sabes lo que haces, puedes manipular los shaders libremente si lo consideras!.

---

>[Back to index](#index)

# 14) Assets folder

Brakeza3D esperará encontrar cada tipo de recurso en su carpeta raíz correspondiente. Dentro de la carpeta
`assets` podrás encontrar:

- `animations`: Ficheros `FBX` animados.
- `fonts`: Ficheros de fuente `TTF`.
- `icons`: Ficheros de iconos de la UI.
- `images`: Ficheros de imagen de carácter general.
- `maps`: Ficheros de mapas `BSP`.
- `models`: Ficheros de modelos `FBX`.
- `projects`: Ficheros de proyectos `JSON`.
- `scenes`: Ficheros de escenas `JSON`.
- `scripts`: Ficheros de scripts `LUA`.
- `shaders`: Ficheros de CustomShaders (`.vs` y/o `.fs`)
- `sounds`: Ficheros de sonido `WAV` o `MP3`.
- `sprites`: Ficheros de imágenes en formato rejilla.
- `textures`: Ficheros de texturas usados por modelos `FBX`.

Brakeza3D permite trabajar con rutas relativas, pero siempre a la ruta principal de cada elemento, es decir,
podrás almacenar tus scripts en subcarpetas, pero siempre estarán dentro de la carpeta raíz de scripts, etc.

---

>[Back to index](#index)

# 15) TextWriter system

El sistema `TextWriter` permite renderizar texto en pantalla usando fuentes TTF.

**Constructor:**
```lua
writer = TextWriter.create("../assets/fonts/arial.ttf")
```

## Métodos

**Renderizado de texto:**
- `writeTextTTF(texto, x, y, color, escala)`: Escribe texto en posición absoluta.
- `writeTextTTFAutoSize(texto, x, y, color)`: Escribe texto con tamaño automático.
- `writeTextTTFMiddleScreen(texto, color, escala)`: Escribe texto centrado en pantalla.
- `writeTTFCenterHorizontal(texto, y, color, escala)`: Escribe texto centrado horizontalmente.

**Propiedades:**
- `setAlpha(float)`: Establece la transparencia del texto (0.0 - 1.0).
- `getAlpha() => float`: Obtiene la transparencia actual.
- `setFont(string)`: Cambia la fuente.

## Ejemplos

### Ejemplo 1 - HUD básico

```lua
-- En el script de escena
local writer = nil
local puntuacion = 0
local vida = 100

function onStart()
    writer = TextWriter.create("../assets/fonts/game_font.ttf")
    writer:setAlpha(1.0)
end

function onUpdate(deltaTime)
    -- Puntuación en la esquina superior izquierda
    writer:writeTextTTF(
        "Puntos: " .. puntuacion,
        20, 20,
        Color.new(1.0, 1.0, 1.0, 1.0),
        1.0
    )
    
    -- Vida en la esquina superior derecha
    local colorVida = vida > 30 
        and Color.new(0.0, 1.0, 0.0, 1.0)  -- Verde
        or Color.new(1.0, 0.0, 0.0, 1.0)   -- Rojo
    
    writer:writeTextTTF(
        "Vida: " .. vida,
        1800, 20,
        colorVida,
        1.0
    )
end
```

### Ejemplo 2 - Mensaje centrado

```lua
function mostrarMensaje(mensaje)
    writer = TextWriter.create("../assets/fonts/title.ttf")
    writer:writeTextTTFMiddleScreen(
        mensaje,
        Color.new(1.0, 1.0, 0.0, 1.0),  -- Amarillo
        2.0  -- Escala grande
    )
end

-- Usar
mostrarMensaje("¡GAME OVER!")
```

### Ejemplo 3 - Texto con fade

```lua
local mensajeAlpha = 1.0
local mensajeTiempo = 0

function onStart()
    writer = TextWriter.create("../assets/fonts/ui.ttf")
end

function onUpdate(deltaTime)
    if mensajeTiempo > 0 then
        mensajeTiempo = mensajeTiempo - deltaTime
        mensajeAlpha = mensajeTiempo / 3.0  -- 3 segundos de fade
        
        writer:setAlpha(mensajeAlpha)
        writer:writeTTFCenterHorizontal(
            "¡Nuevo objetivo!",
            500,
            Color.new(1.0, 1.0, 1.0, 1.0),
            1.5
        )
    end
end

function mostrarObjetivo()
    mensajeTiempo = 3.0
    mensajeAlpha = 1.0
end
```

### Ejemplo 4 - FPS counter

```lua
local writer = nil
local fpsTexto = "FPS: 0"

function onStart()
    writer = TextWriter.create("../assets/fonts/mono.ttf")
    writer:setAlpha(0.7)  -- Semitransparente
end

function onUpdate(deltaTime)
    -- Calcular FPS aproximado
    local fps = math.floor(1.0 / deltaTime)
    fpsTexto = "FPS: " .. fps
    
    -- Mostrar en esquina
    writer:writeTextTTF(
        fpsTexto,
        10, 10,
        Color.new(0.0, 1.0, 0.0, 1.0),
        0.8
    )
end
```

### Ejemplo 5 - Diálogo con nombre de personaje

```lua
local dialogoActivo = false
local dialogoTexto = ""
local dialogoNombre = ""

function mostrarDialogo(nombre, texto)
    dialogoActivo = true
    dialogoNombre = nombre
    dialogoTexto = texto
end

function onUpdate(deltaTime)
    if dialogoActivo then
        -- Fondo semitransparente (necesitarías un Image2D)
        
        -- Nombre del personaje
        writer:writeTextTTF(
            dialogoNombre,
            100, 900,
            Color.new(1.0, 1.0, 0.0, 1.0),  -- Amarillo
            1.2
        )
        
        -- Texto del diálogo
        writer:writeTextTTF(
            dialogoTexto,
            100, 950,
            Color.new(1.0, 1.0, 1.0, 1.0),  -- Blanco
            1.0
        )
        
        -- Cerrar diálogo con Enter
        local input = componentsManager:getComponentInput()
        if input:isCharFirstEventDown('\r') then
            dialogoActivo = false
        end
    end
end
```

---

>[Back to index](#index)

# 16) Scene management

El sistema `SceneLoader` permite guardar, cargar y limpiar escenas.

**Acceso:**
```lua
render = componentsManager:getComponentRender()
sceneLoader = render:getSceneLoader()
```

## Métodos

- `saveScene(nombre)`: Guarda la escena actual con el nombre especificado.
- `loadScene(nombre)`: Carga una escena previamente guardada.
- `clearScene()`: Limpia todos los objetos de la escena actual.

## Ejemplo - Sistema de niveles

```lua
-- game_manager.lua

local nivelActual = 1

function onStart()
    render = componentsManager:getComponentRender()
    sceneLoader = render:getSceneLoader()
    
    cargarNivel(nivelActual)
end

function cargarNivel(numero)
    -- Limpiar escena actual
    sceneLoader:clearScene()
    
    -- Cargar nuevo nivel
    local nombreNivel = "nivel_" .. numero
    sceneLoader:loadScene(nombreNivel)
    
    print("Nivel " .. numero .. " cargado")
end

function completarNivel()
    nivelActual = nivelActual + 1
    
    if nivelActual <= 10 then
        cargarNivel(nivelActual)
    else
        print("¡Juego completado!")
        -- Cargar escena de créditos
        sceneLoader:loadScene("creditos")
    end
end

function reiniciarNivel()
    cargarNivel(nivelActual)
end

function volverAlMenu()
    sceneLoader:clearScene()
    sceneLoader:loadScene("menu_principal")
end
```

## Ejemplo - Guardar progreso

```lua
function guardarProgreso()
    -- Guardar estado actual como checkpoint
    render = componentsManager:getComponentRender()
    sceneLoader = render:getSceneLoader()
    
    sceneLoader:saveScene("checkpoint_nivel" .. nivelActual)
    
    print("Progreso guardado")
end

function cargarUltimoCheckpoint()
    sceneLoader:loadScene("checkpoint_nivel" .. nivelActual)
    print("Checkpoint cargado")
end
```

## Ejemplo - Transición entre escenas

```lua
local transicionando = false
local tiempoTransicion = 0
local proximaEscena = ""

function iniciarTransicion(nombreEscena)
    proximaEscena = nombreEscena
    transicionando = true
    tiempoTransicion = 0
end

function onUpdate(deltaTime)
    if transicionando then
        tiempoTransicion = tiempoTransicion + deltaTime
        
        -- Fade out durante 1 segundo
        if tiempoTransicion < 1.0 then
            local alpha = tiempoTransicion
            -- Aplicar tint negro creciente
            -- (esto requeriría un shader de tint)
        else
            -- Cambiar escena
            sceneLoader:loadScene(proximaEscena)
            transicionando = false
            
            -- Fade in en la nueva escena
        end
    end
end
```

---

### Pathfinding en Grid3D

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

El método `makeTravelCubesGrid` devuelve ell camino solicitado si este fue posible mediante un array de `CubeGrid3D`,
cuya estructura es la siguiente:

```C
struct CubeGrid3D {
    AABB3D box;             // Caja contenedora 
    int posX;               // Índice de la posición X en el grid
    int posY;               // Índice de la posición Y en el grid
    int posZ;               // Índice de la posición Z en el grid
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

### Octree

El Octree permite particionamiento espacial eficiente para búsquedas y optimizaciones.

```lua
-- Construir octree en un modelo
modelo = Mesh3D.create(Vertex3D.new(0, 0, 0), "../assets/models/terrain.fbx")
modelo:buildOctree(5)  -- Profundidad 5
brakeza:addObject3D(modelo, "terreno")

-- Acceder al octree
octree = modelo:getOctree()

-- Acceder a la raíz
nodoRaiz = octree.root

-- Comprobar si es hoja
if nodoRaiz:isLeaf() then
    print("El nodo raíz es una hoja")
end

-- Acceder al AABB del nodo
aabb = nodoRaiz.bounds
print("Centro: ", aabb:getCenter())

-- Recorrer hijos si no es hoja
if not nodoRaiz:isLeaf() then
    for i, hijo in ipairs(nodoRaiz.children) do
        print("Hijo " .. i)
        -- Procesar hijo
    end
end
```

---

### Ejemplos en codigo

https://github.com/rzeronte/brakeza3d/blob/master/doc/examples-lua-code.md

---

**¡Documentación actualizada! 🎉**

Esta documentación refleja todas las capacidades actuales del motor Brakeza3D expuestas a través del scripting Lua.
