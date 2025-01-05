# Ejemplo de control del teclado

El encargado de ofrecernos acceso al teclado es el `componentInput`. A través de este componente tendremos acceso a diversas funciones para implementar nuestra propia lógica.


```
function onUpdate()
    left = componentsManager:getComponentInput():isClickLeft()
    
    if (left)
        print("Click Left!")
    end
    
    if (componentsManager:getComponentInput():isCharPressed("A")) then
        print("Key A pressed!")
    end
    
    if (componentsManager:getComponentInput():isKeyEventDown()) then
        print("Key event down!")
    end
    
    if (componentsManager:getComponentInput():isKeyEventUp()) then
        print("Key event up!")
    end
end

```