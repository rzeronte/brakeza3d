//
// Created by eduardo on 24/08/23.
//

#ifndef BRAKEZA3D_SHADERBLURBUFFER_H
#define BRAKEZA3D_SHADERBLURBUFFER_H


#include "../../include/Render/ShaderOpenCL.h"

class ShaderBlurBuffer: public ShaderOpenCL {
    int blurSize;
public:
    ShaderBlurBuffer(bool active, int blurSize);

    void update() override;

    void executeKernelOpenCL();
};


#endif //BRAKEZA3D_SHADERBLURBUFFER_H
