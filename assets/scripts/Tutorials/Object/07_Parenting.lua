-- ============================================================================
-- Tutorial 07: Object Parenting (Hierarchies)
-- Type: Object Script
-- ============================================================================
-- Learn how to create parent-child relationships between objects.
-- When a parent moves/rotates, children follow automatically.
-- ============================================================================

local childObjects = {}
local orbitAngle = 0
local childCount = 4
local orbitSpeed = 2.0

function onStart()
    print("Parenting script attached to: " .. this:getName())
    print("This object will be the PARENT.")

    createChildObjects()
end

function createChildObjects()
    local childPositions = {
        Vertex3D.new(2, 0, 0),
        Vertex3D.new(-2, 0, 0),
        Vertex3D.new(0, 2, 0),
        Vertex3D.new(0, 0, 2),
    }

    for i, offset in ipairs(childPositions) do
        if i <= childCount then
            local childName = Brakeza:uniqueObjectLabel("Child")

            local parentPos = this:getPosition()
            local childPos = Vertex3D.new(
                parentPos.x + offset.x,
                parentPos.y + offset.y,
                parentPos.z + offset.z
            )
            local ambient = Color.new(0.1, 0.1, 0.1, 1)
            local diffuse = Color.new(math.random(), math.random(), math.random(), 1)
            local specular = Color.new(1, 1, 1, 1)

            local child = ObjectFactory.LightPoint(childPos, ambient, diffuse, specular)
            child:setName(childName)
            child:setConstant(1.0)
            child:setLinear(0.3)
            child:setCuadratic(0.1)

            child:setBelongToScene(true)
            Brakeza:AddObject3D(child)

            -- ========================================
            -- SET PARENT RELATIONSHIP
            -- ========================================
            child:setParent(this)

            table.insert(childObjects, {object = child, offset = offset})
            print("Created child: " .. childName)
        end
    end

    print("Total children: " .. #childObjects)
end

function onUpdate()
    local dt = Brakeza:getDeltaTime()
    local input = Components:Input()

    -- ========================================
    -- MOVE PARENT WITH WASD
    -- ========================================
    local speed = 3.0 * dt

    if input:isCharPressed("W") then
        this:addToPosition(Vertex3D.new(0, 0, -speed))
    end
    if input:isCharPressed("S") then
        this:addToPosition(Vertex3D.new(0, 0, speed))
    end
    if input:isCharPressed("A") then
        this:addToPosition(Vertex3D.new(-speed, 0, 0))
    end
    if input:isCharPressed("D") then
        this:addToPosition(Vertex3D.new(speed, 0, 0))
    end

    -- ========================================
    -- ROTATE PARENT WITH Q/E
    -- ========================================
    if input:isCharPressed("Q") then
        rotateY(-dt)
    end
    if input:isCharPressed("E") then
        rotateY(dt)
    end

    -- ========================================
    -- ANIMATE CHILD ORBITS WITH O
    -- ========================================
    if input:isCharPressed("O") then
        orbitAngle = orbitAngle + dt * orbitSpeed
        updateChildOrbits()
    end

    -- ========================================
    -- DETACH/ATTACH WITH SPACE
    -- ========================================
    if input:isCharFirstEventDown("SPACE") then
        toggleParenting()
    end

    -- ========================================
    -- PRINT INFO WITH I
    -- ========================================
    if input:isCharFirstEventDown("I") then
        printChildInfo()
    end
end

function rotateY(angle)
    local rot = this:getRotation()
    local delta = M3.new()
    delta:setMatrixRotationForEulerAngles(0, angle, 0)
    this:setRotation(delta * rot)
end

function updateChildOrbits()
    local parentPos = this:getPosition()

    for i, childData in ipairs(childObjects) do
        local child = childData.object
        local baseOffset = childData.offset

        local angle = orbitAngle + (i - 1) * (math.pi * 2 / #childObjects)
        local radius = math.sqrt(baseOffset.x^2 + baseOffset.y^2 + baseOffset.z^2)

        local newX = parentPos.x + math.cos(angle) * radius
        local newY = parentPos.y + baseOffset.y
        local newZ = parentPos.z + math.sin(angle) * radius

        child:setPosition(Vertex3D.new(newX, newY, newZ))
    end
end

function toggleParenting()
    if #childObjects > 0 then
        local firstChild = childObjects[1].object
        local currentParent = firstChild:getParent()

        if currentParent then
            for _, childData in ipairs(childObjects) do
                childData.object:setParent(nil)
            end
            print("Children detached!")
        else
            for _, childData in ipairs(childObjects) do
                childData.object:setParent(this)
            end
            print("Children attached!")
        end
    end
end

function printChildInfo()
    print("=== Parent-Child Info ===")
    print("Parent: " .. this:getName())
    print("Attached children count: " .. #childObjects)
end

function onEnd()
    for _, childData in ipairs(childObjects) do
        childData.object:setRemoved(true)
    end
end
