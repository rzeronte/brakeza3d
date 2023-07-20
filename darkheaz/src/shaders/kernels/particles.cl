
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
    __global OCVertex3D *smokeDirection,
    __global OCPoint2D *origin,
    float intensity,
    int stopAdd
    )
{
    int i = get_global_id(0);
    OCParticle *p = &particles[i];

    if ((totalExecutionTimeInSeconds > i * context->step_add_particle && !p->active) && stopAdd == 0) {
        p->active = true;
        unsigned int randVal = lcg(i);
        float positionNoiseX = getRandomFloat(-context->position_noise, context->position_noise, &randVal) * intensity;
        float positionNoiseY = getRandomFloat(-context->position_noise, context->position_noise, &randVal) * intensity;
        p->position = (OCVertex3D) {origin->x + positionNoiseX, origin->y + positionNoiseY, 0, 0, 0};
        p->rotation = (OCVertex3D) {0, 0, 0, 0, 0};

        float velocityMagnitude = getRandomFloat(context->min_velocity, context->max_velocity, &randVal) * intensity;
        float angle = getRandomFloat(-context->smoke_angle_range / 2.0f, context->smoke_angle_range / 2.0f, &randVal);
        float radAngle = radians(angle);
        float cosAngle = cos(radAngle);
        float sinAngle = sin(radAngle);

        float rotatedX = smokeDirection->x * cosAngle - smokeDirection->y * sinAngle;
        float rotatedY = smokeDirection->x * sinAngle + smokeDirection->y * cosAngle;

        float velocityNoiseX = getRandomFloat(-context->velocity_noise, context->velocity_noise, &randVal);
        float velocityNoiseY = getRandomFloat(-context->velocity_noise, context->velocity_noise, &randVal);

        p->velocity = (OCVertex3D) {
            (rotatedX + velocityNoiseX) * velocityMagnitude,
            (rotatedY + velocityNoiseY) * velocityMagnitude + context->gravity * deltaTimeInSeconds,
            smokeDirection->z * velocityMagnitude,
            0, 0
        };

        p->timeToLive = getRandomFloat(-context->particle_lifespan / 2.0f, context->particle_lifespan / 2.0f, &randVal);
        p->timeLiving = p->timeToLive;

        p->colorFrom = *colorFrom;
        p->colorTo = *colorTo;
    }

    if (p->active) {
        p->timeLiving -= deltaTimeInSeconds;
        unsigned int randVal = lcg(i);

        if (p->timeLiving <= 0) {
            p->active = false;
            p->position = (OCVertex3D) {origin->x, origin->y, 0, 0, 0};

        } else {
            p->velocity.y += context->gravity * deltaTimeInSeconds;
            p->position.x += p->velocity.x * deltaTimeInSeconds;
            p->position.y += p->velocity.y * deltaTimeInSeconds;
            p->position.z += p->velocity.z * deltaTimeInSeconds;

            // Añade la desaceleración aquí
            p->velocity.x *= context->deceleration_factor;
            p->velocity.y *= context->deceleration_factor;
            p->velocity.z *= context->deceleration_factor;

            // Calcula la relación de vida para escalar el ruido en la velocidad
            float lifeRatio = p->timeLiving / p->timeToLive;

            // Ajusta el ruido en función de la relación de vida
            float velocityNoiseX = getRandomFloat(-context->velocity_noise, context->velocity_noise, &randVal) * lifeRatio;
            float velocityNoiseY = getRandomFloat(-context->velocity_noise, context->velocity_noise, &randVal) * lifeRatio;

            p->velocity.x += velocityNoiseX;
            p->velocity.y += velocityNoiseY;

            float t = lifeRatio;
            OCVertex3D color;
            color.x = p->colorFrom.x * t + p->colorTo.x * (1 - t);
            color.y = p->colorFrom.y * t + p->colorTo.y * (1 - t);
            color.z = p->colorFrom.z * t + p->colorTo.z * (1 - t);

            int x = (int)p->position.x;
            int y = (int)p->position.y;

            if (x >= 0 && x < screenWidth && y >= 0 && y < screenHeight) {
                unsigned int randVal = lcg(i);
                float alpha = getRandomFloat(context->alpha_min, context->alpha_max, &randVal);

                unsigned int index = y * screenWidth + x;
                unsigned int c2 = createRGBA(color.x, color.y, color.z, 0);
                unsigned int c1 = video[index];

                video[index] = alphaBlend(c2, c1, (int) alpha);
            }
        }
    } else {
        p->position = (OCVertex3D) {origin->x, origin->y, 0, 0, 0};
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
