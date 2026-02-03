-- ============================================================================
-- Tutorial 01: Basic Movement
-- Type: Object Script (attach to any Object3D)
-- ============================================================================
-- Learn how to move objects using different techniques.
-- 'this' refers to the object this script is attached to.
-- ============================================================================

function onStart()
    print("Movement script attached to: " .. this:getName())
end

function onUpdate()
    local dt = Brakeza:getDeltaTime()
    local input = Components:Input()

    -- ========================================
    -- METHOD 1: Direct position setting
    -- ========================================
    -- Get current position
    local pos = this:getPosition()

    -- Move with WASD
    if input:isCharPressed("W") then
        -- Move forward (negative Z in Brakeza3D)
        this:setPosition(Vertex3D.new(pos.x, pos.y, pos.z - speed * dt))
    end
    if input:isCharPressed("S") then
        this:setPosition(Vertex3D.new(pos.x, pos.y, pos.z + speed * dt))
    end
    if input:isCharPressed("A") then
        this:setPosition(Vertex3D.new(pos.x - speed * dt, pos.y, pos.z))
    end
    if input:isCharPressed("D") then
        this:setPosition(Vertex3D.new(pos.x + speed * dt, pos.y, pos.z))
    end

    -- ========================================
    -- METHOD 2: Using addToPosition
    -- ========================================
    -- Move up/down with Space and C
    if input:isCharPressed("SPACE") then
        this:addToPosition(Vertex3D.new(0, speed * dt, 0))
    end
    if input:isCharPressed("C") then
        this:addToPosition(Vertex3D.new(0, -speed * dt, 0))
    end

    -- ========================================
    -- METHOD 3: Move along object's local axes
    -- ========================================
    -- Get the object's forward, right, and up vectors
    local forward = this:AxisForward()
    local right = this:AxisRight()

    -- Move along local forward with arrow keys
    if input:isCharPressed("I") then
        local movement = forward:getScaled(speed * dt)
        this:addToPosition(movement)
    end
    if input:isCharPressed("K") then
        local movement = forward:getScaled(-speed * dt)
        this:addToPosition(movement)
    end

    -- ========================================
    -- ROTATION
    -- ========================================
    -- Rotate with Q and E
    if input:isCharPressed("Q") then
        rotateY(-rotationSpeed * dt)
    end
    if input:isCharPressed("E") then
        rotateY(rotationSpeed * dt)
    end
end

-- Helper function to rotate around Y axis
function rotateY(angle)
    local currentRotation = this:getRotation()
    local rotationMatrix = M3.new()
    rotationMatrix:setMatrixRotationForEulerAngles(0, angle, 0)
    local newRotation = rotationMatrix * currentRotation
    this:setRotation(newRotation)
end

function onEnd()
    print("Movement script ended for: " .. this:getName())
end
