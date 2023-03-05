#include "ShaderTrailBuffer.h"
#include "../../../include/EngineBuffers.h"
#include "../../../include/Brakeza3D.h"

ShaderTrailBuffer::ShaderTrailBuffer(bool enabled)
:
    ShaderOpenCL(enabled, "trail.opencl"),
    stencilObjectsBuffer(new bool[bufferSize]),
    color(Color::olive())
{
    openCLBufferResult = clCreateBuffer(
        context,
        CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR,
        this->bufferSize * sizeof(Uint32),
        this->videoBuffer,
        nullptr
    );

    openCLBufferForStencilObjects = clCreateBuffer(
        context,
        CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR,
        this->bufferSize * sizeof(bool),
        this->stencilObjectsBuffer,
        &clRet
    );

    clEnqueueWriteBuffer(
        clCommandQueue,
        openCLBufferResult,
        CL_TRUE,
        0,
        this->bufferSize * sizeof(Uint32),
        this->videoBuffer,
        0,
        nullptr,
        nullptr
    );
}

void ShaderTrailBuffer::update()
{
    if (!isEnabled()) return;

    Shader::update();

    executeKernelOpenCL();
    clearStencilBuffer();
}

void ShaderTrailBuffer::executeKernelOpenCL()
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

    clEnqueueWriteBuffer(
        clCommandQueue,
        openCLBufferForStencilObjects,
        CL_TRUE,
        0,
        this->bufferSize * sizeof(bool),
        this->stencilObjectsBuffer,
        0,
        nullptr,
        nullptr
    );

    clSetKernelArg(kernel, 0, sizeof(int), &EngineSetup::get()->screenWidth);
    clSetKernelArg(kernel, 1, sizeof(int), &EngineSetup::get()->screenHeight);
    clSetKernelArg(kernel, 2, sizeof(float), &Brakeza3D::get()->getExecutionTime());
    clSetKernelArg(kernel, 3, sizeof(cl_mem), (void *)&openClBufferMappedWithVideoInput);
    clSetKernelArg(kernel, 4, sizeof(cl_mem), (void *)&openCLBufferForStencilObjects);
    clSetKernelArg(kernel, 5, sizeof(cl_mem), (void *)&openCLBufferResult);
    clSetKernelArg(kernel, 6, sizeof(float), &this->color.r);
    clSetKernelArg(kernel, 7, sizeof(float), &this->color.g);
    clSetKernelArg(kernel, 8, sizeof(float), &this->color.b);

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
        openClBufferMappedWithVideoInput,
        CL_TRUE,
        0,
        this->bufferSize * sizeof(Uint32),
        EngineBuffers::getInstance()->videoBuffer,
        0,
        nullptr,
        nullptr
    );

    clEnqueueReadBuffer(
        clCommandQueue,
        openCLBufferResult,
        CL_TRUE,
        0,
        this->bufferSize * sizeof(Uint32),
        this->videoBuffer,
        0,
        nullptr,
        nullptr
    );

    this->debugKernel();
}

void ShaderTrailBuffer::addStencilBufferObject(Object3D *o)
{
    if (!o->isStencilBufferEnabled()) return;

    auto stencilObject = o->getStencilBuffer();
    for (int i = 0 ; i < bufferSize ; i++) {
        this->stencilObjectsBuffer[i] = this->stencilObjectsBuffer[i] || stencilObject[i];
    }
}

void ShaderTrailBuffer::clearStencilBuffer() {
    std::fill(stencilObjectsBuffer, stencilObjectsBuffer + bufferSize, false);
}
