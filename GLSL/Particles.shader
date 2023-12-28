#version 430 core

#define NUM_PARTICLES 1024

layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

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
uniform int numActiveParticles;

uniform OCParticlesContext context;

void main()
{
    uint i = gl_GlobalInvocationID.x;

    if (particles[i].enabled > 0) {
        particles[i].timeLiving += deltaTimeInSeconds;

        if (particles[i].timeLiving > particles[i].timeToLive) {
            particles[i].enabled = 0;
            particles[i].force = 1;
        } else {
            particles[i].position += particles[i].velocity * deltaTimeInSeconds;
            particles[i].velocity *= context.deceleration_factor;
        }
    } else {
        float relativeTime = mod(totalExecutionTimeInSeconds, context.particle_lifespan);
        float activationTime = i * context.step_add_particle;

        // Ajusta nextParticleTime para garantizar que esté en el rango adecuado
        float nextParticleTime = min((i + 1) * context.step_add_particle, context.particle_lifespan);

        // Si el tiempo relativo está en el rango correcto
        if (totalExecutionTimeInSeconds > i * context.step_add_particle && particles[i].force < 1) {
            particles[i].position = vec4(origin, 0);
            particles[i].enabled = 1;

            particles[i].velocity = normalize(vec4(smokeDirection, 0));

            particles[i].timeToLive = context.particle_lifespan;
            particles[i].timeLiving = 0;
        }
    }
}

