# Objetos 2D principales

Brakeza3D permite incorporar a la pantalla elementos `2D` o `2.5D`

- [Image2D](#image2d)
- [Image2DAnimation](#image2danimation)
- [BillboardAnimation](#billboardanimation)
- [BillboardAnimation8Directions](#billboardanimation8directions)

## Image2D

Es un objeto `2D`. Dibuja una imagen en pantalla.  

```
    img = Image2D.create(10, 10, "../assets/images/logo_small.png")
    img:setEnabled(true)
    img:updatePosition(300, 300)
    brakeza:addObject3D(img, 'imagen')
```

## Image2DAnimation

Dibuja una animación en pantalla, es en esencia, una colección de `Image2D`.

Está diseñado para cargar la animación desde formato **spritesheet**, es decir, una imagen con un grid, donde cada celda
es una imagen de la animación. Es por este motivo que deberemos de indicar el **ancho** y **alto** de cada frame y el **nº de frames**
en la imagen, para que la función ajuste adecuadamente el sprite.

Podrás configurar la velocidad (**fps**) a la que se muestra una animación.
```
    -- Image2DAnimation.create(x, y, spriteFile, width, height, numFrames, fps)
    img = Image2DAnimation.create(0, 0, "../assets/sprites/explosion_a.png", 128, 128, 15, 24)
    img:setEnabled(true)
    brakeza:addObject3D(img, 'imagen')
```

---

## BillboardAnimation

Es un objeto `2.5D`. El billboard es un plano 3D que siempre mira a cámara. 

Podrás moverlo, escalarlo, rotarlo, etc.

También está diseñado para cargar la animación desde formato **spritesheet**.  

Fionalmente se dibujará la animación en el plano, según la configuración indicada.

```
    -- BillboardAnimation.create(position, width, height, spriteFile, spriteWidth, spriteHeight, numFrames, fps)
    animation = BillboardAnimation.create(Vertex3D.new(10, 10, 10), 100, 100, "../assets/sprites/explosion_a.png", 128, 128, 15, 24)
    animation:setEnabled(true)
    brakeza:addObject3D(animation, 'my_animation')
```
---

## BillboardAnimation8Directions

Es un tipo de `Billboard animation` en el que se almacenan *ocho direcciones*.

**Según el ángulo a cámara, el billbaord cambiará su animación automáticamente.**
Su comportamiento es similar a lo que vemos en los enemigos de mítico 'Doom'.

Está diseñado para cargar las imágenes de las animaciones desde una carpeta, en la 
que cada fichero debe respetar un patrón: `[direccion]_[frame].png`.

Supongamos que deseamos cargar una animación con dos frames. Estos serían los ficheros esperados:

| Direccion | Frame 0 | Frame 1 | Frame 2 |
|-----------|---------|---------|---------|
| 01        | 1_0.png | 1_1.png | 1_2.png |
| 02        | 2_0.png | 2_1.png | 2_2.png |
| 03        | 3_0.png | 3_1.png | 3_2.png |
| 04        | 4_0.png | 4_1.png | 4_2.png |
| 05        | 5_0.png | 5_1.png | 5_2.png |
| 06        | 6_0.png | 6_1.png | 6_2.png |
| 07        | 7_0.png | 7_1.png | 7_2.png |
| 08        | 8_0.png | 8_1.png | 8_2.png |

Así sucesivamente, si nuestra animación tiene más frames.

```
    img = BillboardAnimation8Directions.create("../assets/sprites/explosion", 15, 24)
    img:setEnabled(true)
    brakeza:addObject3D(img, 'imagen')
```

---
