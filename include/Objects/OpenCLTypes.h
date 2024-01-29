//
// Created by eduardo on 9/05/23.
//

#ifndef BRAKEZA3D_OPENCLTYPES_H
#define BRAKEZA3D_OPENCLTYPES_H


#include <SDL_stdinc.h>
#include <vector>
#include "Vertex3D.h"
#include "../EngineSetup.h"
#include <glm/vec4.hpp>

struct OCVertex3D {
    OCVertex3D() {}

    float x;
    float y;
    float z;
    float u;
    float v;
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

struct OCLPlane {
    OCLPlane() {}

    OCVertex3D A;
    OCVertex3D B;
    OCVertex3D C;
    OCVertex3D normal;
};

struct CameraData {
    CameraData() {}
    OCVertex3D position;  // Posición de la cámara
    OCVertex3D rotation;  // Rotación de la cámara
};

struct FrustumData {
    FrustumData() {}
    OCLPlane planes[6];  // Frustum planes (OCPlanes)
};

struct OCParticlesContext {
    OCParticlesContext() = default;

    OCParticlesContext(
        float gravity,
        float particlesByFrame,
        float particleLifespan,
        int smokeAngleRange,
        int minVelocity,
        int maxVelocity,
        int alphaMin,
        int alphaMax,
        int positionNoise,
        int velocityNoise,
        float decelerationFactor
    )
    :
        GRAVITY(gravity),
        PARTICLES_BY_SECOND(particlesByFrame),
        PARTICLE_LIFESPAN(particleLifespan),
        SMOKE_ANGLE_RANGE(smokeAngleRange),
        MIN_VELOCITY(minVelocity),
        MAX_VELOCITY(maxVelocity),
        MIN_ALPHA(alphaMin),
        MAX_ALPHA(alphaMax),
        POSITION_NOISE(positionNoise),
        VELOCITY_NOISE(velocityNoise),
        DECELERATION_FACTOR(decelerationFactor)
    {
    }

    static OCParticlesContext forProjectile() {
        return OCParticlesContext(
            0.0f,
            5,
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

    static OCParticlesContext forRayLight() {
        return OCParticlesContext();
    }
    static OCParticlesContext forExplosion() {
        return OCParticlesContext();
    }
    static OCParticlesContext forPlayerEngine() {
        return OCParticlesContext(
            0.0f,
            5,
            1.5f,
            25.0f,
            1,
            10,
            125.0f,
            255.0f,
            5,
            29,
            0.99f
        );
    }

    float GRAVITY;
    int PARTICLES_BY_SECOND;
    float PARTICLE_LIFESPAN;
    int SMOKE_ANGLE_RANGE;
    int MIN_VELOCITY;
    int MAX_VELOCITY;
    int MIN_ALPHA;
    int MAX_ALPHA;
    int POSITION_NOISE;
    int VELOCITY_NOISE;
    float DECELERATION_FACTOR;
};

#endif //BRAKEZA3D_OPENCLTYPES_H
