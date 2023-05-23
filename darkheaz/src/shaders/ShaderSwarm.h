//
// Created by eduardo on 21/05/23.
//

#ifndef BRAKEZA3D_SHADERSWARM_H
#define BRAKEZA3D_SHADERSWARM_H


#include "../../../include/Render/ShaderOpenCL.h"
#include "../../../include/Objects/Swarm.h"

#define MAX_SWARM_OBJETS 1024

class ShaderSwarm : public ShaderOpenCL {
    cl_mem openCLBufferBoids;
    cl_mem openCLBufferSwarmContext;

    Vertex3D center;
    std::vector<OCBoid> boids;
    int numberElements;
public:
    ShaderSwarm(bool active, Vertex3D center, int numberElements, OCSwarmContext swarmContext);

    void initBoids();
    void update() override;
    void executeKernelOpenCL();

    static OCSwarmContext openCLSwarmContext();
};


#endif //BRAKEZA3D_SHADERSWARM_H
