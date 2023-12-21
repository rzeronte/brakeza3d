#version 430 core

#define NUM_PARTICLES 1024

layout (local_size_x = NUM_PARTICLES, local_size_y = 1, local_size_z = 1) in;

struct OCParticle {
    vec4 position;
    vec4 rotation;
    vec4 velocity;
    float timeToLive;
    float timeLiving;
    float force;
    float enabled;
};

struct OCParticlesContext {
    float gravity;
    float step_add_particle;
    float particle_lifespan;
    float smoke_angle_range;
    float min_velocity;
    float max_velocity;
    float alpha_min;
    float alpha_max;
    float position_noise;
    float velocity_noise;
    float deceleration_factor;
};

layout(std430, binding = 0) buffer OCParticles {
    OCParticle particles[NUM_PARTICLES];
};

uniform vec3 smokeDirection;
uniform vec3 origin;
uniform float totalExecutionTimeInSeconds;
uniform float deltaTimeInSeconds;
uniform bool stopAdd;
uniform float intensity;
uniform int screenWidth;
uniform int screenHeight;

uniform OCParticlesContext particlesContext;

void resetParticle(uint i);
void updateParticleEnabled(uint i);
uint lcg(uint prev);
float getRandomFloat(float a, float b, inout uint randVal);

void main()
{
    uint indexParticle = gl_GlobalInvocationID.x;

    updateParticleEnabled(indexParticle);
}

// Función LCG (Linear Congruential Generator)
uint lcg(uint prev) {
    return (1103515245u * prev + 12345u) % 0x7fffffffu;
}

// Función para generar un número aleatorio en el rango [a, b)
float getRandomFloat(float a, float b, inout uint randVal) {
    randVal = lcg(randVal);
    return a + (float(randVal) / 0x7fffffffu) * (b - a);
}

void resetParticle(uint i)
{
        particles[i].enabled = 1.0f;
        uint randVal = lcg(i);
        float positionNoiseX = getRandomFloat(-particlesContext.position_noise, particlesContext.position_noise, randVal) * intensity;
        float positionNoiseY = getRandomFloat(-particlesContext.position_noise, particlesContext.position_noise, randVal) * intensity;
        float positionNoiseZ = getRandomFloat(-particlesContext.position_noise, particlesContext.position_noise, randVal) * intensity;
        particles[i].position = vec4(origin.x + positionNoiseX, origin.y + positionNoiseY, origin.z + positionNoiseZ, 0);
        particles[i].rotation = vec4(0);

        float velocityMagnitude = particlesContext.min_velocity * intensity;
        float angle = particlesContext.smoke_angle_range;
        float radAngle = radians(angle);
        float cosAngle = cos(radAngle);
        float sinAngle = sin(radAngle);

        float rotatedX = smokeDirection.x * cosAngle - smokeDirection.y * sinAngle;
        float rotatedY = smokeDirection.x * sinAngle + smokeDirection.y * cosAngle;

        float velocityNoiseX = getRandomFloat(-particlesContext.velocity_noise, particlesContext.velocity_noise, randVal);
        float velocityNoiseY = getRandomFloat(-particlesContext.velocity_noise, particlesContext.velocity_noise, randVal);
        float velocityNoiseZ = getRandomFloat(-particlesContext.velocity_noise, particlesContext.velocity_noise, randVal);

        particles[i].velocity = vec4(
            (rotatedX + velocityNoiseX) * velocityMagnitude,
            (rotatedY + velocityNoiseY) * velocityMagnitude,
            velocityNoiseZ * smokeDirection.z * velocityMagnitude,
            0
        );

        particles[i].timeToLive = particlesContext.particle_lifespan; //getRandomFloat(-particlesContext.particle_lifespan / 2.0f, particlesContext.particle_lifespan / 2.0f, randVal);
        particles[i].timeLiving = particlesContext.particle_lifespan;
}

void updateParticleEnabled(uint i)
{
        particles[i].timeLiving -= deltaTimeInSeconds;

        if (particles[i].timeLiving <= 0) {
            resetParticle(i);
        } else {

            particles[i].velocity.y += particlesContext.gravity * deltaTimeInSeconds;
            particles[i].position += particles[i].velocity * deltaTimeInSeconds;
            particles[i].velocity *= vec4(particlesContext.deceleration_factor);
            float lifeRatio = particles[i].timeLiving / particles[i].timeToLive;

            uint randVal = lcg(i);
            vec4 noiseVelocity = vec4(
                getRandomFloat(-particlesContext.velocity_noise, particlesContext.velocity_noise, randVal) * lifeRatio,
                getRandomFloat(-particlesContext.velocity_noise, particlesContext.velocity_noise, randVal) * lifeRatio,
                getRandomFloat(-particlesContext.velocity_noise, particlesContext.velocity_noise, randVal) * lifeRatio,
                0
            );

            particles[i].velocity += noiseVelocity;
        }

}