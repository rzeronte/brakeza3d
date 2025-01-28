>[Back to index](https://github.com/rzeronte/brakeza3d/blob/master/doc/00-index.md)

- [Objetos colisionables](#objetos-colisionables-principales)
- [Modos de colisión](#modos-de-colisión)
- [Forma del colisionador](#forma-del-colisionador)
- [Desactivar colisiones](#desactivar-colisiones)
- [Modo de colisión KINEMATIC](#modo-de-colisión-kinematic)

---

# Objetos colisionables

Los objetos colisionables permitirán al programador implementar lógica en función de las colisiones 
producidas.

En Brakeza3D todos los `Object3D` puede trabajar con físicas y colisiones.

Cualquier objeto colisionables lanzará una llamada al método `onCollision` de tus scripts LUA por cada colisión producida.

```lua
function onCollision(with)
    print("Collision with " .. with:getLabel())
end
```

## Modos de colisión

Existen dos modos de funcionamiento para el sistema de colisiones:

- `GHOST`: Objeto colisionable no reactivo a físicas.
- `RIGIDBODY`: Objeto colisionable reactivo a físicas.

Puedes configurar el modo de las colisiones de un objeto desde la GUI o desde tus scripts LUA.

Para hacerlo mediante tus scripts puedes utilizar el método ``setupGhostCollider`` o
``setupRigidBodyCollider``.

```lua
    eye = Mesh3D.create(Vertex3D.new(0, 0, 10), "../assets/models/eye.fbx")
    eye:setCollisionsEnabled(true)
    eye:setupGhostCollider(CollisionShape.SIMPLE_SHAPE); -- Ghost
    brakeza:addObject3D(eye, 'myOneEye')

    eye2 = Mesh3D.create(Vertex3D.new(0, 0, 10), "../assets/models/eye.fbx")
    eye2:setCollisionsEnabled(true)
    eye2:setupRigidBodyCollider(CollisionShape.SIMPLE_SHAPE); -- RigidBody
    brakeza:addObject3D(eye2, 'myTwoEye')
```

En modo de colisión ``RIGIDBODY`` podemos indicarle si la geometría será **estática**. Los elementos estáticos no
se moverán (su masa es automáticamente 0). Es el único modo de colisión permitido para geometrías no convexas.

```lua
    eye2 = Mesh3D.create(Vertex3D.new(0, 0, 10), "../assets/models/eye.fbx")
    eye2:setColliderStatic(true) -- estático!
    eye2:setupRigidBodyCollider(CollisionShape.SIMPLE_SHAPE); -- RigidBody
    brakeza:addObject3D(eye2, 'myTwoEye')
```
 
## Forma del colisionador

Cualquier ``Object3D`` puede trabajar con la forma de colisionador denominada `CollisionShape.SIMPLE_SHAPE`.
**Es el modo por defecto**. La forma de colisión será un paralelogramo de un tamaño configurable.
Su performance es muy buena.

No obstante, todos aquellos objetos que dependen de `Mesh3D` podrán crearse con una forma de colisionador 
denominada `CollisionShape.TRIANGLE3D_MESH_SHAPE` creando una una malla de colisión exacta a la geometría del modelo. Es más precisa, pero conlleva un mayor costo
  computacional.

Puedes manipular los modos de colisión tanto desde la GUI como desde tus scripts LUA.

```lua
    eye = Mesh3D.create(Vertex3D.new(0, 0, 10), "../assets/models/eye.fbx")
    eye:setCollisionsEnabled(true)
    eye:setupGhostCollider(CollisionShape.TRIANGLE3D_MESH_SHAPE); -- Ghost
    brakeza:addObject3D(image3d, 'myOneEye')
```

---

## Desactivar colisiones

Puedes desactivar las colisiones para un objeto con el método ``setCollisionsEnabled``. Esto eliminará cualquier forma
de colisión configurada previamente.

---

## Movimiento en objetos colisionadores

Tanto los ``GHOST`` como los ``RIGIDBODY`` pueden ser movidos por la escena.

- `setLinearVelocity(Vertex3D)`

## Aplicar fuerzas a objetos rigidos

Podemos aplicar fuerzas a los objetos ``RIGIDBODY`` gracias a los siguientes métodos:

- `applyCentralImpulse(Vertex3D f)`
- `applyCentralForce(Vertex3D f)`
- `applyImpulse(Vertex3D f, Vertex3D r)`

## Modo de colisión KINEMATIC
