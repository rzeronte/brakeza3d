-- ============================================================================
-- Tutorial 05: Skeletal Animation
-- Type: Object Script (attach to Mesh3DAnimation)
-- ============================================================================
-- Learn how to control skeletal animations.
-- This script should be attached to a Mesh3DAnimation object.
-- ============================================================================

local currentAnimation = "idle"

function onStart()
    print("Animation script attached to: " .. this:getName())

    -- ========================================
    -- INITIAL ANIMATION SETUP
    -- ========================================
    this:setAnimationByName(defaultAnimation)
    this:setAnimationSpeed(animationSpeed)
    this:setLoop(true)

    print("Animation controller ready!")
end

function onUpdate()
    local dt = Brakeza:getDeltaTime()
    local input = Components:Input()

    -- ========================================
    -- CHANGE ANIMATIONS WITH NUMBER KEYS
    -- ========================================
    if input:isCharFirstEventDown("1") then
        playAnimation("idle")
    end
    if input:isCharFirstEventDown("2") then
        playAnimation("walk")
    end
    if input:isCharFirstEventDown("3") then
        playAnimation("run")
    end
    if input:isCharFirstEventDown("4") then
        playAnimation("jump")
    end
    if input:isCharFirstEventDown("5") then
        playAnimation("attack")
    end

    -- ========================================
    -- ANIMATION SPEED CONTROL WITH P/O
    -- ========================================
    if input:isCharPressed("P") then
        animationSpeed = math.min(3.0, animationSpeed + dt)
        this:setAnimationSpeed(animationSpeed)
    end
    if input:isCharPressed("O") then
        animationSpeed = math.max(0.1, animationSpeed - dt)
        this:setAnimationSpeed(animationSpeed)
    end

    -- ========================================
    -- TOGGLE LOOP WITH L
    -- ========================================
    if input:isCharFirstEventDown("L") then
        local looping = this:isLoop()
        this:setLoop(not looping)
        print("Loop: " .. tostring(not looping))
    end

    -- ========================================
    -- CHECK IF ANIMATION ENDED
    -- ========================================
    if this:isAnimationEnds() then
        if currentAnimation == "attack" then
            playAnimation("idle")
        end
    end

    -- ========================================
    -- MOVEMENT-BASED ANIMATION
    -- ========================================
    handleMovementAnimation()
end

function playAnimation(name)
    if currentAnimation ~= name then
        currentAnimation = name
        this:setAnimationByName(name)
        print("Playing: " .. name)
    end
end

function handleMovementAnimation()
    local input = Components:Input()
    local moving = false

    if input:isCharPressed("W") or
       input:isCharPressed("A") or
       input:isCharPressed("S") or
       input:isCharPressed("D") then
        moving = true
    end

    -- Don't interrupt attack animation
    if currentAnimation == "attack" and not this:isAnimationEnds() then
        return
    end

    if moving then
        playAnimation("walk")
    else
        playAnimation("idle")
    end
end

function onEnd()
end
