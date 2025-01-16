>[Back to index](https://github.com/rzeronte/brakeza3d/blob/master/doc/00-index.md)

# Iluminación global

Brakeza3D incluye una `luz por defecto` en la escena. Puedes verlo como *el sol*,

Al fin y al cabo, es una luz direccional, por lo que podrás ajustar algunos de sus
parámetros para conseguir la luz base deseada.

Al ser una luz global, **carece de posición**.

Puedes manipular las componentes de la iluminación global mediante la UI o a través
de tus scripts LUA.

Se configura mediante el `componente render`:

```
    render = componentsManager:getComponentRender()
```

- `setGlobalIlluminationDirection(Vertex3D)`: `Dirección` de iluminación global.
- `setGlobalIlluminationAmbient(Vertex3D)`: Componente `ambiente` de iluminación global.
- `setGlobalIlluminationDiffuse(Vertex3D)`: Componente `difuso` de iluminación global.
- `setGlobalIlluminationSpecular(Vertex3D)`: Componente `especuilar` de de iluminación global.

`Nota`: Las componentes ambiente, difusa y especular, las manipulamos a través del tipo `Vertex3D` y no a través de colores, 
simplemente por conveniencia, ya que se esperan valores de rango `0-1` y no `0-255` como hace un `Color`.

Ejemplo para configurar la componente `ambiente`:

```
    render = componentsManager:getComponentRender()
    render:setGlobalIlluminationAmbient(Vertex3D.new(1, 0, 0)) -- rojo
```