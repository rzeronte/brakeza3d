//
// Created by eduardo on 30/05/22.
//

#include "../../include/Render/ShaderOpenCL.h"
#include "../../include/Misc/Tools.h"
#include "../../include/EngineSetup.h"
#include "../../include/EngineBuffers.h"
#include "../../include/Render/Logging.h"

ShaderOpenCL::ShaderOpenCL(
        cl_device_id device_id,
        cl_context context,
        cl_command_queue command_queue,
        const std::string& kernelFilename
) {
    this->clDeviceId = device_id;
    this->clCommandQueue = command_queue;
    this->kernelFilename = kernelFilename;

    initOpenCLProgram(device_id, context);

    Logging::Log("Loading '" + kernelFilename + "' kernel", "OPENCL");
}

void ShaderOpenCL::initOpenCLProgram(cl_device_id &device_id, cl_context context)
{
    size_t source_size;
    char * source_str= Tools::readFile(
        EngineSetup::get()->DARKHEAZ_CL_SHADERS_FOLDER + this->kernelFilename,
        source_size
    );

    program = clCreateProgramWithSource(
        context,
        1,
        (const char **)&source_str,
        (const size_t *)&source_size,
        &clRet
    );

    clBuildProgram(program, 1, &device_id, nullptr, nullptr, nullptr);

    kernel = clCreateKernel(program, "onUpdate", &clRet);

    opencl_buffer_video = clCreateBuffer(
            context,
            CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR,
            EngineBuffers::getInstance()->sizeBuffers * sizeof(Uint32),
            EngineBuffers::getInstance()->videoBuffer,
            &clRet
    );

    opencl_buffer_videoShader = clCreateBuffer(
            context,
            CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR,
            EngineBuffers::getInstance()->sizeBuffers * sizeof(Uint32),
            this->videoBuffer,
            &clRet
    );
}