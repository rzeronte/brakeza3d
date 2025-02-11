function onStart()
end

function onUpdate()
    render = componentsManager:getComponentRender()
    customShader = render:getSceneShaderByLabel("Custom")

    if customShader ~= nil then
        customShader:setDataTypeValue("r_factor", math.sin(brakeza:getExecutionTime()))
        customShader:setDataTypeValue("a_factor", math.cos(brakeza:getExecutionTime()))
        customShader:setDataTypeValue("g_factor", math.cos(brakeza:getExecutionTime()))
    else
        print("Error: Not custom shader called: 'Custom'")
    end
end

function onCollision(with)
end
