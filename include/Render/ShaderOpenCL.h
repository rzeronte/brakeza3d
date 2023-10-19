//
// Created by eduardo on 30/05/22.
//

#ifndef BRAKEZA3D_SHADEROPENCL_H
#define BRAKEZA3D_SHADEROPENCL_H


#include <CL/cl.h>
#include "Shader.h"
#include "../Misc/cJSON.h"

class ShaderOpenCL: public Shader {
protected:

    std::string kernelFilename;

    cl_command_queue clQueue;
    cl_device_id clDeviceId;
    cl_int clRet;
    cl_context context;

    cl_program program;
    cl_kernel kernel;

    bool useCustomProgram;
public:
    explicit ShaderOpenCL(bool active, const std::string& kernelFilename);
    explicit ShaderOpenCL(bool active);

    virtual ~ShaderOpenCL();

    void initOpenCLProgram();

    void debugKernel(const std::string& from) const;

    virtual void preUpdate();

    virtual void postUpdate();

    virtual void drawImGuiProperties();

    virtual cJSON * getJSON();
};


#endif //BRAKEZA3D_SHADEROPENCL_H
