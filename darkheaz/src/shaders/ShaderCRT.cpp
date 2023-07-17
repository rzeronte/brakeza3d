#include "ShaderCRT.h"
#include "../../../include/EngineSetup.h"
#include "../../../include/Brakeza3D.h"

ShaderCRT::ShaderCRT(bool active, const std::string& imageFilename, const std::string& maskFilename)
:
    ShaderOpenCL(active, "crt.cl"),
    image(Image(imageFilename)),
    mask(Image(maskFilename)),
    maxAlpha(255)
{
}

void ShaderCRT::update()
{
    Shader::update();

    if (!isEnabled()) return;

    executeKernelOpenCL();
}

void ShaderCRT::executeKernelOpenCL()
{
    clSetKernelArg(kernel, 0, sizeof(int), &EngineSetup::get()->screenWidth);
    clSetKernelArg(kernel, 1, sizeof(int), &EngineSetup::get()->screenHeight);
    clSetKernelArg(kernel, 2, sizeof(float), &Brakeza3D::get()->getExecutionTime());
    clSetKernelArg(kernel, 3, sizeof(cl_mem), (void *)&EngineBuffers::get()->videoBufferOCL);
    clSetKernelArg(kernel, 4, sizeof(cl_mem), (void *)&image.openClTexture);
    clSetKernelArg(kernel, 5, sizeof(cl_mem), (void *)&mask.openClTexture);
    clSetKernelArg(kernel, 6, sizeof(unsigned int), &maxAlpha);

    size_t global_item_size = this->bufferSize;
    size_t local_item_size = 64;

    clRet = clEnqueueNDRangeKernel(clQueue, kernel, 1, nullptr, &global_item_size, &local_item_size, 0, nullptr, nullptr);
}

void ShaderCRT::setMaxAlpha(unsigned int maxAlpha) {
    ShaderCRT::maxAlpha = maxAlpha;
}

void ShaderCRT::setImage(const std::string& filename)
{
    image.setImage(filename);
}

void ShaderCRT::setMask(const std::string& filename)
{
    mask.setImage(filename);
}
