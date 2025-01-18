> [Back to index](https://github.com/rzeronte/brakeza3d/blob/master/doc/00-index.md)

- [Sistema de scripting y ciclo de vida de los objetos](#sistema-de-scripting-y-ciclo-de-vida-de-los-objetos)
- [Ciclo de vida de un objeto](#ciclo-de-vida-de-un-objeto)
- [Scripts LUA](#scripts-lua)
- [Vinculación de scripts](#vinculación-de-scripts)
- [Variables](#variables)
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

### Variables globales

Las variables definidas en scripts vinculadas a ``escenas`` serán globales.

Podrás acceder directamente a las variables globales desde cualquier otro
script.

```
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

```
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

```
function onStart()
    ...
    componentsManager:getComponentRender():getSceneLoader():loadScene("../scenes/scene_example.json")
    ...
    componentsManager:getComponentRender():getSceneLoader():saveScene("../scenes/scene_example.json")
    ...
end
```

---

### Ejemplos en codigo

https://github.com/rzeronte/brakeza3d/blob/master/doc/13-examples-lua-code.md
