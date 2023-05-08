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

    cl_command_queue clQueue;
    cl_device_id clDeviceId;
    cl_int clRet;
    cl_context context;

    cl_program program;
    cl_kernel kernel;
public:
    explicit ShaderOpenCL(bool active, const std::string& kernelFilename);

    virtual ~ShaderOpenCL();

    void initOpenCLProgram();

    void debugKernel(std::string from) const;
};


#endif //BRAKEZA3D_SHADEROPENCL_H
