//
// Created by eduardo on 24/08/23.
//

#include "ShaderBlurBuffer.h"
#include "../../include/EngineSetup.h"
#include "../../include/EngineBuffers.h"
#include "../../include/ComponentsManager.h"

ShaderBlurBuffer::ShaderBlurBuffer(bool active, int blurSize)
:
    ShaderOpenCL(active, "blurBuffer.cl"),
    blurSize(blurSize)
{
}

void ShaderBlurBuffer::update()
{
    Shader::update();
    executeKernelOpenCL();
}

void ShaderBlurBuffer::executeKernelOpenCL()
{
    auto bufferVideoParticles = ComponentsManager::get()->getComponentRender()->getClBufferVideoParticles();

    clSetKernelArg(kernel, 0, sizeof(int), &EngineSetup::get()->screenWidth);
    clSetKernelArg(kernel, 1, sizeof(int), &EngineSetup::get()->screenHeight);
    clSetKernelArg(kernel, 2, sizeof(int), &this->blurSize);
    clSetKernelArg(kernel, 3, sizeof(cl_mem), (void *)&EngineBuffers::get()->videoBufferOCL);
    clSetKernelArg(kernel, 4, sizeof(cl_mem), (void *)&bufferVideoParticles);

    size_t global_item_size = this->bufferSize;
    size_t local_item_size = 64;

    clRet = clEnqueueNDRangeKernel(clQueue, kernel, 1, nullptr, &global_item_size, &local_item_size, 0, nullptr, nullptr);

    debugKernel("ShaderBlurBuffer");
}
