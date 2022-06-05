//
// Created by eduardo on 30/05/22.
//

#ifndef BRAKEZA3D_SHADEROPENCL_H
#define BRAKEZA3D_SHADEROPENCL_H


#include <CL/cl.h>
#include "Shader.h"

class ShaderOpenCL: public Shader {

public:
    ShaderOpenCL(cl_device_id device_id, cl_context context, cl_command_queue command_queue, const std::string& kernelFilename);

    void initOpenCLProgram(cl_device_id &device_id, cl_context context);

    std::string kernelFilename;

    cl_command_queue clCommandQueue;
    cl_device_id clDeviceId;
    cl_int clRet;

    cl_program program;
    cl_kernel kernel;

    cl_mem opencl_buffer_video;
    cl_mem opencl_buffer_videoShader;
};


#endif //BRAKEZA3D_SHADEROPENCL_H
