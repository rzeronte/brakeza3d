//
// Created by eduardo on 4/05/23.
//

#ifndef BRAKEZA3D_SHADERPARTICLES_H
#define BRAKEZA3D_SHADERPARTICLES_H


#include "../../../include/Render/ShaderOpenCL.h"
#include "../../../include/Objects/Point2D.h"
#include "../../../include/Objects/Triangle3D.h"

#define MAX_OPENCL_PARTICLES 1024

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

struct OCParticlesContext {
    float GRAVITY = 0.0f;
    float STEP_ADD_PARTICLE = 0.0025f;
    float PARTICLE_LIFESPAN = 1.0f;
    float SMOKE_ANGLE_RANGE = 25.0f;
    float MIN_VELOCITY = 0.0f;
    float MAX_VELOCITY = 50.0f;
    float ALPHA_MIN = 0.0f;
    float ALPHA_MAX = 255.0f;
    float POSITION_NOISE = 2.5f;
    float VELOCITY_NOISE = 0.2f;
    float DECELERATION_FACTOR = 0.98f;
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
    ShaderParticles(bool active, Color from, Color to);

    void update(Point2D origin, Vertex3D direction, float intensity);

    ~ShaderParticles() override;

    void executeKernelOpenCL();

    void setOrigin(const Point2D &origin);

    void setDirection(const Vertex3D &direction);

    void setIntensity(float intensity);

    void setStopAdd(bool stopAdd);
};


#endif //BRAKEZA3D_SHADERPARTICLES_H
