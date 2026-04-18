# Brakeza3D — Informe de Mejoras del Motor

> Generado: 2026-04-17. Solo codigo del motor (src/, include/, GLSL/). Librerias de terceros excluidas.

---

## 1. ARQUITECTURA / DISENO

### 1.1 Singleton con puntero crudo — MEDIA
**Archivos:** `include/Brakeza.h`, `include/Components/Components.h`, `include/Render/Profiler.h`
`static Brakeza *instance` con `new Brakeza()` manual. Sin destruccion garantizada, dificulta testing.
**Fix:** Meyers singleton (`static Brakeza& get()`) o inyeccion de dependencias.

### 1.2 God Class — ComponentRender — MEDIA
**Archivos:** `include/Components/ComponentRender.h`, `src/Components/ComponentRender.cpp`
Gestiona 20+ shaders, seleccion de objetos, pipeline deferred, post-procesado y cadenas de shaders.
**Fix:** Extraer `ShaderManager`, `RenderPipeline`, `PostProcessingChain`.

### 1.3 Dependencias circulares — MEDIA
`include/3D/Object3D.h:17` incluye `../GUI/GUI.h`; `include/Components/Component.h:5` incluye `../3D/Object3D.h`.
**Fix:** Forward declarations para tipos GUI; `weak_ptr` en relaciones padre/hijo.

### 1.4 Serializacion sin abstraccion — MEDIA
`JsonByObject` recibe `Object3D*` mutable sin necesitar modificacion.
**Fix:** `const Object3D*` para operaciones de lectura; factory abstracta para serializadores.

---

## 2. CALIDAD DE CODIGO

### 2.1 Punteros crudos generalizados — ALTA
**Archivos:** `src/Components/ComponentRender.cpp:23-49`, `src/Components/Components.cpp:10-32`
21 shaders con `new` sin RAII. `textWriter = new TextWriter(...)` sin `delete` garantizado.
**Fix:** `std::unique_ptr<TextWriter>`, `std::map<std::string, std::unique_ptr<ShaderBaseOpenGL>>`.

### 2.2 Acceso cJSON sin null-check — ALTA
**Archivo:** `src/Serializers/Object3DSerializer.cpp:81,91,110`
`->valuestring` directo sin verificar que la clave exista = UB.
**Fix:** Null-check obligatorio antes de desreferenciar cada `cJSON_GetObjectItem`.

### 2.4 Const-correctness ausente — BAJA
**Archivo:** `include/3D/Object3D.h:101-107`
`enabledPointer()`, `rotationPointer()` devuelven refs mutables a estado interno.
**Fix:** Devolver `const&` o eliminar (ya existen setters).

---

## 3. RENDIMIENTO

### 3.3 getModelMatrix() sin cache — BAJA
**Archivo:** `src/3D/Object3D.cpp:179-188`
Reconstruye `glm::mat4` en cada llamada. **Fix:** Dirty flag + cache.

### 3.4 Sin cache de preprocessing de shaders — BAJA
`ShaderPreProcessor` puede ejecutarse multiples veces por misma fuente.
**Fix:** Cache por ruta de archivo fuente.

---

## 4. PIPELINE DE RENDERING / SHADERS

### 4.1 Framebuffer incompleto no aborta — MEDIA
**Archivo:** `src/Render/PostProcessingManager.cpp:57-62`
Solo imprime error stderr, continua con framebuffer invalido.
**Fix:** Devolver estado de error o lanzar excepcion.

### 4.2 Errores de compilacion de shader no se propagan — MEDIA
**Archivo:** `src/OpenGL/Base/ShaderBaseOpenGL.cpp:115-138`
El error se loguea pero la funcion devuelve void.
**Fix:** `bool CompileShaderToProgramID(...)` retornando exito/fallo.

### 4.3 Sin comprobacion de errores GL tras renders — ALTA
Sin llamadas a `glGetError()` tras comandos de draw. Fallos silenciosos de GPU.
**Fix:** Helper `CheckGLErrors(const char *ctx)` activo solo en DEBUG.

### 4.4 Fugas de estado OpenGL — MEDIA
**Archivo:** `src/Components/ComponentRender.cpp`
Cambios de estado GL sin restauracion RAII. Una excepcion a mitad del render corrompe el estado.
**Fix:** Wrappers RAII: `ScopedFramebufferBind`, `ScopedBlendState`, etc.

---

## 5. THREADING / ASYNC

### 5.1 Posible race condition en ThreadPool — ALTA
**Archivo:** `include/Render/ThreadPool.h:20-28`
`tasks` y `mainThreadCallbacks` son queues compartidas sin garantia de acceso serializado.
**Fix:** `ThreadSafeQueue<T>` con mutex + condition variable propios.

### 5.2 Lua state no es thread-safe — ALTA
**Archivo:** `include/Components/ComponentScripting.h:24`
`sol::state lua` compartido. Si scripts corren en worker threads, acceso sin locks = UB.
**Fix:** Serializar llamadas Lua por main thread, o states thread-local.

### 5.3 waitAll() sin timeout — MEDIA
**Archivo:** `include/Render/ThreadPool.h:66`
Espera infinita si un job nunca termina.
**Fix:** `bool waitAll(int timeoutMs = 5000);`

---

## 6. SERIALIZACION / PERSISTENCIA

### 6.1 Sin versioning de esquema JSON — ALTA
**Archivo:** `src/Serializers/Object3DSerializer.cpp`
Si el esquema cambia, las escenas antiguas se rompen sin migracion posible.
**Fix:** Campo `"version"` + switch de migracion por version.

### 6.2 attachedObjects no se serializa — ALTA
**Archivo:** `src/Serializers/Object3DSerializer.cpp`
El vector de objetos adjuntos se pierde al recargar la escena.
**Fix:** Serializar recursivamente `attachedObjects` en `JsonByObject`.

### 6.3 Sin escritura atomica — MEDIA
**Archivo:** `src/Loaders/SceneLoader.cpp`
Escritura directa sobre el archivo: crash a mitad = corrupcion.
**Fix:** Escribir a `.tmp`, validar, luego `fs::rename`.

---

## 7. SCRIPTING (LUA)

### 7.1 Inicializacion del entorno Lua prematura — ALTA
**Archivo:** `src/3D/Object3D.cpp:15-22`
Constructor de `Object3D` crea `luaEnvironment` llamando a `Scripting()->getLua()` antes de que el componente este listo.
**Fix:** Lazy-init en el primer `AttachScript()`.

### 7.2 Excepciones de script detienen todo — MEDIA
**Archivo:** `src/Misc/ScriptLUA.cpp:115-140`
Un error en un script llama a `StopLUAScripts()` globalmente.
**Fix:** Aislar error por objeto; continuar con los demas.

### 7.3 Settings de Sol2 inconsistentes — BAJA
`SOL_SAFE_REFERENCES 0` y `SOL_SAFE_GETTER 0` sin documentar motivo.

### 7.4 Sin hook onDestroy en scripts — MEDIA
Los scripts no pueden liberar recursos al destruirse el objeto.
**Fix:** Anadir `onDestroy()` al ciclo de vida del scripting.

---

## 8. GESTION DE ERRORES

### 8.1 Fallos de carga de archivo silenciosos — ALTA
**Archivo:** `src/Misc/ScriptLUA.cpp:20-26`
Si el .lua no existe, el script se limpia sin notificar al `Object3D` propietario.

### 8.2 Fallo de carga de malla no reportado — MEDIA
**Archivo:** `src/3D/Mesh3D.cpp:66-71`
`AssimpLoadGeometryFromFile` devuelve void; el llamador no sabe si fallo.

### 8.3 Acceso a indice sin bounds checking — MEDIA
**Archivo:** `src/Components/ComponentRender.cpp:138,144`
`selectedObjects[0]` sin verificar que el vector no este vacio.

---

## Resumen de Prioridades

| Severidad | Cantidad | Categorias principales |
|-----------|----------|----------------------|
| ALTA      | 12       | Memory management, GL errors, thread safety, Lua lifecycle, serialization |
| MEDIA     | 11       | God class, shader pipeline, threading, persistence |
| BAJA      |  6       | Const-correctness, caching, polish |

**Punto de entrada recomendado:** Issues 2.1, 5.2, 6.1 y 7.1 — mayor riesgo de bugs dificiles de reproducir en produccion.
