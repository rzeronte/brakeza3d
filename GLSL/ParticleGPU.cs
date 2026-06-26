#version 430 core

#define NUM_PARTICLES 1024

layout(local_size_x = 64, local_size_y = 1, local_size_z = 1) in;

struct OCParticle {
    vec4 position;   // xyz = world pos, w = size
    vec4 rotation;   // unused (reserved)
    vec4 velocity;   // xyz = velocity, w = unused
    float timeToLive;
    float timeLiving;
    float force;     // unused
    float enabled;   // 1.0 = alive, 0.0 = dead
};

layout(std430, binding = 0) buffer OCParticles {
    OCParticle particles[NUM_PARTICLES];
};

uniform vec3  origin;
uniform vec3  smokeDirection;
uniform float totalTime;
uniform float deltaTime;
uniform bool  stopAdd;

uniform float gravity;
uniform float particlesBySecond;
uniform float particleLifespan;
uniform float smokeAngleRange;
uniform float minVelocity;
uniform float maxVelocity;
uniform float alphaMin;
uniform float alphaMax;
uniform float positionNoise;
uniform float velocityNoise;
uniform float decelerationFactor;

// Integer hash for GPU pseudo-random
float hash(uint n) {
    n = (n << 13u) ^ n;
    n = n * (n * n * 15731u + 789221u) + 1376312589u;
    return float(n & uint(0x7fffffffu)) / float(0x7fffffff);
}

// Returns a direction perturbed by up to angleRange degrees around baseDir
vec3 perturbDirection(uint seed, vec3 baseDir, float angleDeg) {
    float rx = (hash(seed * 3u + 0u) - 0.5) * 2.0 * radians(angleDeg);
    float ry = (hash(seed * 3u + 1u) - 0.5) * 2.0 * radians(angleDeg);

    vec3 upRef = abs(baseDir.y) < 0.9 ? vec3(0.0, 1.0, 0.0) : vec3(1.0, 0.0, 0.0);
    vec3 right = normalize(cross(upRef, baseDir));
    vec3 fwd   = normalize(cross(baseDir, right));

    return normalize(baseDir + right * sin(rx) + fwd * sin(ry));
}

void main()
{
    uint i = gl_GlobalInvocationID.x;
    if (i >= NUM_PARTICLES) return;

    if (particles[i].enabled > 0.5) {
        // --- Update living particle ---
        particles[i].timeLiving += deltaTime;

        if (particles[i].timeLiving >= particles[i].timeToLive) {
            particles[i].enabled = 0.0;
        } else {
            // Gravity
            particles[i].velocity.y += gravity * deltaTime;

            // Velocity noise
            uint ns = uint(i) * 7919u + uint(totalTime * 1000.0);
            particles[i].velocity.x += (hash(ns + 0u) - 0.5) * velocityNoise * deltaTime;
            particles[i].velocity.y += (hash(ns + 1u) - 0.5) * velocityNoise * deltaTime;
            particles[i].velocity.z += (hash(ns + 2u) - 0.5) * velocityNoise * deltaTime;

            // Deceleration
            particles[i].velocity.xyz *= decelerationFactor;

            // Integrate
            particles[i].position.xyz += particles[i].velocity.xyz * deltaTime;
        }
    } else if (!stopAdd) {
        // --- Check if this slot should spawn ---
        // Particle i owns time slot [i*slotWidth, (i+1)*slotWidth] within each cycle
        float slotWidth = particleLifespan / float(NUM_PARTICLES);
        float mySlot    = float(i) * slotWidth;
        float tCur      = mod(totalTime, particleLifespan);
        float tPrev     = mod(totalTime - deltaTime, particleLifespan);

        bool crossed;
        if (tPrev <= tCur) {
            crossed = (tPrev <= mySlot) && (mySlot < tCur);
        } else {
            // Wrap-around within the cycle
            crossed = (mySlot >= tPrev) || (mySlot < tCur);
        }

        if (crossed) {
            uint ss = uint(i) * 1234u + uint(totalTime * 100.0);

            // Random position offset around origin
            float px = (hash(ss + 0u) - 0.5) * positionNoise;
            float py = (hash(ss + 1u) - 0.5) * positionNoise;
            float pz = (hash(ss + 2u) - 0.5) * positionNoise;

            // Random size
            float sz = hash(ss + 3u) * 0.4 + 0.1;

            particles[i].position = vec4(origin + vec3(px, py, pz), sz);
            particles[i].rotation = vec4(0.0);

            // Random velocity
            vec3 dir   = perturbDirection(ss + 10u, smokeDirection, smokeAngleRange);
            float spd  = minVelocity + hash(ss + 4u) * (maxVelocity - minVelocity);
            particles[i].velocity = vec4(dir * spd, 0.0);

            particles[i].timeToLive = particleLifespan;
            particles[i].timeLiving = 0.0;
            particles[i].force      = 0.0;
            particles[i].enabled    = 1.0;
        }
    }
}
