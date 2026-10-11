//
// Created by eduardo on 9/05/23.
//

#ifndef BRAKEZA3D_PARTICLESCONTEXT_H
#define BRAKEZA3D_PARTICLESCONTEXT_H


#include <SDL_stdinc.h>
#include <vector>
#include "../3D/Vertex3D.h"
#include "../Config.h"
#include <glm/vec4.hpp>

struct OCParticle {
    glm::vec4 position;
    glm::vec4 rotation;
    glm::vec4 velocity;
    float timeToLive;
    float timeLiving;
    float force;
    float active;
};

struct ParticlesContext {
    ParticlesContext() = default;

    ParticlesContext(
        float gravity,
        float particlesBySecond,
        float particleLifespan,
        int smokeAngleRange,
        float minVelocity,
        float maxVelocity,
        int alphaMin,
        int alphaMax,
        float positionNoise,
        float velocityNoise,
        float decelerationFactor,
        float particleSizeScale = 1.0f
    )
    :
        GRAVITY(gravity),
        PARTICLES_BY_SECOND(particlesBySecond),
        PARTICLE_LIFESPAN(particleLifespan),
        SMOKE_ANGLE_RANGE(smokeAngleRange),
        MIN_VELOCITY(minVelocity),
        MAX_VELOCITY(maxVelocity),
        MIN_ALPHA(alphaMin),
        MAX_ALPHA(alphaMax),
        POSITION_NOISE(positionNoise),
        VELOCITY_NOISE(velocityNoise),
        DECELERATION_FACTOR(decelerationFactor),
        PARTICLE_SIZE_SCALE(particleSizeScale)
    {
    }

    static ParticlesContext defaultParticlesContext() {
        return ParticlesContext(
            0.0f,
            200,   // partículas/s
            1.0f,
            25.0f,
            1,
            10,
            125.0f,
            255.0f,
            5,
            10,
            0.99f
        );
    }

    float GRAVITY;                  // aceleración vertical, u/s² (positivo = hacia arriba del mundo)
    float PARTICLES_BY_SECOND;      // partículas que nacen por segundo (admite decimales)
    float PARTICLE_LIFESPAN;        // segundos
    int SMOKE_ANGLE_RANGE;          // grados: desvío aleatorio (±rango/2 por eje) respecto al "arriba" del emisor
    float MIN_VELOCITY;             // velocidad de salida, u/s (decimales: los valores útiles son pequeños, 0.5-15)
    float MAX_VELOCITY;             // velocidad de salida, u/s
    int MIN_ALPHA;                  // opacidad al nacer, 0-255 (se desvanece con la vida)
    int MAX_ALPHA;                  // opacidad al nacer, 0-255
    float POSITION_NOISE;           // unidades: el nacimiento se reparte en ±valor/2 por eje (admite decimales)
    float VELOCITY_NOISE;           // grados: giro aleatorio de la dirección, por frame a 60 fps (no cambia el módulo)
    float DECELERATION_FACTOR;      // factor por frame a 60 fps (se compensa a otros fps); 1 = sin frenado
    float SIZE_START = 1.0f;           // multiplicador del tamaño al nacer (interpola hasta SIZE_END al morir)
    float SIZE_END   = 1.0f;           // multiplicador del tamaño al morir. 1/1 = tamaño constante (original)
    float PARTICLE_SIZE_SCALE = 1.0f;  // multiplica el tamaño base (~0.1-0.6 u) de cada partícula. 1.0 = comportamiento original.
    // Rotación de cada partícula sobre sí misma, en el plano de la pantalla (billboard). Por defecto
    // sin giro (comportamiento original).
    bool  ROTATION_RANDOM_START = false;  // nace con un ángulo aleatorio (0-360°)
    float ROTATION_SPEED_MIN    = 0.0f;   // velocidad de giro, grados/s (cada partícula elige una en [min, max])
    float ROTATION_SPEED_MAX    = 0.0f;
    bool  ROTATION_RANDOM_DIR   = false;  // sentido de giro aleatorio por partícula (si no, todas igual)
};

#endif //BRAKEZA3D_PARTICLESCONTEXT_H
