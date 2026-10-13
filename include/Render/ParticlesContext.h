//
// Created by eduardo on 9/05/23.
//

#ifndef BRAKEZA3D_PARTICLESCONTEXT_H
#define BRAKEZA3D_PARTICLESCONTEXT_H


#include <SDL_stdinc.h>
#include <vector>
#include <string>
#include "../3D/Vertex3D.h"
#include "../Config.h"
#include <glm/vec4.hpp>

// Forma desde la que nacen las partículas (el eje de la forma es el "arriba" local del emisor)
enum class EmitterShape : int {
    POINT = 0,      // un punto (comportamiento original)
    BOX,            // caja de SHAPE_SIZE_X/Y/Z, centrada en el emisor
    SPHERE,         // esfera de SHAPE_RADIUS
    CONE,           // vértice en el emisor, se abre hacia +Y: radio SHAPE_RADIUS a SHAPE_HEIGHT de altura
    CYLINDER,       // radio SHAPE_RADIUS, altura SHAPE_HEIGHT, centrado en el emisor
    TORUS,          // anillo en el plano XZ: radio mayor SHAPE_RADIUS, radio del tubo SHAPE_TUBE_RADIUS
    MESH            // superficie de una malla de la escena (SHAPE_MESH = su nombre)
};

// Hacia dónde salen: por el eje del emisor (original) o hacia fuera de la forma (normal)
enum class EmitterShapeDirection : int {
    AXIS = 0,
    OUTWARD = 1
};

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

    // Espirales / vórtice alrededor del EJE del emisor (su "arriba" local, pasando por su posición).
    // Todo a 0 = comportamiento original (sin vórtice).
    float SWIRL_SPEED = 0.0f;           // grados/s: las partículas giran alrededor del eje (posición y velocidad). Signo = sentido
    float SWIRL_PULL  = 0.0f;           // 1/s²: atracción hacia el eje proporcional a la distancia (hélices cerradas; 0 = ninguna)
    float SPIRAL_RADIAL_SPEED = 0.0f;   // u/s: velocidad de salida hacia fuera del eje, además de la velocidad normal
    int   SPIRAL_ARMS = 0;              // 0 = acimut de salida aleatorio (disco/anillo); N >= 1 = N brazos repartidos por igual
    float SPIRAL_SPIN = 0.0f;           // grados/s: avance del acimut de salida (el "aspersor"); con brazos dibuja la espiral

    // Forma de emisión. POINT + AXIS = comportamiento original (el reparto de POSITION_NOISE se suma siempre).
    EmitterShape SHAPE = EmitterShape::POINT;
    float SHAPE_RADIUS = 1.0f;          // esfera, cono, cilindro; radio mayor del toroide
    float SHAPE_HEIGHT = 2.0f;          // cono, cilindro
    float SHAPE_TUBE_RADIUS = 0.3f;     // radio del tubo del toroide
    float SHAPE_SIZE_X = 2.0f;          // caja: lados completos
    float SHAPE_SIZE_Y = 2.0f;
    float SHAPE_SIZE_Z = 2.0f;
    bool  SHAPE_SURFACE_ONLY = false;   // true = nacen sobre la superficie; false = por todo el volumen (MESH siempre es superficie)
    EmitterShapeDirection SHAPE_DIRECTION = EmitterShapeDirection::AXIS;
    std::string SHAPE_MESH;             // MESH: nombre del Mesh3D de la escena (se resuelve cada frame; sigue su movimiento)
};

#endif //BRAKEZA3D_PARTICLESCONTEXT_H
