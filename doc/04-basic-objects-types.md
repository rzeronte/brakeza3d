# Tipos básicos de objetos

## El objeto Object3D

El objeto más básico en Brakeza3D es el `Object3D`.

Generalmente, cualquier elemento en la escena, será `Object3D`.

Las propiedades principales de un `Object3D` son:

- Posición (`Vertex3D`): Posición en el mundo del objeto
- Rotación (`M3`): Rotacion del objeto
- Escala (`float`): Factor de escala del modelo
- Activado (`bool`): Determina si el objeto está activado o desactivado.

Existen además varios tipos de dato a los que debemos acostumbrarnos. Los más habituales son:

- `Vertex3D`: Para almacenar posiciones en el espacio. Sus propiedades son: `X`, `Y`, `Z`.
- `M3`: Matriz 3x3 para almacenar rotaciones.
- `Point2D`: Para almacenar posiciones en 2D. Sus propiedades son `X`, `Y`.

## Manipulando un Object3D

Podemos manipular un Object3D.

- `getPosition()`: Obtenemos la posición del objeto en formato `Vertex3D`.
- `setPosition(Vertex3D)`: Establecemos una nueva posición para el objeto.
- `getRotation()`: Obtenemos la rotación del objeto en formato `M3`.
- `setRotation(M3)`: Establecemos una nueva rotación para el objeto
- `getScale()`: Obtenemos el factor de escala del modelo.
- `setScale(float)`: Establecemos un nuevo factor de escala.

## Scripting

Cualquier `Object3D` puede vincularse a scripts LUA. Veremos esto en profundidad en el apartado dedicado
al scripting.

## Objetos derivados de Object3D

Brakeza3D incluye multitud de objetos 3D listos para utilizarse. Todos extienden de
`Object3D`.

Por ejemplo tenemos objetos que pueden cargar modelos 3D, otros nos permitirán cargar una imagen en pantalla,
otros cargarán sprites animados, etc. A continuación enumero una lista de algunos de los más destacados:

- `Mesh3D`: Modelo 3D con triángulos y texturas
- `Mesh3DAnimation`: Similar a Mesh3D, pero para modelos animados.
- `Mesh3DGhost`: Modelo 3D colisionable.
- `Mesh3DBody`: Modelo 3D colisionable y reactivo a las físicas.
- `Image2D`: Dibuja una imagen en pantalla.
- `Image3D`: Plano 3D sobre el que se visualiza una textura de imagen.
- `Image2DAnimation`: Conjunto de `Image2D` que conforma una animación.
- `ParticleEmitter`: Emisor de partículas.
- `LightPoint`: Punto de luz.
- ...

A lo largo de la documentación nos detendremos en algunos de estos objetos para su mayor
comprensión y detalle.
