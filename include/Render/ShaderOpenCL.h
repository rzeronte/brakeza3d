//
// Created by eduardo on 30/05/22.
//

#ifndef BRAKEZA3D_SHADEROPENCL_H
#define BRAKEZA3D_SHADEROPENCL_H


#include <CL/cl.h>
#include "Shader.h"

class ShaderOpenCL: public Shader {

public:
    ShaderOpenCL(const std::string& kernelFilename);

    void initOpenCLProgram();

    std::string kernelFilename;

    cl_command_queue clCommandQueue;
    cl_device_id clDeviceId;
    cl_int clRet;
    cl_context context;

    cl_program program;
    cl_kernel kernel;

    cl_mem opencl_buffer_video_shader;
    cl_mem opencl_buffer_video_screen;

    void debugKernel();
};


#endif //BRAKEZA3D_SHADEROPENCL_H
