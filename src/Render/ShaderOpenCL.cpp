//
// Created by eduardo on 30/05/22.
//

#include "../../include/Render/ShaderOpenCL.h"
#include "../../include/Misc/Tools.h"
#include "../../include/EngineSetup.h"
#include "../../include/EngineBuffers.h"
#include "../../include/Render/Logging.h"
#include "../../include/ComponentsManager.h"

ShaderOpenCL::ShaderOpenCL(const std::string& kernelFilename)
{
    this->clDeviceId = ComponentsManager::get()->getComponentRender()->clDeviceId;
    this->clCommandQueue = ComponentsManager::get()->getComponentRender()->clCommandQueue;
    this->kernelFilename = kernelFilename;
    this->context = ComponentsManager::get()->getComponentRender()->clContext;

    initOpenCLProgram();

    Logging::Log("Loading '" + kernelFilename + "' kernel", "OPENCL");
}

void ShaderOpenCL::initOpenCLProgram()
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

    clBuildProgram(program, 1, &this->clDeviceId, nullptr, nullptr, nullptr);

    kernel = clCreateKernel(program, "onUpdate", &clRet);

    opencl_buffer_video_shader = clCreateBuffer(
            context,
            CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR,
            EngineBuffers::getInstance()->sizeBuffers * sizeof(Uint32),
            EngineBuffers::getInstance()->videoBuffer,
            &clRet
    );

    opencl_buffer_video_screen = clCreateBuffer(
            context,
             CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR,
            EngineBuffers::getInstance()->sizeBuffers * sizeof(Uint32),
            EngineBuffers::getInstance()->videoBuffer,
            &clRet
    );
}

void ShaderOpenCL::debugKernel()
{
    if (clRet != CL_SUCCESS) {
        Logging::getInstance()->Log( "Error OpenCL kernel: " + std::to_string(clRet) );

        char buffer[2048];
        clGetProgramBuildInfo(
                program,
                clDeviceId,
                CL_PROGRAM_BUILD_LOG,
                sizeof(buffer),
                buffer,
                nullptr
        );

        if (strlen(buffer) > 0 ) {
            Logging::getInstance()->Log( buffer );
        }
    }
}