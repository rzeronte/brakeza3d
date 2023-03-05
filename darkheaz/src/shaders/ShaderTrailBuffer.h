//
// Created by eduardo on 5/12/22.
//

#ifndef BRAKEZA3D_SHADERTRAILBUFFER_H
#define BRAKEZA3D_SHADERTRAILBUFFER_H


#include "../../../include/Render/ShaderOpenCL.h"
#include "../../../include/Objects/Object3D.h"

class ShaderTrailBuffer: public ShaderOpenCL {
    cl_mem openCLBufferResult;
    cl_mem openCLBufferForStencilObjects;
    bool *stencilObjectsBuffer;
    Color color;
public:
    explicit ShaderTrailBuffer(bool enabled);

    void update() override;

    void executeKernelOpenCL();

    void addStencilBufferObject(Object3D *o);

    void clearStencilBuffer();
};


#endif //BRAKEZA3D_SHADERTRAILBUFFER_H
