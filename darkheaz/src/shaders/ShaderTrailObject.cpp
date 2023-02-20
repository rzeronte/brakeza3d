
#include "ShaderTrailObject.h"
#include "../../../include/EngineBuffers.h"
#include "../../../include/Brakeza3D.h"

ShaderTrailObject::ShaderTrailObject(Object3D *o, Color c) : ShaderOpenCL("trail.opencl")
{
    this->object = o;
    this->color = c;

    setEnabled(false);

    opencl_buffer_image = clCreateBuffer(
        context,
        CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR,
        this->bufferSize * sizeof(Uint32),
        &this->videoBuffer,
        &clRet
    );

    opencl_buffer_stencil = clCreateBuffer(
        context,
        CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
        this->bufferSize * sizeof(bool),
        &o->stencilBuffer,
        &clRet
    );
}

void ShaderTrailObject::update()
{
    Shader::update();

    if (!isEnabled()) return;

    if (this->object == nullptr) {
        return;
    }

    if (!this->object->isStencilBufferEnabled()) {
        return;
    }

    executeKernelOpenCL();
}

ShaderTrailObject::~ShaderTrailObject() {

}

void ShaderTrailObject::executeKernelOpenCL()
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
        opencl_buffer_stencil,
        CL_FALSE,
        0,
        this->bufferSize * sizeof(bool),
        object->stencilBuffer,
        0,
        nullptr,
        nullptr
    );

    clEnqueueWriteBuffer(
        clCommandQueue,
        opencl_buffer_image,
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
    clSetKernelArg(kernel, 5, sizeof(cl_mem), (void *)&opencl_buffer_stencil);
    clSetKernelArg(kernel, 6, sizeof(cl_mem), (void *)&opencl_buffer_image);
    clSetKernelArg(kernel, 7, sizeof(float), &this->color.r);
    clSetKernelArg(kernel, 8, sizeof(float), &this->color.g);
    clSetKernelArg(kernel, 9, sizeof(float), &this->color.b);

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

    clEnqueueReadBuffer(
        clCommandQueue,
        opencl_buffer_image,
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
