-- ============================================================================
-- Tutorial 13: Skeletal Animation
-- Type: Object Script (attach to Mesh3DAnimation)
-- ============================================================================
-- Learn how to control skeletal animations.
-- This script should be attached to a Mesh3DAnimation object.
--   1 idle   2 walk   3 run   4 jump   5 crouch walk   P / O speed   L loop   W A S D walk/idle
-- ============================================================================

local currentAnimation = "idle"

-- "this" llega a Lua como Object3D (sin métodos de animación): hay que pedir el mismo objeto como Mesh3DAnimation
local anim = nil

-- setAnimationByName pide el nombre EXACTO que trae el FBX. Las de assets/animations/character.fbx son estas; la lección
-- usa nombres cortos (idle, walk...) y esta tabla los traduce. Si pides uno que no existe, el motor lo avisa en el log.
local ANIM = {
    idle   = "Armature|IDLE",
    walk   = "Armature|WALKING",
    run    = "Armature|RUNNING",
    jump   = "Armature|POSE_JUMPING",
    crouch = "Armature|CROUCH_WALKING",
}

function onStart()
    print("Animation script attached to: " .. this:getName())

    anim = Brakeza:getMesh3DAnimationByName(this:getName())
    if anim == nil then
        print("[13_Animation] '" .. this:getName() .. "' is not a Mesh3DAnimation")
        return
    end

    -- ========================================
    -- INITIAL ANIMATION SETUP
    -- ========================================
    anim:setAnimationByName(ANIM[defaultAnimation] or defaultAnimation)
    anim:setAnimationSpeed(animationSpeed)
    anim:setLoop(true)

    print("Animation controller ready!")
end

function onUpdate()
    if anim == nil then return end
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
        playAnimation("crouch")
    end

    -- ========================================
    -- ANIMATION SPEED CONTROL WITH P/O
    -- ========================================
    if input:isCharPressed("P") then
        animationSpeed = math.min(3.0, animationSpeed + dt)
        anim:setAnimationSpeed(animationSpeed)
    end
    if input:isCharPressed("O") then
        animationSpeed = math.max(0.1, animationSpeed - dt)
        anim:setAnimationSpeed(animationSpeed)
    end

    -- ========================================
    -- TOGGLE LOOP WITH L
    -- ========================================
    if input:isCharFirstEventDown("L") then
        local looping = anim:isLoop()
        anim:setLoop(not looping)
        print("Loop: " .. tostring(not looping))
    end

    -- ========================================
    -- CHECK IF ANIMATION ENDED
    -- ========================================
    if anim:isAnimationEnds() then
        if currentAnimation == "jump" then
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
        anim:setAnimationByName(ANIM[name] or name)
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

    -- Don't interrupt the jump animation until it ends
    if currentAnimation == "jump" and not anim:isAnimationEnds() then
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
