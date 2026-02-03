-- ============================================================================
-- Tutorial 02: Rotation
-- Type: Object Script
-- ============================================================================
-- Learn different ways to rotate objects.
-- ============================================================================

local rotX = 0
local rotY = 0
local rotZ = 0

function onStart()
    print("Rotation script attached to: " .. this:getName())
end

function onUpdate()
    local dt = Brakeza:getDeltaTime()
    local input = Components:Input()

    -- ========================================
    -- MANUAL ROTATION WITH KEYS
    -- ========================================
    -- Rotate around X axis (pitch) with W/S
    if input:isCharPressed("W") then
        rotX = rotX + rotationSpeed * dt
    end
    if input:isCharPressed("S") then
        rotX = rotX - rotationSpeed * dt
    end

    -- Rotate around Y axis (yaw) with A/D
    if input:isCharPressed("A") then
        rotY = rotY + rotationSpeed * dt
    end
    if input:isCharPressed("D") then
        rotY = rotY - rotationSpeed * dt
    end

    -- Rotate around Z axis (roll) with Q/E
    if input:isCharPressed("Q") then
        rotZ = rotZ + rotationSpeed * dt
    end
    if input:isCharPressed("E") then
        rotZ = rotZ - rotationSpeed * dt
    end

    -- ========================================
    -- TOGGLE AUTO-ROTATION WITH SPACE
    -- ========================================
    if input:isCharFirstEventDown("SPACE") then
        if autoRotate == 0 then
            autoRotate = 1
        else
            autoRotate = 0
        end
        print("Auto-rotate: " .. tostring(autoRotate))
    end

    if autoRotate == 1 then
        rotY = rotY + 0.5 * dt
    end

    -- ========================================
    -- RESET ROTATION WITH R
    -- ========================================
    if input:isCharFirstEventDown("R") then
        rotX = 0
        rotY = 0
        rotZ = 0
        print("Rotation reset")
    end

    -- ========================================
    -- APPLY ROTATION
    -- ========================================
    applyEulerRotation(rotX, rotY, rotZ)
end

-- Apply rotation using Euler angles
function applyEulerRotation(x, y, z)
    local rotMatrix = M3.new()
    rotMatrix:setMatrixRotationForEulerAngles(x, y, z)
    this:setRotation(rotMatrix)
end

function onEnd()
end
