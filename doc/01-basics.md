>[Back to index](https://github.com/rzeronte/brakeza3d/blob/master/doc/00-index.md)

- [Introducción](#brakeza3d-conceptos-básicos)
- [Conceptos principales](#conceptos-principales)
- [Sistema de scripting](#sistema-de-scripting)
- [GUI](#gui)

---

# Introducción

Brakeza3D es un sencillo motor 2D/3D multiplataforma para desarrollo de videojuegos.

Incluye el motor de render, un scene graph, físicas de colisiones, scripting y una sencilla GUI.

Aunque el motor está escrito en C++, en esta documentación aprenderemos a crear nuestros propios
proyectos usando LUA como lenguaje de scripting.

---

## Conceptos principales

En Brakeza3D existen cuatro conceptos principales:

- `Proyectos`
- `Scripts`
- `Escenas`
- `Objetos`

### 1) Proyectos

Un proyecto es un contenedor de `escenas` y `scripts`.

Se pueden almacenar en disco y recuperar en cualquier momento.

### 2) Scripts

Ficheros de código fuente LUA para interactuar con Brakeza3D. Los scripts pueden 
vincularse a `proyectos`, `escenas` u `objetos` para implementar la lógica deseada.

Se pueden almacenar en disco y recuperar en cualquier momento.

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


