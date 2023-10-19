//
// Created by eduardo on 24/08/23.
//

#ifndef BRAKEZA3D_SHADERPARTICLESBLURBUFFER_H
#define BRAKEZA3D_SHADERPARTICLESBLURBUFFER_H


#include "../../include/Render/ShaderOpenCL.h"

class ShaderParticlesBlurBuffer: public ShaderOpenCL {
    int blurSize;
public:
    ShaderParticlesBlurBuffer(bool active, int blurSize);

    void update() override;

    void executeKernelOpenCL();

    void postUpdate() override;

    void preUpdate() override;

    cJSON *getJSON();
};


#endif //BRAKEZA3D_SHADERPARTICLESBLURBUFFER_H
