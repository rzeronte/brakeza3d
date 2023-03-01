//
// Created by eduardo on 30/05/22.
//

#ifndef BRAKEZA3D_SHADEROPENCL_H
#define BRAKEZA3D_SHADEROPENCL_H


#include <CL/cl.h>
#include "Shader.h"

class ShaderOpenCL: public Shader {
protected:
    std::string kernelFilename;

    cl_command_queue clCommandQueue;
    cl_device_id clDeviceId;
    cl_int clRet;
    cl_context context;

    cl_program program;
    cl_kernel kernel;

    cl_mem openClBufferMappedWithVideoInput;
    cl_mem openClBufferMappedWithVideoOutput;
public:
     explicit ShaderOpenCL(std::string kernelFilename);

    void initOpenCLProgram();

    void debugKernel() const;
};


#endif //BRAKEZA3D_SHADEROPENCL_H
