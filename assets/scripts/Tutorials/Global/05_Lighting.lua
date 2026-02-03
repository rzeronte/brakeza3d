function onStart()
    textWriter = ObjectFactory.TextWriter("../assets/fonts/Courier.ttf", 2)
    sunAngle = 0
    ambientIntensity = 0.2

    local pos = Vertex3D.new(0, 3, 0)
    local ambient = Color.new(0.1, 0.1, 0.1, 1)
    local diffuse = Color.new(1.0, 0.8, 0.6, 1)
    local specular = Color.new(1.0, 1.0, 1.0, 1)

    dynamicLight = ObjectFactory.LightPoint(pos, ambient, diffuse, specular)
    dynamicLight:setName("TutorialLight")
    dynamicLight:setConstant(1.0)
    dynamicLight:setLinear(0.09)
    dynamicLight:setCuadratic(0.032)
    dynamicLight:setBelongToScene(true)
    Brakeza:AddObject3D(dynamicLight)

    print("Lighting tutorial loaded!")
end

function onUpdate()
    local dt = Brakeza:getDeltaTime()
    local input = Components:Input()
    local render = Components:Render()

    -- Rotate sun with Q/E
    if input:isCharPressed("Q") then sunAngle = sunAngle - dt * 0.5 end
    if input:isCharPressed("E") then sunAngle = sunAngle + dt * 0.5 end

    local sunX = math.cos(sunAngle)
    local sunY = -0.5
    local sunZ = math.sin(sunAngle)
    render:setGlobalIlluminationDirection(Vertex3D.new(sunX, sunY, sunZ))

    -- Adjust ambient with Z/X
    if input:isCharPressed("Z") then
        ambientIntensity = math.max(0, ambientIntensity - dt * 0.3)
    end
    if input:isCharPressed("X") then
        ambientIntensity = math.min(1, ambientIntensity + dt * 0.3)
    end
    render:setGlobalIlluminationAmbient(Vertex3D.new(ambientIntensity, ambientIntensity, ambientIntensity))

    -- Move light with I/J/K/L
    if dynamicLight then
        local pos = dynamicLight:getPosition()
        local speed = 3.0 * dt
        if input:isCharPressed("I") then dynamicLight:setPosition(Vertex3D.new(pos.x, pos.y, pos.z - speed)) end
        if input:isCharPressed("K") then dynamicLight:setPosition(Vertex3D.new(pos.x, pos.y, pos.z + speed)) end
        if input:isCharPressed("J") then dynamicLight:setPosition(Vertex3D.new(pos.x - speed, pos.y, pos.z)) end
        if input:isCharPressed("L") then dynamicLight:setPosition(Vertex3D.new(pos.x + speed, pos.y, pos.z)) end
    end

    -- RENDER INFO
    local screenW = Components:Window():getWidth()
    local screenH = Components:Window():getHeight()
    local centerY = 150

    textWriter:writeTextTTFAutoSize(screenW/2 - 130, centerY, "=== LIGHTING TUTORIAL ===", Color.new(1, 1, 0, 1), 1.2)
    textWriter:writeTextTTFAutoSize(screenW/2 - 180, centerY + 40, "Q/E = Rotate sun direction", Color.new(0.7, 0.7, 0.7, 1), 1.8)
    textWriter:writeTextTTFAutoSize(screenW/2 - 180, centerY + 70, "Z/X = Decrease/Increase ambient", Color.new(0.7, 0.7, 0.7, 1), 1.0)
    textWriter:writeTextTTFAutoSize(screenW/2 - 180, centerY + 100, "I/K = Move light forward/back", Color.new(0.7, 0.7, 0.7, 1), 1.0)
    textWriter:writeTextTTFAutoSize(screenW/2 - 180, centerY + 130, "J/L = Move light left/right", Color.new(0.7, 0.7, 0.7, 1), 1.0)
    textWriter:writeTextTTFAutoSize(screenW/2 - 180, centerY + 170, "Sun: " .. math.floor(sunAngle*100)/100 .. " | Ambient: " .. math.floor(ambientIntensity*100)/100, Color.new(0, 1, 0, 1), 1.0)
end

function onEnd()
    if dynamicLight then dynamicLight:setRemoved(true) end
end
