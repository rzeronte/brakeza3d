#include "ShaderEdgeObject.h"
#include "../../../include/EngineSetup.h"
#include "../../../include/EngineBuffers.h"
#include "../../../include/Render/Logging.h"
#include "../../../include/Brakeza3D.h"

ShaderEdgeObject::ShaderEdgeObject(Color c) : ShaderOpenCL("edge.opencl")
{
    object = nullptr;
    color = c;

    opencl_buffer_stencil = clCreateBuffer(
        context,
        CL_MEM_READ_ONLY,
        this->bufferSize * sizeof(bool),
        nullptr,
        &clRet
    );
}

void ShaderEdgeObject::update()
{
    Shader::update();

    if (!isEnabled()) return;

    if (this->object == nullptr) {
        return;
    }

    if (!this->object->isStencilBufferEnabled()) {
        return;
    }

    auto mesh = dynamic_cast<Mesh3D*> (object);
    if (mesh == nullptr) {
        return;
    }

    executeKernelOpenCL();
}

void ShaderEdgeObject::executeKernelOpenCL()
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
        object->getStencilBuffer(),
        0,
        nullptr,
        nullptr
    );

    clSetKernelArg(kernel, 0, sizeof(int), &EngineSetup::get()->screenWidth);
    clSetKernelArg(kernel, 1, sizeof(int), &EngineSetup::get()->screenHeight);
    clSetKernelArg(kernel, 2, sizeof(float), &Brakeza3D::get()->getExecutionTime());
    clSetKernelArg(kernel, 3, sizeof(cl_mem), (void *)&openClBufferMappedWithVideoOutput);
    clSetKernelArg(kernel, 4, sizeof(cl_mem), (void *)&openClBufferMappedWithVideoInput);
    clSetKernelArg(kernel, 5, sizeof(cl_mem), (void *)&opencl_buffer_stencil);
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

ShaderEdgeObject::~ShaderEdgeObject() {

}

void ShaderEdgeObject::setObject(Object3D *o) {
    this->object = o;
}

void ShaderEdgeObject::setColor(Color c) {
    this->color = c;
}