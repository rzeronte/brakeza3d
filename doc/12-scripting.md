> [Back to index](https://github.com/rzeronte/brakeza3d/blob/master/doc/00-index.md)

- [Sistema de scripting y ciclo de vida de los objetos](#sistema-de-scripting-y-ciclo-de-vida-de-los-objetos)
- [Ciclo de vida de un objeto](#ciclo-de-vida-de-un-objeto)
- [Scripts LUA](#scripts-lua)
- [Vinculación de scripts](#vinculación-de-scripts)
- [Variables globales](#variables-globales)
- [Variables locales](#variables-locales)
- [Gestión de escenas](#gestión-de-escenas)
- [Ejemplos en código](#ejemplos-en-codigo)
---

# Sistema de scripting

Brakeza3D incorpora un sistema de scripting utilizando LUA como lenguaje.

Ya sea desde la UI o desde código, podrás asociar ``scripts`` a la ``escena`` y/o
``objetos``.

---
 
## Sistema de scripting y ciclo de vida de los objetos

En el búcle principal de Brakeza3D, se ejecutan una serie de acciones constantemente. Una de ellas
es el sistema de scripting. Este sistema puede estar en ON/OFF/PAUSE.

Si está en ON, los objetos ejecutarán su ciclo de vida implementado en los scripts.

El sistema de scripting puede ser ejecutado (PLAY), para iniciar la ejecución de los scripts.

También puede ser rebobinado (REWIND) al inicio, para volver a ejecutar los scripts desde su inicio.

Finalmente, podrá ser detenido (STOP) para evitar que el sistema de scripting continue.

También podrás resetear (RELOAD) el estado para volver al inicio los scripts y para refrescarlos desde disco.

### Ciclo de vida de un objeto

Los objetos disponen de su propio ciclo de vida el cual debemos de entender para trabajar con nuestros objetos cargados, es el siguiente:

- **onStart**: Instante en que se inicia la ejecución. Cuando activamos el sistema de scripting (PLAY)

- **onUpdate**: Instante actual (siempre y cuando el sistema de scripting esté ON)

- **onEnd**: Instante en el que se detiene la ejecución (STOP)

Una template básica de script LUA para Brakeza3D sería:

```
function onStart()
    -- código a ejecutar al inicio del sprint, una única vez.
end

function onUpdate()
    -- código a ejecutar en cada frame.
end
```

### Scripts LUA

Los scripts de LUA son elementos que podremos vincular a elementos del sistema. En ellos implementaremos la lógica y comportamiento de objetos en Brakeza3D.

Podemos diferenciar dos grandes tipos de scripts:

- **Scripts de Objeto**: Se asocian a **objetos**. Un mismo script puede vincularse a multitud de objetos.
- **Scripts globales**: Se asocian a la **escena**, no a ningún objeto específico, son de caracter general.

---

### Vinculación de scripts

Podrás realizar estas operaciones desde la UI con el ratón y drag/drop, sin embargo, 
en ocasiones necesitarás poder hacerlo dinámicamente desde código.

Disponemos de un objeto ``ScriptLUA`` que encapsula la lógica de carga de un script.


Los `Object3D` disponen de un método `attachScript` que nos permitirá vincularlos:

```
    print("Load LightPoint3D")
    lightp = LightPoint3D.create(Vertex3D.new(5, -10, 5))
    brakeza:addObject3D(lightp, 'myLightPoint')

    script = ScriptLUA.create("../../scripts/MoveForwardObject.lua")
    if script ~= nil then
        lightp:attachScript(script)
    end
```

De igual forma el ``ComponentRender`` dispone de ``addLUAScript`` que te permitirá vincular
un script a la escena:

```
    script = ScriptLUA.create("../../scripts/global_script.lua")
    if script ~= nil then
        componentsManager:getComponentRender():addLUAScript(script)
    end
```

### Variables globales

---

### Variables locales

---

### Gestión de escenas

---


# Ejemplos en codigo

### Ejemplos de scripts globales
 
- Cargar un modelo 3D https://github.com/rzeronte/brakeza3d/blob/master/doc/examples/load_object_example.md
- Cargar una escena https://github.com/rzeronte/brakeza3d/blob/master/doc/examples/load_scene_example.md
- Control de teclado y ratón https://github.com/rzeronte/brakeza3d/blob/master/doc/examples/keyboard_control_example.md
- Buscar un objeto https://github.com/rzeronte/brakeza3d/blob/master/doc/examples/find_object_example.md
- Variables globales https://github.com/rzeronte/brakeza3d/blob/master/doc/examples/global_variable_example.md

### Ejemplos de scripts de objeto
 
- La variable `this` https://github.com/rzeronte/brakeza3d/blob/master/doc/examples/this_object_example.md
- Detección de colisiones https://github.com/rzeronte/brakeza3d/blob/master/doc/examples/collision_detection_example.md
- Variables de objeto https://github.com/rzeronte/brakeza3d/blob/master/doc/examples/object_variable_example.md

