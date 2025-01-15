> [Back to index](https://github.com/rzeronte/brakeza3d/blob/master/doc/00-index.md)

# Ejemplo de detección de colisiones entre objetos

En Brakeza3D existen dos tipos de objetos colisionables: Los `GhostBody` y los `RigidBody`.

Los `GhostBody` son objetos que detectan colisiones, pero no reaccionan a ellas. Es decir, las físicas no afectarán
al cuerpo, que actuará cual 'fantasma' permitiendo que se atreviesen sin problemas.

Por otro lado, los `RigidBody` son objetos si responden físicamente a las colisiones producidas.

Podrás configurar desde la GUI que la malla de colisión sea simple (AABB) o completamente ajustada al modelo.

```
function onCollision(with)
    if with ~= nil then
        print("Collision with " .. with:getLabel())
    else
        print("Collision with unknow object")
    end
end
```
Si vinculamos un script con el código superior en un objeto colisionable, veremos el nombre de otros objetos que colisionen con él.

# Explicación del código

Los objetos colisionables, lanzarán una llamada al método `onCollision` por cada colisión producida. Podremos acceder al objeto
con el que ha colisionado a través de la variable `with` que es de tipo `Object3D`





