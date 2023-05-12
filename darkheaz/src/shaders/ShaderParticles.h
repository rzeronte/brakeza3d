//
// Created by eduardo on 4/05/23.
//

#ifndef BRAKEZA3D_SHADERPARTICLES_H
#define BRAKEZA3D_SHADERPARTICLES_H


#include "../../../include/Render/ShaderOpenCL.h"
#include "../../../include/Objects/Point2D.h"
#include "../../../include/Objects/Triangle3D.h"
#include "../../../include/Objects/OpenCLTypes.h"
#include "../../../include/EngineSetup.h"

#define MAX_OPENCL_PARTICLES 1024

struct OCParticlesContext {
    OCParticlesContext() = default;

    OCParticlesContext(
        float gravity, float stepAddParticle, float particleLifespan, float smokeAngleRange,
        float minVelocity, float maxVelocity, float alphaMin, float alphaMax, float positionNoise,
        float velocityNoise, float decelerationFactor
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

class ShaderParticles : public ShaderOpenCL {
    cl_mem openCLBufferParticles;
    cl_mem openCLBufferColorFrom;
    cl_mem openCLBufferColorTo;
    cl_mem openCLBufferDirection;
    cl_mem openCLBufferOrigin;
    cl_mem openCLBufferContext;

    Point2D origin;
    OCPoint2D ocOrigin;

    Vertex3D direction;
    OCVertex3D ocDirection;

    Color colorFrom;
    Color colorTo;

    OCParticlesContext particlesContext;

    float intensity;

    bool stopAdd;

public:
    ShaderParticles(bool active, Color from, Color to, OCParticlesContext context);

    void update(Point2D origin, Vertex3D direction, float intensity);

    ~ShaderParticles() override;

    void executeKernelOpenCL();

    void setOrigin(const Point2D &origin);

    void setDirection(const Vertex3D &direction);

    void setIntensity(float intensity);

    void setStopAdd(bool stopAdd);

    OCParticlesContext &getParticlesContext();

    void resetContext();
};


#endif //BRAKEZA3D_SHADERPARTICLES_H
