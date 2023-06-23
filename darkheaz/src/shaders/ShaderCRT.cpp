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
    clBufferImage = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, this->bufferSize * sizeof(Uint32), this->image.pixels(), nullptr);
    clEnqueueWriteBuffer(clQueue, clBufferImage, CL_TRUE, 0, this->bufferSize * sizeof(Uint32), image.pixels(), 0, nullptr, nullptr);

    clBufferMask = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, this->bufferSize * sizeof(Uint32), this->mask.pixels(), nullptr);
    clEnqueueWriteBuffer(clQueue, clBufferMask, CL_TRUE, 0, this->bufferSize * sizeof(Uint32), mask.pixels(), 0, nullptr, nullptr);
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
    clSetKernelArg(kernel, 4, sizeof(cl_mem), (void *)&clBufferImage);
    clSetKernelArg(kernel, 5, sizeof(cl_mem), (void *)&clBufferMask);
    clSetKernelArg(kernel, 6, sizeof(unsigned int), &maxAlpha);

    size_t global_item_size = this->bufferSize;
    size_t local_item_size = 64;

    clRet = clEnqueueNDRangeKernel(clQueue, kernel, 1, nullptr, &global_item_size, &local_item_size, 0, nullptr, nullptr);

    debugKernel("ShaderCRT");
}

void ShaderCRT::setMaxAlpha(unsigned int maxAlpha) {
    ShaderCRT::maxAlpha = maxAlpha;
}

void ShaderCRT::setImage(const std::string& filename)
{
    image.setImage(filename);
    clEnqueueWriteBuffer(clQueue, clBufferImage, CL_TRUE, 0, this->bufferSize * sizeof(Uint32), image.pixels(), 0, nullptr, nullptr);
}

void ShaderCRT::setMask(const std::string& filename)
{
    mask.setImage(filename);
    clEnqueueWriteBuffer(clQueue, clBufferMask, CL_TRUE, 0, this->bufferSize * sizeof(Uint32), mask.pixels(), 0, nullptr, nullptr);
}
