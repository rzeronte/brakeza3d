# Objetos 2D principales

Brakeza3D permite incorporar a la pantalla elementos `2D` o `2.5D`


- `Image2D`: Es un objeto `2D`. Dibuja una imagen en pantalla.  


- `Image2DAnimation`: Dibuja una animación en pantalla, es en esencia, una colección de `Image2D`.

---

- `BillboardAnimation`: Es un objeto `2.5D`. El billboard es un plano 3D que siempre mira a cámara. 
Podrás moverlo, y alejarte de el en el espacio 3D.
En dicho plano se dibujará la animación indicada según la configuración deseada.


- `BillboardAnimation8Directions`: Es un tipo de `Billboard animation` en el que se almacenan *ocho direcciones*.
**Según el ángulo a cámara, el billbaord cambiará su animación automáticamente.**
Su comportamiento es similar a lo que vemos en los enemigos de mítico 'Doom'.


---
