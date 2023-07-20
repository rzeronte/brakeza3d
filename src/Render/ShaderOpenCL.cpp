//
// Created by eduardo on 30/05/22.
//

#include "../../include/Render/ShaderOpenCL.h"
#include "../../include/Misc/Tools.h"
#include "../../include/EngineSetup.h"
#include "../../include/EngineBuffers.h"
#include "../../include/Render/Logging.h"
#include "../../include/ComponentsManager.h"

ShaderOpenCL::ShaderOpenCL(bool active, const std::string& kernelFilename): Shader(active), useCustomProgram(true)
{
    this->clDeviceId = ComponentsManager::get()->getComponentRender()->getClDeviceId();
    this->clQueue = ComponentsManager::get()->getComponentRender()->getClCommandQueue();
    this->kernelFilename = kernelFilename;
    this->context = ComponentsManager::get()->getComponentRender()->getClContext();

    initOpenCLProgram();

    Logging::Log("Loading '%s' kernel", kernelFilename.c_str());
}

ShaderOpenCL::ShaderOpenCL(bool active): Shader(active), useCustomProgram(false)
{
    this->clDeviceId = ComponentsManager::get()->getComponentRender()->getClDeviceId();
    this->clQueue = ComponentsManager::get()->getComponentRender()->getClCommandQueue();
    this->context = ComponentsManager::get()->getComponentRender()->getClContext();

    Logging::Log("Reusing '%s' kernel", kernelFilename.c_str());
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
    if (useCustomProgram) {
        clReleaseProgram(program);
        clReleaseKernel(kernel);
    }
}
