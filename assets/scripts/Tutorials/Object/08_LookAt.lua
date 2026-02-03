-- ============================================================================
-- Tutorial 08: LookAt and Targeting
-- Type: Object Script
-- ============================================================================
-- Learn how to make objects face targets.
-- ============================================================================

local targetObject = nil
local lookAtEnabled = true

function onStart()
    print("LookAt script attached to: " .. this:getName())

    -- Try to find a target object
    targetObject = Brakeza:getObjectByName(targetName)

    if not targetObject then
        targetObject = Brakeza:getObjectByName("Target")
    end

    if targetObject then
        print("Target found: " .. targetObject:getName())
    else
        print("No target found. Create an object named '" .. targetName .. "' or 'Target'")
    end
end

function onUpdate()
    local dt = Brakeza:getDeltaTime()
    local input = Components:Input()

    -- ========================================
    -- TOGGLE LOOKAT WITH SPACE
    -- ========================================
    if input:isCharFirstEventDown("SPACE") then
        lookAtEnabled = not lookAtEnabled
        print("LookAt enabled: " .. tostring(lookAtEnabled))
    end

    -- ========================================
    -- TOGGLE SMOOTH/INSTANT WITH 'S'
    -- ========================================
    if input:isCharFirstEventDown("S") then
        if smoothLookAt == 0 then
            smoothLookAt = 1
        else
            smoothLookAt = 0
        end
        print("Smooth LookAt: " .. tostring(smoothLookAt == 1))
    end

    -- ========================================
    -- ADJUST SPEED WITH P/O
    -- ========================================
    if input:isCharPressed("P") then
        lookSpeed = math.min(20, lookSpeed + dt * 5)
    end
    if input:isCharPressed("O") then
        lookSpeed = math.max(0.5, lookSpeed - dt * 5)
    end

    -- ========================================
    -- PERFORM LOOKAT
    -- ========================================
    if lookAtEnabled and targetObject then
        if smoothLookAt == 1 then
            smoothLookAtTarget(targetObject, dt)
        else
            this:LookAt(targetObject)
        end
    end

    -- ========================================
    -- MANUAL ROTATION WITH A/D (when LookAt disabled)
    -- ========================================
    if not lookAtEnabled then
        if input:isCharPressed("A") then
            rotateY(dt * 2)
        end
        if input:isCharPressed("D") then
            rotateY(-dt * 2)
        end
    end

    -- ========================================
    -- CYCLE TARGETS WITH TAB
    -- ========================================
    if input:isCharFirstEventDown("TAB") then
        cycleTarget()
    end
end

function smoothLookAtTarget(target, dt)
    local dotProduct = calculateFacingDot(target)

    if dotProduct < 0.999 then
        this:LookAt(target)
    end
end

function calculateFacingDot(target)
    local myPos = this:getPosition()
    local targetPos = target:getPosition()

    local toTargetX = targetPos.x - myPos.x
    local toTargetY = targetPos.y - myPos.y
    local toTargetZ = targetPos.z - myPos.z

    local len = math.sqrt(toTargetX^2 + toTargetY^2 + toTargetZ^2)
    if len > 0 then
        toTargetX = toTargetX / len
        toTargetY = toTargetY / len
        toTargetZ = toTargetZ / len
    end

    local forward = this:AxisForward()
    local dot = forward.x * toTargetX + forward.y * toTargetY + forward.z * toTargetZ

    return dot
end

function rotateY(angle)
    local rot = this:getRotation()
    local delta = M3.new()
    delta:setMatrixRotationForEulerAngles(0, angle, 0)
    this:setRotation(delta * rot)
end

function cycleTarget()
    local targets = {"Player", "Target", "Enemy", "Target1", "Target2"}

    for _, name in ipairs(targets) do
        local obj = Brakeza:getObjectByName(name)
        if obj and obj ~= targetObject then
            targetObject = obj
            print("New target: " .. name)
            return
        end
    end

    print("No other targets found")
end

function onEnd()
end
