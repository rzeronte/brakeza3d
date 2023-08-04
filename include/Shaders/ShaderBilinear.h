//
// Created by eduardo on 10/05/23.
//

#ifndef BRAKEZA3D_SHADERBILINEAR_H
#define BRAKEZA3D_SHADERBILINEAR_H


#include "../Render/ShaderOpenCL.h"

class ShaderBilinear: public ShaderOpenCL {
    cl_mem videoOutputBufferOCL;
public:
    explicit ShaderBilinear(bool active);

    void update() override;

    void executeKernelOpenCL();
};



#endif //BRAKEZA3D_SHADERBILINEAR_H
