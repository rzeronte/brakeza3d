> [Back to index](https://github.com/rzeronte/brakeza3d/blob/master/doc/00-index.md)

# Ejemplo de búsqueda de objeto en el mundo

En multitud de ocasiones necesitaremos encontrar un objeto en nuestro mundo.

Podemos localizar objetos en nuestro mundo por su nombre (que es único).

```
function onUpdate()
    ...
	o = brakeza:getSceneObjectByLabel("MyObject")
	if o ~= nil then
	    o:addToPosition(Vertex3D.new(-2, 0, 0))
	    position = o:getPosition()
	    local msg = "Item found, Position: " .. position.x .. ", " .. position.y .. ", " .. position.z
        print(msg)
    else
        print("Object not found")
	end
    ...
end
```

En este ejemplo, se localiza el objeto de nombre `my_object` y se mueve su posición -2 unidades en el eje X en cada frame.


# Explicación del código

Lo primero, observamos que situamos nuestro código en la función `onUpdate`. Esto ejecutará el código en cada frame.

Usaremos el objeto global `brakeza` que nos ofrece el método `getSceneObjectByLabel`, cuyo único argumento el nombre del objeto a buscar.

Ya que el resultado de `getSceneObjectByLabel` puede ser nulo, debemos encargarnos de que efectivamente tenemos un objeto antes de poder acceder a sus métodods.