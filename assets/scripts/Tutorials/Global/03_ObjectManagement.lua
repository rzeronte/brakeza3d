function onStart()
    textWriter = ObjectFactory.TextWriter("../assets/fonts/Courier.ttf", 2)
    createdObjects = {}
    nameCounter = 0
    print("Object Management tutorial loaded!")
end

function onUpdate()
    local input = Components:Input()

    if input:isCharFirstEventDown("C") then
        createRandomObject()
    end

    if input:isCharFirstEventDown("X") then
        deleteLastObject()
    end

    if input:isCharFirstEventDown("V") then
        toggleLastObjectVisibility()
    end

    -- RENDER INFO
    local screenW = Components:Window():getWidth()
    local screenH = Components:Window():getHeight()
    local centerY = 100

    textWriter:writeTextTTFAutoSize(screenW/2 - 150, centerY, "=== OBJECT MANAGEMENT ===", Color.new(1, 1, 0, 1), 1.2)
    textWriter:writeTextTTFAutoSize(screenW/2 - 180, centerY + 40, "C = Create random light", Color.new(0.7, 0.7, 0.7, 1), 1.0)
    textWriter:writeTextTTFAutoSize(screenW/2 - 180, centerY + 70, "X = Delete last object", Color.new(0.7, 0.7, 0.7, 1), 1.0)
    textWriter:writeTextTTFAutoSize(screenW/2 - 180, centerY + 100, "V = Toggle visibility", Color.new(0.7, 0.7, 0.7, 1), 1.0)
    textWriter:writeTextTTFAutoSize(screenW/2 - 80, centerY + 140, "Objects: " .. objectCount, Color.new(0, 1, 0, 1), 1.0)
end

function createRandomObject()
    nameCounter = nameCounter + 1
    local uniqueName = "TutorialObj_" .. nameCounter
    local x = math.random(-5, 5)
    local y = math.random(0, 3)
    local z = math.random(-5, 5)

    local pos = Vertex3D.new(x, y, z)
    local ambient = Color.new(0.1, 0.1, 0.1, 1)
    local diffuse = Color.new(math.random(), math.random(), math.random(), 1)
    local specular = Color.new(1, 1, 1, 1)

    local newObject = ObjectFactory.LightPoint(pos, ambient, diffuse, specular)
    newObject:setName(uniqueName)
    newObject:setBelongToScene(true)
    Brakeza:AddObject3D(newObject, uniqueName)

    table.insert(createdObjects, uniqueName)
    objectCount = objectCount + 1
    print("Created: " .. uniqueName)
end

function deleteLastObject()
    if #createdObjects > 0 then
        local objectName = table.remove(createdObjects)
        local obj = Brakeza:getObjectByName(objectName)
        if obj then
            obj:setRemoved(true)
            objectCount = objectCount - 1
            print("Deleted: " .. objectName)
        end
    end
end

function toggleLastObjectVisibility()
    if #createdObjects > 0 then
        local objectName = createdObjects[#createdObjects]
        local obj = Brakeza:getObjectByName(objectName)
        if obj then
            local current = obj:isEnabled()
            obj:setEnabled(not current)
            print("Toggled: " .. objectName)
        end
    end
end

function onEnd()
    for _, name in ipairs(createdObjects) do
        local obj = Brakeza:getObjectByName(name)
        if obj then obj:setRemoved(true) end
    end
end
