-- ============================================================================
-- Tutorial 06: Particle Systems
-- Type: Object Script (attach to ParticleEmitter or any object)
-- ============================================================================
-- Learn how to create and control particle effects.
-- ============================================================================

local emitter = nil
local isEmitting = true

function onStart()
    print("Particles script attached to: " .. this:getName())

    -- ========================================
    -- CREATE PARTICLE EMITTER
    -- ========================================
    -- Position at this object's location
    local pos = this:getPosition()
    local emitterPos = Vertex3D.new(
        pos.x + emitterOffset.x,
        pos.y + emitterOffset.y,
        pos.z + emitterOffset.z
    )
    local colorFrom = Color.new(1.0, 0.5, 0.0, 1.0)
    local colorTo = Color.new(1.0, 0.0, 0.0, 0.0)

    emitter = ObjectFactory.ParticleEmitter(emitterPos, colorFrom, colorTo)
    emitter:setName("TutorialParticles")

    -- ========================================
    -- CONFIGURE PARTICLE CONTEXT
    -- ========================================
    local context = ParticlesContext.new()

    context.GRAVITY = gravity
    context.DECELERATION_FACTOR = 0.98
    context.PARTICLES_BY_SECOND = particlesPerSecond
    context.PARTICLE_LIFESPAN = particleLifespan
    context.MIN_VELOCITY = 1.0
    context.MAX_VELOCITY = 3.0
    context.SMOKE_ANGLE_RANGE = 45
    context.MIN_ALPHA = 0.5
    context.MAX_ALPHA = 1.0
    context.POSITION_NOISE = 0.2
    context.VELOCITY_NOISE = 0.3

    emitter:setContext(context)

    -- ========================================
    -- PARTICLE COLORS
    -- ========================================
    emitter:setColorFrom(Color.new(1.0, 0.5, 0.0, 1.0))
    emitter:setColorTo(Color.new(1.0, 0.0, 0.0, 0.0))

    -- ========================================
    -- ADD TO SCENE
    -- ========================================
    emitter:setBelongToScene(true)
    Brakeza:AddObject3D(emitter)

    print("Particle emitter created!")
end

function onUpdate()
    local dt = Brakeza:getDeltaTime()
    local input = Components:Input()

    -- ========================================
    -- FOLLOW THIS OBJECT
    -- ========================================
    if emitter then
        local pos = this:getPosition()
        emitter:setPosition(Vertex3D.new(
            pos.x + emitterOffset.x,
            pos.y + emitterOffset.y,
            pos.z + emitterOffset.z
        ))
    end

    -- ========================================
    -- TOGGLE EMISSION WITH SPACE
    -- ========================================
    if input:isCharFirstEventDown("SPACE") then
        isEmitting = not isEmitting
        emitter:setStopAdd(not isEmitting)
        print("Emitting: " .. tostring(isEmitting))
    end

    -- ========================================
    -- CHANGE COLORS WITH 1-4
    -- ========================================
    if input:isCharFirstEventDown("1") then
        -- Fire effect
        emitter:setColorFrom(Color.new(1.0, 0.6, 0.0, 1.0))
        emitter:setColorTo(Color.new(1.0, 0.0, 0.0, 0.0))
        print("Fire colors")
    end

    if input:isCharFirstEventDown("2") then
        -- Smoke effect
        emitter:setColorFrom(Color.new(0.3, 0.3, 0.3, 0.8))
        emitter:setColorTo(Color.new(0.1, 0.1, 0.1, 0.0))
        print("Smoke colors")
    end

    if input:isCharFirstEventDown("3") then
        -- Magic sparkles
        emitter:setColorFrom(Color.new(0.5, 0.0, 1.0, 1.0))
        emitter:setColorTo(Color.new(0.0, 0.5, 1.0, 0.0))
        print("Magic colors")
    end

    if input:isCharFirstEventDown("4") then
        -- Snow
        emitter:setColorFrom(Color.new(1.0, 1.0, 1.0, 1.0))
        emitter:setColorTo(Color.new(0.9, 0.9, 1.0, 0.5))
        print("Snow colors")
    end
end

function onEnd()
    if emitter then
        emitter:setRemoved(true)
    end
end
