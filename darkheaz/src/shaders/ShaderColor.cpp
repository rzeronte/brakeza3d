//
// Created by eduardo on 19/11/22.
//

#include "ShaderColor.h"
#include "../../../include/EngineSetup.h"
#include "../../../include/EngineBuffers.h"
#include "../../../include/Render/Logging.h"
#include "../../../include/Brakeza3D.h"

ShaderColor::ShaderColor(Color color, float progress) : ShaderOpenCL("color.opencl")
{
    this->color = color;
    this->progress = progress;
}

void ShaderColor::update()
{
    Shader::update();

    if(!isEnabled()) return;

    executeKernelOpenCL();
}

ShaderColor::~ShaderColor()
{
}

void ShaderColor::executeKernelOpenCL()
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
    clSetKernelArg(kernel, 8, sizeof(float), &this->progress);

    // Process the entire lists
    size_t global_item_size = this->bufferSize;
    // Divide work items into groups of 64
    size_t local_item_size = 64;

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

void ShaderColor::setProgress(float value) {
    ShaderColor::progress = value;
}

float ShaderColor::getProgress() const {
    return progress;
}