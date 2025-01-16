> [Back to index](https://github.com/rzeronte/brakeza3d/blob/master/doc/00-index.md)

# Tipos de objetos de luz

Todos los tipos de luz son ``Object3D``, por lo que su movimiento y rotación es similar a cualquier otro objeto.

- [LightPoint3D](#lightpoint3d)
- [SpotLight3D](#spotlight3d)

---
## LightPoint3D

Punto de luz que emite en todas las direcciones.

Un punto de luz dispone de algunos atributos que afectarán como la luz funciona:

### Componentes ADS

- `Componente ambiente (vec3)`: Intensidad de color de la luz ambiente sobre el objeto.
- `Componente difuso (vec)`: Intensidad de color de la luz sobre la textura.
- `Componente especular (dev)`: Intensidad de color de reflejos especulares.

Podrás manipular estos atributos desde la GUI o desde tus scripts LUA. Para hacerlo desde tus scripts,
podrás usar los métodos:

- `setAmbient(Color)`
- `setColor(Color)`
- `setColorSpecular(Color)`

### Constantes

- `Constant (float)`
- `Constante Linear (float)`
- `Constante Quadratic (float)`

También podrás manipular estos atributos desde la GUI o desde tus scripts LUA. Para hacerlo desde tus scripts,
podrás usar los métodos:

- `setConstant(float)`
- `setLinear(float)`
- `setCuadratic(float)`

Ejemplo de instanciación desde script LUA:

```
    -- LightPoint3D.create(position)
    lightp = LightPoint3D.create(Vertex3D.new(5, 5, 5))
    brakeza:addObject3D(lightp, 'myLightPoint')
```
## SpotLight3D

Punto de luz que emite en una dirección dada. Es básicamente un *foco* de luz.

El objeto `SpotLight3D` hereda directamente de `LightPoint3D`, por lo que posee todas
sus propiedades, sin embargo, añade algunas importantes:

- `direction`: `Vertex3D` con la dirección en la que apunta el foco
- `cut`: `float` Constante de tamaño (tamaño del foco)
- `cutOff`: `float` Constante de corte de luz (difusión del borde del foco)

Podrás manipular dichos valores mediante los métodos:

- `setDirection(Vertex3D)`
- `setCut(float)`
- `setCutOff(float)`

Ejemplo de instanciación desde script LUA:

```
    -- SpotLight3D.create(position, direction)
    lights = SpotLight3D.create(Vertex3D.new(5, -10, 5), Vertex3D.new(0, 0, 1))
    lights:setColor(Color.new(255, 255, 0, 255))
    brakeza:addObject3D(lights, 'mySpotLight')
```

