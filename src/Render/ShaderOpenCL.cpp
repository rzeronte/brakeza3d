//
// Created by eduardo on 30/05/22.
//

#include "../../include/Render/ShaderOpenCL.h"
#include "../../include/Misc/Tools.h"
#include "../../include/EngineSetup.h"
#include "../../include/EngineBuffers.h"
#include "../../include/Render/Logging.h"
#include "../../include/ComponentsManager.h"

ShaderOpenCL::ShaderOpenCL(bool active, const std::string& kernelFilename): Shader(active)
{
    this->clDeviceId = ComponentsManager::get()->getComponentRender()->getClDeviceId();
    this->clQueue = ComponentsManager::get()->getComponentRender()->getClCommandQueue();
    this->kernelFilename = kernelFilename;
    this->context = ComponentsManager::get()->getComponentRender()->getClContext();

    initOpenCLProgram();

    Logging::Log("Loading '%s' kernel", kernelFilename.c_str());
}

void ShaderOpenCL::initOpenCLProgram()
{
    size_t source_size;
    char * source_str = Tools::readFile(
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

    openClBufferMappedWithVideoInput = clCreateBuffer(
        context,
        CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR,
        EngineBuffers::get()->sizeBuffers * sizeof(Uint32),
        EngineBuffers::get()->videoBuffer,
        nullptr
    );

    openClBufferMappedWithVideoOutput = clCreateBuffer(
        context,
         CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR,
        EngineBuffers::get()->sizeBuffers * sizeof(Uint32),
        EngineBuffers::get()->videoBuffer,
        nullptr
    );

    free(source_str);
}

void ShaderOpenCL::debugKernel(std::string from) const
{
    if (!EngineSetup::get()->LOGGING) return;

    if (clRet != CL_SUCCESS) {
        Logging::Log( "Error OpenCL kernel: %d %s",  clRet, from.c_str());

        char buffer[2048*10];
        clGetProgramBuildInfo(
            program,
            clDeviceId,
            CL_PROGRAM_BUILD_LOG,
            sizeof(buffer),
            buffer,
            nullptr
        );

        if (strlen(buffer) > 0 ) {
            Logging::Log( buffer );
        }
    }
}

ShaderOpenCL::~ShaderOpenCL()
{
    clReleaseMemObject(openClBufferMappedWithVideoInput);
    clReleaseMemObject(openClBufferMappedWithVideoOutput);
    clReleaseProgram(program);
    clReleaseKernel(kernel);
}
