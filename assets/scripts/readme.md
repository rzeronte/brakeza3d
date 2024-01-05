mensaje = this:getTypeObject()
RenderLayersToMain = this:getPosition()
hola = this:isStucked()
tiempo = brakeza:getExecutionTime()

camera = componentsManager:getComponentCamera():getCamera()

position = camera:getPosition()

left = componentsManager:getComponentInput():isClickLeft()

if (componentsManager:getComponentInput():isCharPressed("A")) then
print("pulsada!")
end

if (componentsManager:getComponentInput():isKeyEventDown()) then
print("Down2!")
end

if (componentsManager:getComponentInput():isKeyEventUp()) then
print("Up1!")
end

print("DeltaTime: " .. tonumber(tiempo))
