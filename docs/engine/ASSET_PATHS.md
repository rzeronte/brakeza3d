# Assets de proyecto y assets del core (AssetPaths)

Estado: **fases 1 y 2 implementadas (2026-11-16)**: el mecanismo existe en el motor y no cambia nada mientras ningún proyecto declare `assetsFolder`.

## Idea

Cada proyecto puede tener su **propia carpeta de assets**, con la misma estructura por tipo que `assets/` (`models/`, `images/`, `sounds/`,
`scripts/`, `scenes/`, `ui/`...), y reutilizar lo del **core** del motor sin copiarlo. Un recurso del proyecto puede además **sobrescribir** uno del
core con el mismo nombre.

Los ficheros de contenido siguen escribiendo rutas literales `../assets/<tipo>/<fichero>` (relativas al ejecutable): **no hay que reescribirlas**.

## Cómo se resuelve

`AssetPaths::Resolve(ruta)` (`include/Misc/AssetPaths.h`, `src/Misc/AssetPaths.cpp`):

1. Sin proyecto con assets propios (o ruta que no cuelga de `../assets/`): devuelve la ruta **tal cual** (comportamiento anterior).
2. Con proyecto activo: si existe `<assetsFolder><tipo>/<fichero>` devuelve esa; si no, devuelve la original (el **core**).
3. Tolera `//` tras el prefijo (`../assets//sounds/x.wav`), cachea las comprobaciones de existencia y es seguro desde los hilos de carga.

## Cómo activarlo en un proyecto

En el JSON del proyecto (`assets/projects/<Nombre>Project.json`), clave opcional:

```json
{ "name": "MyGame", "assetsFolder": "../projects/MyGame/assets/", ... }
```

`ProjectLoader::LoadProject` la lee **antes** de cargar scripts y escenas del proyecto y `SaveProject` la conserva. Sin la clave, el proyecto
usa solo el core.

## Dónde está enganchada

| Punto | Qué hace |
|---|---|
| `Tools::FileExists`, `Tools::ReadFile` | resuelven la ruta antes de abrirla: escenas, proyectos, scripts `.lua` y `loadJSON` de Lua (79 usos) |
| `ProjectLoader::LoadProject` / `SaveProject` | fijan y conservan `assetsFolder`; sincronizan el prefijo del core con `Config::ASSETS_FOLDER` |
| Modelos | `Mesh3D`, `FBXLightLoader` (ifstream + Assimp), `ColliderMesh3DAnimation` |
| Imágenes | el cargador de `Tools` (`IMG_Load`), `UIManager::parseWidgetJSON` (widgets) |
| Audio | `Sound3D::DecodeFile`, `SoundPackage` |
| Fuentes | `ComponentWindow`, `ObjectFactory`, `UIManager` (`TTF_OpenFont`) |
| Vídeo | `VideoPlayer` (`avformat_open_input`) |
| Otros | `MeshInstances` (instancias BZIN), binding Lua de escenas, `ShaderBaseOpenGL` / `ShaderBaseCompute` (shaders) |
| Lua | `io.open` envuelto en `ComponentScripting::InitLUATypes` (resuelve la ruta); `Assets.resolve(ruta)` y `Assets.projectRoot()` para scripts; `package.path` (`require`) mira antes `<assetsFolder>scripts/` (`UpdateScriptSearchPath`) |

Resueltos también (2026-11-16, tras mover los assets de un proyecto a su propia carpeta): la lectura de FBX en `Mesh3DAnimation` (`AssimpLoadAnimation`) y el fichero de tipos de los shaders de código (`ShaderBaseCustomOGLCode::ParseTypesFromFileAttributes`: si `fileTypes` ya empieza por `../` no se le antepone `../GLSL/`, que impedía reconocer el prefijo `../assets/`).

**Texturas con ruta absoluta en el FBX.** Los FBX exportados pueden guardar la textura como ruta absoluta del equipo de exportación (`C:/.../assets/textures/X.png`, con barras invertidas en Windows). `Mesh3D::AssimpInitMaterials` añade un candidato: lo que sigue a la última `assets/` se busca bajo `Config::ASSETS_FOLDER` (proyecto primero, core de respaldo). Es una red de seguridad; lo correcto es exportar con rutas relativas.

Escrituras (`fopen(..., "w")` de los editores, `ofstream`) NO se resuelven: se escribe donde diga la ruta.
