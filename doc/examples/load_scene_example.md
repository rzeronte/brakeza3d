# Ejemplo de carga de una escena

Cargar una escena será una operación muy habitual cuando deseemos cargar niveles para nuestro juego.

Para ello haremos uso de un "script global", que vincularemos a la escena activa para su futura ejecución.

El código para cargar una escena es el siguiente:

```
function onStart()
    ...
    componentsManager:getComponentRender():getSceneLoader():loadScene("../scenes/scene_example.json")
    ...
end
```

Si el fichero JSON proporcionado es una escena bien formada, encontraremos el siguiente mensaje en la pestaña de logs:
```
Loading scene: %s"
```


# Explicación del código

Lo primero, observamos que situamos nuestro código en la función `onStart`. Esto ejecutará el código solo una vez.

Usaremos el objeto global `componentsManager` para obtener el `componentRender`. Este último, nos ofrece el método `loadScene`, cuyo único argumento es la ruta al fichero de escena.

La función `loadScene` eliminará del mundo cualquier objeto existente, por lo que la carga de la escena siempre se producirá sobre un mundo totalmente vacío.