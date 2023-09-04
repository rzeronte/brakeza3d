//
// Created by eduardo on 26/06/23.
//

#include "ShaderEnergyShield.h"
#include "../../include/Brakeza3D.h"

ShaderEnergyShield::ShaderEnergyShield(
    bool active,
    Mesh3D* parent,
    const std::string &textureFilename,
    const std::string &maskFilename
)
:
    ShaderOpenCL(active, "energyShield.opencl"),
    texture(Image(textureFilename)),
    mask(Image(maskFilename)),
    maxAlpha(255),
    parent(parent)
{
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
    clSetKernelArg(kernel, 4, sizeof(cl_mem), (void *)this->parent->getOpenClRenderer()->getClBufferStencil());
    clSetKernelArg(kernel, 5, sizeof(cl_mem), (void *)&texture.openClTexture);
    clSetKernelArg(kernel, 6, sizeof(cl_mem), (void *)&mask.openClTexture);
    clSetKernelArg(kernel, 7, sizeof(unsigned int), &point.x);
    clSetKernelArg(kernel, 8, sizeof(unsigned int), &point.y);
    clSetKernelArg(kernel, 9, sizeof(unsigned int), &maxAlpha);

    size_t global_item_size = this->bufferSize;
    size_t local_item_size = 64;

    clRet = clEnqueueNDRangeKernel(clQueue, kernel, 1, nullptr, &global_item_size, &local_item_size, 0, nullptr, nullptr);

    debugKernel("ShaderEnergyShield");
}

void ShaderEnergyShield::setMaxAlpha(unsigned int maxAlpha) {
    ShaderEnergyShield::maxAlpha = maxAlpha;
}
