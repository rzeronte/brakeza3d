>[Back to index](https://github.com/rzeronte/brakeza3d/blob/master/doc/00-index.md)

- [El objeto Object3D](#el-objeto-object3d)
- [Interactuando con un Object3D](#interactuando-con-un-object3d)
- [Scripting sobre objetos](#scripting-sobre-objetos)
- [Objetos derivados de Object3D](#objetos-derivados-de-object3d)
- [Tipos básicos auxiliares](#tipos-básicos-auxiliares)

---

# Tipos básicos de objetos

## El objeto Object3D

El objeto más básico en Brakeza3D es el `Object3D`. Cualquier elemento en la escena, será `Object3D`.

Las propiedades principales de un `Object3D` son:

- Posición (`Vertex3D`): Posición en el mundo del objeto
- Rotación (`M3`): Rotacion del objeto
- Escala (`float`): Factor de escala del modelo
- Activado (`bool`): Determina si el objeto está activado o desactivado.

## Interactuando con un Object3D

Podemos interactuar un Object3D.

- `getLabel()`: Obtenemos el nombre del objeto.
- `setLabel()`: Establecemos un nuevo nombre de objeto.
- `getPosition()`: Obtenemos la posición del objeto en formato `Vertex3D`.
- `setPosition(Vertex3D)`: Establecemos una nueva posición para el objeto.
- `getRotation()`: Obtenemos la rotación del objeto en formato `M3`.
- `setRotation(M3)`: Establecemos una nueva rotación para el objeto
- `getScale()`: Obtenemos el factor de escala del modelo.
- `setScale(float)`: Establecemos un nuevo factor de escala.
- `setEnable(bool)`: Activa o desactiva el objeto en la escena.
- `setRemoved(bool)`: Elimina en el siguiente frame el objeto de la escena.

---

## Scripting sobre objetos

Cualquier `Object3D` puede ser vinculado a uno o varios scripts LUA. Veremos esto en profundidad en el apartado dedicado
al scripting.

---

## Objetos derivados de Object3D

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

## Tipos básicos auxiliares

Existen además varios tipos de dato que facilitan la implementación de tareas en 3D y estarán
disponibles a través de tus scripts LUA.

- `Vertex3D`: Posiciones en el espacio 3D (`X`, `Y`, `Z`).
- `Point2D`: Posiciones en 2D. (`X`, `Y`).
- `M3`: Matriz 3x3.
- `Color`: Color RGBA (rango 0-1)
- `ScriptLUA`: Objeto script con código LUA.

---

## Componentes

Brakeza3D agrupa sus funcionalidades en `Componentes`. Cada componente representa un aspecto
fundamental del núcleo de Brakeaz3D.

- `ComponentWindow`: Gestiona la ventana del sistema operativo.
- `ComponentScripting`: Gestiona el sistema de scripting.
- `ComponentCamera`: Gestiona la cámara.
- `ComponentCollisions`: Gestiona el motor de físicas y colisiones.
- `ComponentInput`: Gestiona la entrada por teclado o pad.
- `ComponentSound`: Gestiona la reproducción de sonidos.
- `ComponentRender`: Gestiona el render con OpenGL

Podrás acceder a todos ellos mediante el ``ComponentsManager`` desde tus scripts LUA. 

