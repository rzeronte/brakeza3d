//
// Created by eduardo on 9/05/23.
//

#ifndef BRAKEZA3D_OPENCLTYPES_H
#define BRAKEZA3D_OPENCLTYPES_H


#include <SDL_stdinc.h>
#include <vector>
#include "Vertex3D.h"
#include "../EngineSetup.h"


struct OCPoint2D {
    OCPoint2D() {}
    OCPoint2D(int x, int y) : x(x), y(y) {}

    int x;
    int y;
};

struct OCVertex3D {
    OCVertex3D() {}
    OCVertex3D(Vertex3D &v): x(v.x), y(v.y), z(v.z), u(v.u), v(v.v) {}
    OCVertex3D(float x, float y, float z, float u, float v) : x(x), y(y), z(z), u(u), v(v) {}
    OCVertex3D(float x, float y, float z) : x(x), y(y), z(z), u(0), v(0) {}

    float x;
    float y;
    float z;
    float u;
    float v;
};

struct OCParticle {
    OCParticle(
            const OCVertex3D &position,
            const OCVertex3D &rotation,
            const OCVertex3D &colorFrom,
            const OCVertex3D &colorTo,
            const OCVertex3D &velocity,
            float timeToLive,
            float force,
            bool active
    ) :
            position(position),
            rotation(rotation),
            timeToLive(timeToLive),
            timeLiving(0),
            force(force), velocity(velocity),
            colorFrom(colorFrom),
            colorTo(colorTo),
            active(active)
    {
    }

    OCVertex3D position;
    OCVertex3D rotation;
    float timeToLive;
    float timeLiving;
    float force;
    OCVertex3D velocity;
    OCVertex3D colorFrom;
    OCVertex3D colorTo;
    bool active;
};

struct OCLight {
    OCLight(
            OCVertex3D position, OCVertex3D forward,
            float power, float kc, float kl, float kq, float specularComponent,
            const Uint32 color, const Uint32 colorSpecularity
    )
            : position(position), forward(forward),
              power(power), kc(kc), kl(kl), kq(kq), specularComponent(specularComponent),
              color(color), colorSpecularity(colorSpecularity)
    {
    }
    OCVertex3D position;
    OCVertex3D forward;
    float power;
    float kc;
    float kl;
    float kq;
    float specularComponent;
    Uint32 color;
    Uint32 colorSpecularity;
};

struct OCTriangle
{
    OCTriangle() {}
    OCTriangle(OCVertex3D A, OCVertex3D B, OCVertex3D C) : A(A), B(B), C(C) {
    }
    OCVertex3D A, B, C;
    OCVertex3D Ao, Bo, Co;
    OCVertex3D Ac, Bc, Cc;
    OCVertex3D An, Bn, Cn;
    OCPoint2D As, Bs, Cs;
    float fullArea, reciprocalFullArea ;
    OCVertex3D normal;
    float tex_u1_Ac_z, tex_u2_Bc_z, tex_u3_Cc_z;
    float tex_v1_Ac_z, tex_v2_Bc_z, tex_v3_Cc_z;
    float persp_correct_Az, persp_correct_Bz, persp_correct_Cz;
    int maxX, minX, maxY, minY;
    bool clipped;
};


struct OCLPlane {
    OCLPlane() {}

    OCLPlane(OCVertex3D a, OCVertex3D b, OCVertex3D c, OCVertex3D normal) {
        this->A = a;
        this->B = b;
        this->C = c;
        this->normal = normal;
    }
    OCVertex3D A;
    OCVertex3D B;
    OCVertex3D C;
    OCVertex3D normal;
};

struct ObjectData {
    ObjectData() {}
    ObjectData(const OCVertex3D &position, const OCVertex3D &rotation, float scale, bool light)
            :
            position(position), rotation(rotation), scale(scale), light(light)
    {
    }
    OCVertex3D position;  // Posición del objeto
    OCVertex3D rotation;  // Rotación del objeto
    float scale;        // Escala del objeto
    bool light;

};

struct CameraData {
    CameraData() {}
    CameraData(const OCVertex3D &position, const OCVertex3D &rotation) : position(position), rotation(rotation) {}
    OCVertex3D position;  // Posición de la cámara
    OCVertex3D rotation;  // Rotación de la cámara
};

struct FrustumData {
    FrustumData() {}
    FrustumData(OCVertex3D vNL, OCVertex3D vNR, OCVertex3D vNT, OCVertex3D vNB, std::vector<OCLPlane> planes)
            :
            vNL(vNL), vNR(vNR), vNT(vNT), vNB(vNB)
    {
        for (size_t i = 0; i < planes.size(); ++i) {
            this->planes[i] = planes[i];
        }
    }
    OCVertex3D vNL;       // Vértice Near Plane Left
    OCVertex3D vNR;       // Vértice Near Plane Right
    OCVertex3D vNT;       // Vértice Near Plane Top
    OCVertex3D vNB;       // Vértice Near Plane Bottom
    OCLPlane planes[6];  // Frustum planes (OCPlanes)
};

struct OCLMeshContext {
    OCLMeshContext() {}
    OCLMeshContext(const ObjectData &objectData, const CameraData &cameraData, const FrustumData &frustumData)
            :
            objectData(objectData),
            cameraData(cameraData),
            frustumData(frustumData)
    {
    }

    ObjectData objectData;
    CameraData cameraData;
    FrustumData frustumData;
};


struct OCParticlesContext {
    OCParticlesContext() = default;

    OCParticlesContext(
        float gravity,
        float stepAddParticle,
        float particleLifespan,
        float smokeAngleRange,
        float minVelocity,
        float maxVelocity,
        float alphaMin,
        float alphaMax,
        float positionNoise,
        float velocityNoise,
        float decelerationFactor
    )
    :
    GRAVITY(gravity),
    STEP_ADD_PARTICLE(stepAddParticle),
    PARTICLE_LIFESPAN(particleLifespan),
    SMOKE_ANGLE_RANGE(smokeAngleRange),
    MIN_VELOCITY(minVelocity),
    MAX_VELOCITY(maxVelocity), ALPHA_MIN(alphaMin),
    ALPHA_MAX(alphaMax),
    POSITION_NOISE(positionNoise),
    VELOCITY_NOISE(velocityNoise),
    DECELERATION_FACTOR(decelerationFactor)
    {

    }

    static OCParticlesContext forProjectile() {
        return OCParticlesContext(0.0f, 0.0025f, 0.5f, 25.0f, 0.0f, 97.0f, 125.0f, 255.0f, 2.0f, 2.4f, 0.98f);
    }

    static OCParticlesContext forRayLight() {
        return OCParticlesContext(0.0f, 0.05f, 0.6f, 0.0f, 20.0f, 245.0f, 125.0f, 255.0f, 9.5f, 9.5f, 0.75);
    }
    static OCParticlesContext forExplosion() {
        return OCParticlesContext(0.0f, 0.0025f, 1.5f, 0.0f, 50.0f, 125.0f, 125.0f, 255.0f, 4.5f, 4.5f, 0.99);
    }

    float GRAVITY = EngineSetup::get()->PARTICLES_SHADER_GRAVITY;
    float STEP_ADD_PARTICLE = EngineSetup::get()->PARTICLES_SHADER_STEP_ADD_PARTICLE;
    float PARTICLE_LIFESPAN = EngineSetup::get()->PARTICLES_SHADER_PARTICLE_LIFESPAN;
    float SMOKE_ANGLE_RANGE = EngineSetup::get()->PARTICLES_SHADER_SMOKE_ANGLE_RANGE;
    float MIN_VELOCITY = EngineSetup::get()->PARTICLES_SHADER_MIN_VELOCITY;
    float MAX_VELOCITY = EngineSetup::get()->PARTICLES_SHADER_MAX_VELOCITY;
    float ALPHA_MIN = EngineSetup::get()->PARTICLES_SHADER_ALPHA_MIN;
    float ALPHA_MAX = EngineSetup::get()->PARTICLES_SHADER_ALPHA_MAX;
    float POSITION_NOISE = EngineSetup::get()->PARTICLES_SHADER_POSITION_NOISE;
    float VELOCITY_NOISE = EngineSetup::get()->PARTICLES_SHADER_VELOCITY_NOISE;
    float DECELERATION_FACTOR =EngineSetup::get()->PARTICLES_SHADER_DECELERATION_FACTOR;
};

#endif //BRAKEZA3D_OPENCLTYPES_H