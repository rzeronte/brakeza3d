//
// Created by eduardo on 26/06/23.
//

#include "ShaderEnergyShield.h"
#include "../../../include/Brakeza3D.h"

ShaderEnergyShield::ShaderEnergyShield(
    bool active,
    Mesh3D* parent,
    const std::string &maskFilename
)
:
    ShaderOpenCL(active, "energyShield.opencl"),
    mask(Image(maskFilename)),
    maxAlpha(255),
    parent(parent)
{
    clBufferMask = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, this->bufferSize * sizeof(Uint32), this->mask.pixels(), nullptr);
    clEnqueueWriteBuffer(clQueue, clBufferMask, CL_TRUE, 0, this->bufferSize * sizeof(Uint32), mask.pixels(), 0, nullptr, nullptr);
}


void ShaderEnergyShield::update()
{
    Shader::update();

    if (!isEnabled()) return;

    executeKernelOpenCL();
}

void ShaderEnergyShield::executeKernelOpenCL()
{
    auto point = Transforms::WorldToPoint(parent->getPosition(), ComponentsManager::get()->getComponentCamera()->getCamera());

    clSetKernelArg(kernel, 0, sizeof(int), &EngineSetup::get()->screenWidth);
    clSetKernelArg(kernel, 1, sizeof(int), &EngineSetup::get()->screenHeight);
    clSetKernelArg(kernel, 2, sizeof(float), &Brakeza3D::get()->getExecutionTime());
    clSetKernelArg(kernel, 3, sizeof(cl_mem), (void *)&EngineBuffers::get()->videoBufferOCL);
    clSetKernelArg(kernel, 4, sizeof(cl_mem), (void *)&this->parent->getOpenClRenderer()->clBufferStencil);
    clSetKernelArg(kernel, 5, sizeof(cl_mem), (void *)&clBufferMask);
    clSetKernelArg(kernel, 6, sizeof(unsigned int), &point.x);
    clSetKernelArg(kernel, 7, sizeof(unsigned int), &point.y);
    clSetKernelArg(kernel, 8, sizeof(unsigned int), &maxAlpha);

    size_t global_item_size = this->bufferSize;
    size_t local_item_size = 64;

    clRet = clEnqueueNDRangeKernel(clQueue, kernel, 1, nullptr, &global_item_size, &local_item_size, 0, nullptr, nullptr);

    debugKernel("ShaderEnergyShield");
}

void ShaderEnergyShield::setMaxAlpha(unsigned int maxAlpha) {
    ShaderEnergyShield::maxAlpha = maxAlpha;
}
