
#include "ShaderHorizontalBar.h"
#include "../../../include/Render/Logging.h"
#include "../../../include/EngineBuffers.h"
#include "../../../include/Brakeza3D.h"


ShaderHorizontalBar::ShaderHorizontalBar(Color color,float verticalPosition) : ShaderOpenCL("bar.opencl")
{
    this->color = color;
    this->verticalPosition = verticalPosition;
}

void ShaderHorizontalBar::executeKernelOpenCL()
{
    clEnqueueWriteBuffer(
        clCommandQueue,
        openClBufferMappedWithVideoInput,
        CL_TRUE,
        0,
        this->bufferSize * sizeof(Uint32),
        EngineBuffers::getInstance()->videoBuffer,
        0,
        nullptr,
        nullptr
    );

    clSetKernelArg(kernel, 0, sizeof(int), &EngineSetup::get()->screenWidth);
    clSetKernelArg(kernel, 1, sizeof(int), &EngineSetup::get()->screenHeight);
    clSetKernelArg(kernel, 2, sizeof(float), &Brakeza3D::get()->executionTime);
    clSetKernelArg(kernel, 3, sizeof(cl_mem), (void *)&openClBufferMappedWithVideoOutput);
    clSetKernelArg(kernel, 4, sizeof(cl_mem), (void *)&openClBufferMappedWithVideoInput);
    clSetKernelArg(kernel, 5, sizeof(float), &this->color.r);
    clSetKernelArg(kernel, 6, sizeof(float), &this->color.g);
    clSetKernelArg(kernel, 7, sizeof(float), &this->color.b);
    clSetKernelArg(kernel, 8, sizeof(float), &verticalPosition);
    clSetKernelArg(kernel, 9, sizeof(float), &this->value);

    // Process the entire lists
    size_t global_item_size = this->bufferSize;
    // Divide work items into groups of 64
    size_t local_item_size = 16;

    clRet = clEnqueueNDRangeKernel(
        clCommandQueue,
        kernel,
        1,
        nullptr,
        &global_item_size,
        &local_item_size,
        0,
        nullptr,
        nullptr
    );

    clEnqueueReadBuffer(
        clCommandQueue,
        openClBufferMappedWithVideoOutput,
        CL_TRUE,
        0,
        this->bufferSize * sizeof(Uint32),
        EngineBuffers::getInstance()->videoBuffer,
        0,
        nullptr,
        nullptr
    );

    this->debugKernel();
}

void ShaderHorizontalBar::update() {
    Shader::update();

    executeKernelOpenCL();
}

float ShaderHorizontalBar::getValue() const {
    return value;
}

void ShaderHorizontalBar::setValue(float v) {
    ShaderHorizontalBar::value = v;
}

