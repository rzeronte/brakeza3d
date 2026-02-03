-- ============================================================================
-- Tutorial 04: Collision Detection
-- Type: Object Script
-- ============================================================================
-- Learn how to detect and respond to collisions.
-- ============================================================================

local collisionCount = 0
local lastCollisionName = "None"

function onStart()
    print("Collision script attached to: " .. this:getName())
    this:setCollisionsEnabled(true)
end

function onUpdate()
    local input = Components:Input()

    -- ========================================
    -- RAYCAST EXAMPLE
    -- ========================================
    if input:isCharFirstEventDown("R") then
        performRaycast()
    end

    -- ========================================
    -- CHECK GROUND WITH DOWNWARD RAY
    -- ========================================
    if input:isCharFirstEventDown("G") then
        checkGround()
    end
end

-- ============================================================================
-- COLLISION CALLBACK
-- ============================================================================
function onCollision(collisionInfo)
    collisionCount = collisionCount + 1

    -- Get the object we collided with
    local otherObject = collisionInfo:getObject()

    if otherObject then
        lastCollisionName = otherObject:getName()

        print("Collision #" .. collisionCount .. ": " .. this:getName() .. " hit " .. lastCollisionName)

        -- ========================================
        -- REACT BASED ON OBJECT NAME
        -- ========================================
        -- Check if name contains "Enemy" or "Pickup"
        if lastCollisionName:find("Enemy") then
            onEnemyCollision(otherObject)
        elseif lastCollisionName:find("Pickup") then
            onPickupCollision(otherObject)
        end
    end

    -- ========================================
    -- BONE COLLISION (for animated meshes)
    -- ========================================
    local boneIndex = collisionInfo:getBoneIndexMapping()
    if boneIndex >= 0 then
        print("Hit bone index: " .. boneIndex)
    end
end

-- ============================================================================
-- COLLISION RESPONSE EXAMPLES
-- ============================================================================

function onEnemyCollision(enemy)
    print("Ouch! Hit by enemy: " .. enemy:getName())
    -- Example: Apply knockback
    local myPos = this:getPosition()
    local enemyPos = enemy:getPosition()
    local knockbackDir = Vertex3D.new(
        myPos.x - enemyPos.x,
        myPos.y - enemyPos.y,
        myPos.z - enemyPos.z
    )
    knockbackDir = knockbackDir:getNormalize()
    local knockback = knockbackDir:getScaled(knockbackForce)
    this:ApplyCentralImpulse(knockback)
end

function onPickupCollision(pickup)
    print("Collected: " .. pickup:getName())
    pickup:setRemoved(true)
end

-- ============================================================================
-- RAYCAST FUNCTIONS
-- ============================================================================

function performRaycast()
    local startPos = this:getPosition()
    local forward = this:AxisForward()
    local endPos = Vertex3D.new(
        startPos.x + forward.x * 10,
        startPos.y + forward.y * 10,
        startPos.z + forward.z * 10
    )

    local target = Brakeza:getObjectByName("Target")
    if target then
        local hit = Components:Collisions():isRayCollisionWith(startPos, endPos, target)
        if hit then
            print("Raycast hit: Target")
        else
            print("Raycast miss")
        end
    else
        print("No target object found for raycast test")
    end
end

function checkGround()
    local pos = this:getPosition()
    local startPos = pos
    local endPos = Vertex3D.new(pos.x, pos.y - 2.0, pos.z)

    local ground = Brakeza:getObjectByName("Ground")
    if ground then
        local onGround = Components:Collisions():isRayCollisionWith(startPos, endPos, ground)
        print("On ground: " .. tostring(onGround))
    end
end

function onEnd()
    print("Total collisions: " .. collisionCount)
end
