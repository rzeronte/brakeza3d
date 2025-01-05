# Ejemplo de manipulación de un objeto

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