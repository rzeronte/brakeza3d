> [Back to index (https://github.com/rzeronte/brakeza3d/blob/master/doc/00-index.md)

# Sistema de shading

Brakeza3D utiliza GLSL como lenguaje de shading. En la carpeta ``GLSL`` podrás encontrar todos 
los shaders existentes.

Shaders de objetos con geometría 3D:

- `Render` (*/GLSL/Render*): Dibuja un objeto en pantalla con texturas e iluminación.
- `Outliner` (*/GLSL/Outliner*): Outliner para un objeto.
- `Color` (*/GLSL/Color*): Dibuja un objeto en pantalla de un color.
- `Wireframe` (*/GLSL/Wireframe*): Dibuja los triangulos de un objeto en pantalla.
- `Points` (*/GLSL/Points*): Dibuja los vertices de un objeto en pantalla.

Shaders de objetos 2D:

- `Image` (*/GLSL/Image*): Dibuja una imagen en pantalla.
- `Line` (*/GLSL/Line*): Dibuja una línea en pantalla.

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
