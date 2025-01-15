> [Back to index](https://github.com/rzeronte/brakeza3d/blob/master/doc/00-index.md)

# Manipulación de un objeto mediante `this`

En otros ejemplos hemos buscado un `Object` por nombre desde `script globales` para manipularlo posteriormente, sin
enmargo, en los `scripts de objeto` dispondremos de una variable que **automáticamente nos dará acceso al propio objeto** al 
que el script está vinculado. Esta es la variable `this`.

A continuación un ejemplo de su uso:

```
function onUpdate()
    ...
    print("Hi I am " .. this:getLabel())
    ...
end

```