-- ============================================================================
-- Tutorial 12: Collision Detection
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

    drawRays()
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

-- Los rayos se dibujan unos segundos para poder verlos: verde = acierta, rojo = falla
local RAY_SECONDS = 3.0
local rays = {}

function addRay(from, to, hit)
    rays[#rays + 1] = { from = from, to = to, hit = hit, ttl = RAY_SECONDS }
end

function drawRays()
    if #rays == 0 then return end
    local render = Components:Render()
    local dt = Brakeza:getDeltaTime()
    for i = #rays, 1, -1 do
        local r = rays[i]
        local c = r.hit and Color.new(0.2, 1.0, 0.3, 1.0) or Color.new(1.0, 0.25, 0.2, 1.0)
        render:DrawLine(r.from, r.to, c)
        r.ttl = r.ttl - dt
        if r.ttl <= 0 then table.remove(rays, i) end
    end
end

function performRaycast()
    local target = Brakeza:getObjectByName("Target")
    if not target then
        print("No target object found for raycast test")
        return
    end

    -- el rayo apunta desde el jugador hacia el Target (el motor no dibuja nada solo)
    local origin = this:getPosition()
    local tpos = target:getPosition()
    local dir = Vertex3D.new(tpos.x - origin.x, tpos.y - origin.y, tpos.z - origin.z):getNormalize()
    local halfSize = 0.484 * (tonumber(this:getScale()) or 1.0)     -- cube.fbx mide ~0.97 sin escalar
    -- sale desde fuera del propio collider, o empezaría dentro de la caja
    local startPos = Vertex3D.new(
        origin.x + dir.x * (halfSize + 0.05),
        origin.y + dir.y * (halfSize + 0.05),
        origin.z + dir.z * (halfSize + 0.05)
    )
    local endPos = Vertex3D.new(
        startPos.x + dir.x * 15,
        startPos.y + dir.y * 15,
        startPos.z + dir.z * 15
    )

    local hit = Components:Collisions():isRayCollisionWith(startPos, endPos, target)
    addRay(startPos, endPos, hit)
    print(hit and "Raycast hit: Target" or "Raycast miss")
end

function checkGround()
    local pos = this:getPosition()
    local halfSize = 0.484 * (tonumber(this:getScale()) or 1.0)     -- cube.fbx mide ~0.97 sin escalar
    local startPos = Vertex3D.new(pos.x, pos.y - halfSize - 0.05, pos.z)   -- justo debajo de la propia caja
    local endPos = Vertex3D.new(pos.x, pos.y - halfSize - 20.0, pos.z)

    local ground = Brakeza:getObjectByName("Ground")
    if ground then
        local onGround = Components:Collisions():isRayCollisionWith(startPos, endPos, ground)
        addRay(startPos, endPos, onGround)
        print("On ground: " .. tostring(onGround))
    end
end

function onEnd()
    print("Total collisions: " .. collisionCount)
end
