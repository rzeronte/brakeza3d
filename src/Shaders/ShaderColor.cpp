//
// Created by eduardo on 19/11/22.
//

#include "../../include/Shaders/ShaderColor.h"
#include "../../include/EngineSetup.h"
#include "../../include/EngineBuffers.h"
#include "../../include/Render/Logging.h"
#include "../../include/Brakeza3D.h"

ShaderColor::ShaderColor(bool active, Color color, float progress)
:
    ShaderOpenCL(active, "color.opencl"),
    color(color),
    progress(progress)
{
}

void ShaderColor::update()
{
    Shader::update();

    if(!isEnabled()) return;

    executeKernelOpenCL();
}

void ShaderColor::executeKernelOpenCL()
{
    clSetKernelArg(kernel, 0, sizeof(int), &EngineSetup::get()->screenWidth);
    clSetKernelArg(kernel, 1, sizeof(int), &EngineSetup::get()->screenHeight);
    clSetKernelArg(kernel, 2, sizeof(float), &Brakeza3D::get()->getExecutionTime());
    clSetKernelArg(kernel, 3, sizeof(cl_mem), (void *)&EngineBuffers::get()->videoBufferOCL);
    clSetKernelArg(kernel, 4, sizeof(float), &this->color.r);
    clSetKernelArg(kernel, 5, sizeof(float), &this->color.g);
    clSetKernelArg(kernel, 6, sizeof(float), &this->color.b);
    clSetKernelArg(kernel, 7, sizeof(float), &this->progress);

    size_t global_item_size = this->bufferSize;
    size_t local_item_size = 256;

    clRet = clEnqueueNDRangeKernel(clQueue, kernel, 1, nullptr, &global_item_size, &local_item_size, 0, nullptr, nullptr);

    this->debugKernel("ShaderColor");
}

void ShaderColor::setProgress(float value) {
    ShaderColor::progress = value;
}