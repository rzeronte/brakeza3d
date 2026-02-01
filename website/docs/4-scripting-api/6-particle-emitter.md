---
sidebar_position: 7
title: Particle emitter
---

# Particle emitter
---

## ParticleEmitter object
---

**Brakeza3D** includes a dedicated object for emitting particles, called `ParticleEmitter`.

A ParticleEmitter emits 2D image-based particles according to a given configuration.

To configure a ParticleEmitter, the following parameters must be provided:

    - Position: 3D position of the particle emitter.
    - Lifetime: TTL of the particle emitter, in seconds.
    - Start Color (optional): Initial color of the particle at birth.
    - End Color (optional): Final color of the particle when it dies.
    - Context: Particle emitter context.
    - Image: Image file used by the particles.

The most important argument is the context. The context contains multiple parameters that can be
adjusted to modify the behavior of the particles. Particle emitter contexts are implemented through
the ParticlesContext object.

```lua
emitter = ObjectFactory.ParticleEmitter(
    Vertex3D.new(10, 10, 10),                       -- position
    Color.new(255, 255, 0, 255),                    -- color from
    Color.new(255, 255, 255, 255),                  -- color to
)
```

## Particle Context
---

ParticlesContext Properties:

| Parameter          | Type  | Description                                          |
|--------------------|-------|------------------------------------------------------|
| gravity            | float | Gravitational factor applied to the particles        |
| particlesByFrame   | float | Number of particles created per second               |
| particleLifespan   | float | Lifetime of each particle                            |
| angleRange         | int   | Emission cone angle for the particles                |
| minVelocity        | int   | Minimum velocity of a particle at birth              |
| maxVelocity        | int   | Maximum velocity of a particle at birth              |
| alphaMin           | int   | Minimum alpha channel value at birth                 |
| alphaMax           | int   | Maximum alpha channel value at birth                 |
| positionNoise      | int   | Random offset applied to the particle spawn position |
| velocityNoise      | int   | Random variation applied to the particle velocity    |
| decelerationFactor | float | Particle deceleration factor over time               |

All ParticlesContext properties can be modified from the UI.

Example: Creating a Particle Emitter from Code

```lua
-- Create a custom particle context
particleContext = ParticlesContext.new(
    0.0,    -- GRAVITY
    10.0,   -- PARTICLES_BY_SECOND
    1.5,    -- PARTICLE_LIFESPAN
    30,     -- SMOKE_ANGLE_RANGE
    2,      -- MIN_VELOCITY
    12,     -- MAX_VELOCITY
    100,    -- MIN_ALPHA
    255,    -- MAX_ALPHA
    4,      -- POSITION_NOISE
    8,      -- VELOCITY_NOISE
    0.98    -- DECELERATION_FACTOR
)
emitter:setContext(particleContext)
```

The following methods allow you to configure and control the behavior of a ParticleEmitter. They provide
access to its particle context, color interpolation, texture assignment, and emission state, enabling
dynamic adjustment of particle effects at runtime through LUA scripts.

| Method                       | Description                                                                                         |
|------------------------------|-----------------------------------------------------------------------------------------------------|
| setContext(ParticlesContext) | Assigns a new particle context to the emitter, defining how particles are generated and behave      |
| setColorFrom(Color)          | Sets the initial color of particles at the moment they are spawned                                  |
| setColorTo(Color)            | Sets the final color of particles when they reach the end of their lifespan                         |
| setTexture(string)           | Sets the texture image used by the particle emitter                                                 |
| setStopAdd(bool)             | Enables or disables the emission of new particles while allowing existing ones to continue updating |
| isActive()                   | Returns whether the particle emitter is currently active                                            |
| getColorFrom()               | Returns the current start color used for newly spawned particles                                    |
| getColorTo()                 | Returns the current end color applied to particles at the end of their lifespan                     |
| getTexture()                 | Returns the file path of the texture currently used by the particle emitter                         |