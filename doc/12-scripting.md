> [Back to index](https://github.com/rzeronte/brakeza3d/blob/master/doc/00-index.md)

# Sistema de scripting

``ScriptLUA``
``attachScript``


```
    print("Load LightPoint3D")
    lightp = LightPoint3D.create(Vertex3D.new(5, -10, 5))
    lightp:setColor(Color.new(255, 0, 0, 255))
    brakeza:addObject3D(lightp, 'myLightPoint')

    script = ScriptLUA.create("../../scripts/MoveForwardObject.lua")
    if script ~= nil then
        print("Script no es nulo")
        lightp:attachScript(script)
    end
```
