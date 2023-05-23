typedef struct OCParticlesContext {
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
} OCParticlesContext;

typedef struct OCVertex3D {
    float x;
    float y;
    float z;
    float u;
    float v;
} OCVertex3D;

typedef struct OCPoint2D {
    int x;
    int y;
} OCPoint2D;

typedef struct OCParticle {
    OCVertex3D position;
    OCVertex3D rotation;
    float timeToLive;
    float timeLiving;
    float force;
    OCVertex3D velocity;
    OCVertex3D colorFrom;
    OCVertex3D colorTo;
    bool active;
} OCParticle;

unsigned int createRGBA(int r, int g, int b, int a);
unsigned int lcg(unsigned int prev);
float getRandomFloat(float a, float b, unsigned int *randVal);
unsigned int alphaBlend(unsigned int color1, unsigned int color2, unsigned int alpha);

__kernel void onUpdate(
    int screenWidth,
    int screenHeight,
    float totalExecutionTimeInSeconds,
    float deltaTimeInSeconds,
    __global unsigned int *video,
    __global OCParticlesContext *context,
    __global OCParticle *particles,
    __global OCVertex3D *colorFrom,
    __global OCVertex3D *colorTo,
    __global OCPoint2D *origin,
    float intensity
)
{
    int i = get_global_id(0);
    OCParticle *p = &particles[i];

    if(totalExecutionTimeInSeconds > context->step_add_particle && !p->active) {
        // Reinitializing particle
        p->position.x = origin->x + context->position_noise * getRandomFloat(-1.0, 1.0, &i);
        p->position.y = origin->y + context->position_noise * getRandomFloat(-1.0, 1.0, &i);
        p->colorFrom = *colorFrom;
        p->colorTo = *colorTo;
        // Random direction for particle
        float angle = getRandomFloat(0, 2.0f * M_PI, &i);
        float speed = getRandomFloat(context->min_velocity, context->max_velocity, &i);

        p->velocity.x = speed * cos(angle) + context->velocity_noise * getRandomFloat(-1.0, 1.0, &i);
        p->velocity.y = speed * sin(angle) + context->velocity_noise * getRandomFloat(-1.0, 1.0, &i);

        p->timeToLive = context->particle_lifespan;
        p->timeLiving = 0;
        p->force = (int) getRandomFloat(-10.0, 20.0, &i);
        p->active = true;
    }

    if(p->active) {
        unsigned int randVal = lcg(i);

        // Update particle's position
        p->position.x += p->velocity.x * deltaTimeInSeconds;
        p->position.y += p->velocity.y * deltaTimeInSeconds;

        // Apply gravity and deceleration
        p->velocity.y -= context->gravity * deltaTimeInSeconds;
        p->velocity.x *= 1.0f - context->deceleration_factor * deltaTimeInSeconds;
        p->velocity.y *= 1.0f - context->deceleration_factor * deltaTimeInSeconds;

        // Apply velocity noise that decreases with time
        float noiseFactor = (p->timeToLive - p->timeLiving) / p->timeToLive;

        p->timeLiving += deltaTimeInSeconds;

        // Blend colors over particle's lifespan
        float alpha = p->timeLiving / p->timeToLive;

        float t = alpha;
        OCVertex3D color;
        color.x = p->colorFrom.x * t + p->colorTo.x * (1 - t);
        color.y = p->colorFrom.y * t + p->colorTo.y * (1 - t);
        color.z = p->colorFrom.z * t + p->colorTo.z * (1 - t);

        // Calculate transparency based on particle's lifespan
        unsigned int transparency = alpha * 255;  // Assuming alpha is in the range [0, 1]

        // Plot particle on screen if it's within the screen boundaries
        int px = (int)p->position.x;
        int py = (int)p->position.y;  // Assuming origin is at the top-left corner

        if (px >= 0 && px < screenWidth && py >= 0 && py < screenHeight) {
            unsigned int index = py * screenWidth + px;
            unsigned int c2 = createRGBA(color.x, color.y, color.z, 255);
            unsigned int c1 = video[index];

            video[index] = alphaBlend(c2, c1, (int) alpha);
        }

        if(p->timeLiving > p->timeToLive) {
            // Resetting particle when it exceeds its lifespan
            p->active = false;
        }
    }
}

unsigned int createRGBA(int r, int g, int b, int a)
{
    return (a << 24) + (b << 16) + (g << 8) + (r);
}

unsigned int lcg(unsigned int prev) {
    return (1103515245*prev + 12345) % 0x7fffffff;
}

float getRandomFloat(float a, float b, unsigned int *randVal) {
    *randVal = lcg(*randVal);
    return a + (*randVal / (float)0x7fffffff) * (b - a);
}

unsigned int alphaBlend(unsigned int color1, unsigned int color2, unsigned int alpha) {
    unsigned int rb = color1 & 0xff00ff;
    unsigned int g = color1 & 0x00ff00;
    rb += ((color2 & 0xff00ff) - rb) * alpha >> 8;
    g += ((color2 & 0x00ff00) - g) * alpha >> 8;

    return (rb & 0xff00ff) | (g & 0xff00);
}
