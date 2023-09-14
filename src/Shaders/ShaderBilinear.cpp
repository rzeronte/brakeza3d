//
// Created by eduardo on 10/05/23.
//

#include "../../include/Shaders/ShaderBilinear.h"
#include "../../include/EngineSetup.h"
#include "../../include/EngineBuffers.h"
#include "../../include/Brakeza3D.h"

ShaderBilinear::ShaderBilinear(bool active) : ShaderOpenCL(active, "bilinear.cl")
{
    videoOutputBufferOCL = clCreateBuffer(
        context,
        CL_MEM_READ_WRITE,
        EngineBuffers::get()->sizeBuffers * sizeof(Uint32),
        nullptr,
        nullptr
    );
}

void ShaderBilinear::update()
{
    Shader::update();
    executeKernelOpenCL();
}

void ShaderBilinear::executeKernelOpenCL()
{
    clEnqueueCopyBuffer(clQueue, EngineBuffers::get()->videoBufferOCL, videoOutputBufferOCL, 0, 0, this->bufferSize * sizeof(Uint32), 0, nullptr, nullptr );
    clSetKernelArg(kernel, 0, sizeof(int), &EngineSetup::get()->screenWidth);
    clSetKernelArg(kernel, 1, sizeof(int), &EngineSetup::get()->screenHeight);
    clSetKernelArg(kernel, 2, sizeof(float), &Brakeza3D::get()->getExecutionTime());
    clSetKernelArg(kernel, 3, sizeof(cl_mem), (void *)&videoOutputBufferOCL);
    clSetKernelArg(kernel, 4, sizeof(cl_mem), (void *)&EngineBuffers::get()->videoBufferOCL);

    size_t global_item_size[2] = {(size_t) EngineSetup::get()->screenWidth, (size_t) EngineSetup::get()->screenHeight};
    size_t local_item_size[2] = {16, 16};

    clRet = clEnqueueNDRangeKernel(clQueue, kernel, 2, NULL, global_item_size, local_item_size, 0, NULL, NULL);

    debugKernel("ShaderBilinear");
}
