//
// Created by eduardo on 24/08/23.
//

#include "ShaderParticlesBlurBuffer.h"
#include "../../include/EngineSetup.h"
#include "../../include/EngineBuffers.h"
#include "../../include/ComponentsManager.h"

ShaderParticlesBlurBuffer::ShaderParticlesBlurBuffer(bool active, int blurSize)
:
    ShaderOpenCL(active, "blurBuffer.cl"),
    blurSize(blurSize)
{
    setLabel("ShaderParticlesBlurBuffer");
}

void ShaderParticlesBlurBuffer::preUpdate()
{
}

void ShaderParticlesBlurBuffer::postUpdate()
{
    if (!this->enabled) return;
    update();
    executeKernelOpenCL();
}

void ShaderParticlesBlurBuffer::update()
{
    Shader::update();
}

void ShaderParticlesBlurBuffer::executeKernelOpenCL()
{
    clSetKernelArg(kernel, 0, sizeof(int), &EngineSetup::get()->screenWidth);
    clSetKernelArg(kernel, 1, sizeof(int), &EngineSetup::get()->screenHeight);
    clSetKernelArg(kernel, 2, sizeof(int), &this->blurSize);
    clSetKernelArg(kernel, 3, sizeof(cl_mem), (void *)&EngineBuffers::get()->videoBufferOCL);
    clSetKernelArg(kernel, 4, sizeof(cl_mem), (void *)ComponentsManager::get()->getComponentRender()->getClBufferVideoParticles());

    size_t global_item_size[2] = {(size_t) EngineSetup::get()->screenWidth, (size_t) EngineSetup::get()->screenHeight};
    size_t local_item_size[2] = {16, 16};

    clEnqueueNDRangeKernel(clQueue, kernel, 2, NULL, global_item_size, local_item_size, 0, NULL, NULL);

    debugKernel("ShaderBlurBuffer");
}


cJSON *ShaderParticlesBlurBuffer::getJSON()
{
    cJSON *root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "type", this->getLabel().c_str());

    return root;
}
