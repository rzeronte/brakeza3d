//
// Created by eduardo on 4/05/23.
//

#ifndef BRAKEZA3D_SHADERPARTICLES_H
#define BRAKEZA3D_SHADERPARTICLES_H


#include "../Render/ShaderOpenCL.h"
#include "../Objects/Point2D.h"
#include "../Objects/Triangle3D.h"
#include "../Objects/OpenCLTypes.h"
#include "../EngineSetup.h"

#define MAX_OPENCL_PARTICLES 1024

class ShaderParticles : public ShaderOpenCL
{
    Vertex3D origin;
    Vertex3D direction;

    Color colorFrom;
    Color colorTo;

    OCParticlesContext particlesContext;

    float intensity;

    bool stopAdd;

    GLuint particlesBuffer;
    Timer timer;
    float deltaTime = 0;
    float last_ticks = 0;
    float current_ticks = 0;
    float executionTime = 0;
public:
    ShaderParticles(bool active, Color from, Color to, OCParticlesContext context);

    void update(Vertex3D origin, Vertex3D direction, float intensity);
    void update() override;

    void executeKernelOpenCL();

    void setOrigin(const Vertex3D &origin);

    void setDirection(const Vertex3D &direction);

    void setIntensity(float intensity);

    void setStopAdd(bool stopAdd);

    OCParticlesContext &getParticlesContext();

    void resetContext();
};


#endif //BRAKEZA3D_SHADERPARTICLES_H
