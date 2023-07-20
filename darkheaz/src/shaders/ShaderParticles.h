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

class ShaderParticles : public ShaderOpenCL
{
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
    void update() override;

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
