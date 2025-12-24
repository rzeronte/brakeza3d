function onStart()
end

function onUpdate()
    customShader = Components:Render():getSceneShaderByLabel("Custom")

    if customShader ~= nil then
        customShader:setDataTypeValue("r_factor", math.sin(Brakeza:getExecutionTime()))
        customShader:setDataTypeValue("a_factor", math.cos(Brakeza:getExecutionTime()))
        customShader:setDataTypeValue("g_factor", math.cos(Brakeza:getExecutionTime()))
    else
        print("Error: Not custom shader called: 'Custom'")
    end
end

function onCollision(with)
end
