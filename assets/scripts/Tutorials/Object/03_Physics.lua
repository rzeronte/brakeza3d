-- ============================================================================
-- Tutorial 03: Physics (Bullet Physics)
-- Type: Object Script
-- ============================================================================
-- Learn how to use the Bullet physics system for rigid bodies.
-- Attach this to an object that has a collider configured.
-- ============================================================================

local isSetup = false

function onStart()
    print("Physics script attached to: " .. this:getName())

    -- ========================================
    -- PHYSICS PROPERTIES
    -- ========================================
    -- Set mass (0 = static/immovable)
    this:setMass(mass)

    -- Set friction (0 = slippery ice, 1 = rubber)
    this:setFriction(friction)

    -- Set restitution/bounciness (0 = no bounce, 1 = super bouncy)
    this:setRestitution(0.3)

    -- Damping (resistance to motion, like air drag)
    this:setLinearDamping(0.1)
    this:setAngularDamping(0.1)

    isSetup = true
    print("Physics configured!")
end

function onUpdate()
    if not isSetup then return end

    local dt = Brakeza:getDeltaTime()
    local input = Components:Input()

    -- ========================================
    -- APPLY FORCES WITH WASD
    -- ========================================
    if input:isCharPressed("W") then
        this:ApplyCentralForce(Vertex3D.new(0, 0, -moveForce))
    end
    if input:isCharPressed("S") then
        this:ApplyCentralForce(Vertex3D.new(0, 0, moveForce))
    end
    if input:isCharPressed("A") then
        this:ApplyCentralForce(Vertex3D.new(-moveForce, 0, 0))
    end
    if input:isCharPressed("D") then
        this:ApplyCentralForce(Vertex3D.new(moveForce, 0, 0))
    end

    -- ========================================
    -- JUMP WITH SPACE (IMPULSE)
    -- ========================================
    if input:isCharFirstEventDown("SPACE") then
        if this:onGround() then
            this:ApplyCentralImpulse(Vertex3D.new(0, jumpForce, 0))
            print("Jump!")
        end
    end

    -- ========================================
    -- DIRECT VELOCITY CONTROL WITH I/K
    -- ========================================
    if input:isCharPressed("I") then
        this:setLinearVelocity(Vertex3D.new(0, 0, -5))
    end
    if input:isCharPressed("K") then
        this:setLinearVelocity(Vertex3D.new(0, 0, 5))
    end

    -- ========================================
    -- STOP ALL MOTION WITH X
    -- ========================================
    if input:isCharFirstEventDown("X") then
        this:setLinearVelocity(Vertex3D.new(0, 0, 0))
        this:setAngularVelocity(Vertex3D.new(0, 0, 0))
        print("Motion stopped!")
    end

    -- ========================================
    -- TOGGLE PHYSICS WITH P
    -- ========================================
    if input:isCharFirstEventDown("P") then
        local enabled = this:isCollisionsEnabled()
        this:setCollisionsEnabled(not enabled)
        print("Physics: " .. tostring(not enabled))
    end
end

-- Called when this object collides with another
function onCollision(collisionInfo)
    local otherObject = collisionInfo:getObject()
    if otherObject then
        print("Collision with: " .. otherObject:getName())
    end
end

function onEnd()
end
