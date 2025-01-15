>[Back to index](https://github.com/rzeronte/brakeza3d/blob/master/doc/00-index.md)

# Objetos colisionables principales

Los objetos colisionables permitirán al programador implementar lógica en función de las colisiones 
producidas.

---

## Objetos colisionables base

Existen dos tipos de objetos base colisionables:

- `Ghost`: Objeto colisionable pero no reactivo a físicas.
- `Body`: Objeto colisionable, reactivo a físicas.

Los objetos colisionables, lanzarán una llamada al método `onCollision` por cada colisión producida.

```
function onCollision(with)
    print("Collision with " .. with:getLabel())
end
```

Ámbos tipos, disponen de dos modalidades de colisiones:
 
 - `SIMPLE_SHAPE`: **Es el modo por defecto**. La forma de colisión será un cubo de un tamaño dado por un `Vertex3D`. Su performance es muy buena.
 - `TRIANGLE3D_MESH_SHAPE`: Se crea una malla de colisión exacta a la geometría del modelo. Es más precisa, pero conlleva un mayor costo
computacional.

Puedes manipular los modos de colisión tanto desde la GUI como desde tus scripts LUA.

---

## Tipos de objetos colisionables

Brakeza3D incluye varios tipos de objeto para colisiones:
 
- [Mesh3DGhost](#mesh3dghost)
- [Mesh3DBody](#mesh3dbody)
- [Mesh3DAnimationGhost](#mesh3danimationghost)
- [BillboardAnimationBody](#billboardanimationbody)
- [BillboardAnimation8DirectionsBody](#billboardanimation8directionsbody)
- [Projectile3DBody](#projectile3dbody)

---

### Mesh3DGhost

Modelo 3D con colisiones sin reacción a físicas.

Si dos objetos colisionan, se **lanzará el evento `onCollision`**, pero **ambos objetos se traspasarán libremente**.

```
    -- Mesh3DGhost.create(position, modelFile)
    ghost = Mesh3DGhost.create(Vertex3D.new(0, -10, 40), "../assets/models/planet_cube_02.fbx")
```

### Mesh3DBody

Modelo 3D con colisiones y reacción a físicas. Incorpora un atributo importante, `mass`, es decir, el peso del objeto.

```
    -- Mesh3DBody.create(position, mass, modelFile)
    meshbody = Mesh3DBody.create(Vertex3D.new(10, 0, 10), 1, "../assets/models/planet_cube_02.fbx")
```

### Mesh3DAnimationGhost
Modelo 3D animado sin reacción a físicas.

### BillboardAnimationBody
Billboard animado con reacción a físicas.

### BillboardAnimation8DirectionsBody
Billboard 8 direcciones con reacción a físicas.

### Projectile3DBody
Modelo 3D reactivo a físicas. Diseñado específicamente para projectiles.

