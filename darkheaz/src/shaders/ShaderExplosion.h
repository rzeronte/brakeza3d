//
// Created by eduardo on 22/05/23.
//

#ifndef BRAKEZA3D_SHADEREXPLOSION_H
#define BRAKEZA3D_SHADEREXPLOSION_H


#include "../../../include/Render/ShaderOpenCL.h"
#include "../../../include/Objects/Point2D.h"
#include "../../../include/Objects/OpenCLTypes.h"
#include "ShaderParticles.h"
#include "../../../include/Misc/Counter.h"

#define MAX_OPENCL_PARTICLES 1024

class ShaderExplosion : public ShaderOpenCL {
public:
    ShaderExplosion(bool active, Color from, Color to, Point2D origin, float emissionTime, OCParticlesContext context);

    void update() override;
    void executeKernelOpenCL();

    cl_mem openCLBufferParticles;
    cl_mem openCLBufferColorFrom;
    cl_mem openCLBufferColorTo;
    cl_mem openCLBufferOrigin;
    cl_mem openCLBufferContext;

    Point2D origin;
    OCPoint2D ocOrigin;

    Color colorFrom;
    Color colorTo;

    OCParticlesContext particlesContext;

    float intensity;
    float emissionTime;

    void resetContext();

    void setIntensity(float intensity);

};


#endif //BRAKEZA3D_SHADEREXPLOSION_H
