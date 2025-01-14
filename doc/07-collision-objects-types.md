# Objetos colisionables principales

Los objetos colisionables permitirán al programador implementar lógica en función de las colisiones 
producidas.

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
## Objetos colisionables

Brakeza3D incluye varios tipos de objeto para colisiones:

- `Mesh3DGhost`: Modelo 3D con colisiones sin reacción a físicas.
- `Mesh3DBody`: Modelo 3D con colisiones con reacción a físicas.
- `Mesh3DAnimationGhost`: Modelo 3D animado sin reacción a físicas.
- `BillboardAnimationBody`: Billboard animado con reacción a físicas.
- `BillboardAnimation8DirectionsBody`: Billboard 8 direcciones con reacción a físicas.
- `Projectile3DBody`: Modelo 3D reactivo a físicas. Diseñado específicamente para projectiles.

