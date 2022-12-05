#include "ShaderTrailBuffer.h"
#include "../../../include/EngineBuffers.h"
#include "../../../include/Brakeza3D.h"

ShaderTrailBuffer::ShaderTrailBuffer() : ShaderOpenCL("trail.opencl")
{
    this->stencilObjectsBuffer = new bool[bufferSize];

    this->color = Color::olive();

    openCLBufferResult = clCreateBuffer(
        context,
        CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR,
        this->bufferSize * sizeof(Uint32),
        &this->videoBuffer,
        &clRet
    );

    openCLBufferForStencilObjects = clCreateBuffer(
        context,
        CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
        this->bufferSize * sizeof(bool),
        &this->stencilObjectsBuffer,
        &clRet
    );
}

void ShaderTrailBuffer::update() {
    Shader::update();

    if (!isEnabled()) return;

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

    clSetKernelArg(kernel, 0, sizeof(int), &EngineSetup::get()->screenWidth);
    clSetKernelArg(kernel, 1, sizeof(int), &EngineSetup::get()->screenHeight);
    clSetKernelArg(kernel, 2, sizeof(float), &Brakeza3D::get()->executionTime);
    clSetKernelArg(kernel, 3, sizeof(cl_mem), (void *)&openClBufferMappedWithVideoOutput);
    clSetKernelArg(kernel, 4, sizeof(cl_mem), (void *)&openClBufferMappedWithVideoInput);
    clSetKernelArg(kernel, 5, sizeof(cl_mem), (void *)&openCLBufferForStencilObjects);
    clSetKernelArg(kernel, 6, sizeof(cl_mem), (void *)&openCLBufferResult);
    clSetKernelArg(kernel, 7, sizeof(float), &this->color.r);
    clSetKernelArg(kernel, 8, sizeof(float), &this->color.g);
    clSetKernelArg(kernel, 9, sizeof(float), &this->color.b);

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
    auto stencilObject = o->getStencilBuffer();
    for (int i = 0 ; i < this->bufferSize ; i++) {
        if (o->isStencilBufferEnabled()) {
            this->stencilObjectsBuffer[i] = this->stencilObjectsBuffer[i] || stencilObject[i];
        }
    }
}

void ShaderTrailBuffer::clearStencilBuffer() {
    std::fill(stencilObjectsBuffer, stencilObjectsBuffer + EngineSetup::get()->RESOLUTION, false);
}


