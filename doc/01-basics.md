# Brakeza3D: Conceptos básicos

Brakeza3D es un sencillo motor 2D/3D multiplataforma para desarrollo de videojuegos.

Incluye el motor de render, un scene graph, físicas de colisiones, scripting y una sencilla GUI.

Aunque el motor está escrito en C++, en esta documentación aprenderemos a crear nuestros propios
proyectos usando LUA como lenguaje de scripting.

---

## Elementos principales

En Brakeza3D existen tres elementos principales:

- 1, Componentes
- 2, Objetos
- 3, Escenas

### 1) Componentes

Son módulos internos de Brakeza3D. Cada uno se encarga de un aspecto fundamental del funcionamiento del motor.

Existen varios componentes a tu disposición. A continuación se enumeran algunos de ellos:

- Componente Cámara
- Componente Render
- Componente Colisiones
- Componente I/O
- ...

Como programador, generalmente podrás acceder a dichos componentes según tus necesidades. 

### 2) Objetos

Un objeto es una figura abstracta que representa cualquier entidad existente en nuestro mundo 3D. Encontraremos
multitud de tipos de objeto a lo largo de esta documentación. A continuación se enumeran algunos de ellos:

- Cámara (Camera3D)
- Linea2D (Line2D)
- Punto 2D (Point2D)
- Malla (Mesh3D)
- Emisor de partículas (ParticleEmissor)
- ...

En líneas generales, cualquier cosa susceptible de ser representada en pantalla será un objeto.

Todos los objetos comparten las propiedades básicas de un elemento 3D ya sea nombre, posición, rotación, tamaño, pero cada tipo de objeto específico ampliará estas segun su caso.

Desde código podremos activar, mover, rotar, eliminar, escalar y muchas otras acciones sobre los objetos.

### 3) Escenas

Una escena se representa por un conjunto de objetos en un estado concreto. Su ventaja principal es que se pueden almacenar en disco y recuperar en cualquier momento.

La escena almacena demás información global como iluminación, posición de cámara, scripts, etc.

Dispondremos de un "SceneLoader", que nos permitirá gestionar el manejo de carga y cambio de escenas.

---

## Sistema de scripting y ciclo de vida de los objetos

En el búcle principal de Brakeza3D, se ejecutan una serie de acciones constantemente. Una de ellas
es el sistema de scripting. Este sistema puede estar en ON/OFF/PAUSE. 

Si está en ON, los objetos ejecutarán su ciclo de vida implementado en los scripts.

El sistema de scripting puede ser ejecutado (PLAY), para iniciar la ejecución de los scripts.

También puede ser rebobinado (REWIND) al inicio, para volver a ejecutar los scripts desde su inicio.

Finalmente, podrá ser detenido (STOP) para evitar que el sistema de scripting continue. 

## Ciclo de vida de un objeto

Los objetos disponen de su propio ciclo de vida el cual debemos de entender para trabajar con nuestros objetos cargados, es el siguiente:

- **onStart**: Instante en que se inicia la ejecución. Cuando activamos el sistema de scripting (PLAY)

- **onUpdate**: Instante actual (siempre y cuando el sistema de scripting esté ON)

- **onEnd**: Instante en el que se detiene la ejecución (STOP)

---

### Scripts LUA

Los scripts de LUA son elementos que podremos vincular a elementos del sistema. En ellos implementaremos la lógica y comportamiento de objetos en Brakeza3D. 

Podemos diferenciar dos grandes tipos de scripts:

- **Scripts de Objeto**: Se asocian a objetos. Un mismo script puede vincularse a multitud de objetos.
- **Scripts globales**: No se asocian a ningún elemento específico, son de caracter general.

---

### GUI

Brakeza3D incorpora una sencilla GUI que nos facilitará algunas de las operaciones más tediosas.

Dispondrás de un sencillo editor de escena totalmente integrado con el sistema de scripting.

Si bien puedes utilizar tu editor de código favorito para manipular los scripts, la GUI será de gran utilidad
para integrar tus scripts, manipular tus objetos, configurar la iluminación, guardar y cargar escenas, etc.


