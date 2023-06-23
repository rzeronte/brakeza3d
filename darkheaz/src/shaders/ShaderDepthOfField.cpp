
#include "ShaderDepthOfField.h"
#include "../../../include/EngineSetup.h"
#include "../../../include/Brakeza3D.h"

ShaderDepthOfField::ShaderDepthOfField(bool active)
:
    ShaderOpenCL(active, "dof.cl"),
    focusPlaneDepth(15000),
    focusRange(4500),
    blurSize(0.0000045f),
    intensity(1.15)
{
}

void ShaderDepthOfField::update() {
    Shader::update();

    executeKernelOpenCL();
}

void ShaderDepthOfField::executeKernelOpenCL()
{
    float dt = Brakeza3D::get()->getDeltaTime();

    clSetKernelArg(kernel, 0, sizeof(int), &EngineSetup::get()->screenWidth);
    clSetKernelArg(kernel, 1, sizeof(int), &EngineSetup::get()->screenHeight);
    clSetKernelArg(kernel, 2, sizeof(float), &dt);
    clSetKernelArg(kernel, 3, sizeof(cl_mem), (void *)&EngineBuffers::get()->videoBufferOCL);
    clSetKernelArg(kernel, 4, sizeof(cl_mem), (void *)&EngineBuffers::get()->depthBufferOCL);
    clSetKernelArg(kernel, 5, sizeof(float), &focusPlaneDepth);
    clSetKernelArg(kernel, 6, sizeof(float), &focusRange);
    clSetKernelArg(kernel, 7, sizeof(float), &blurSize);
    clSetKernelArg(kernel, 8, sizeof(float), &intensity);


    size_t global_item_size = this->bufferSize;
    size_t local_item_size = 64;

    clRet = clEnqueueNDRangeKernel(clQueue, kernel, 1, nullptr, &global_item_size, &local_item_size, 0, nullptr, nullptr);

    debugKernel("ShaderDepthOfField");
}
