//
// Created by eduardo on 14/11/22.
//

#include "ShaderEdgeObject.h"
#include "../../../include/EngineSetup.h"
#include "../../../include/EngineBuffers.h"
#include "../../../include/Render/Logging.h"
#include "../../../include/Brakeza3D.h"

ShaderEdgeObject::ShaderEdgeObject() : ShaderOpenCL("edge.opencl")
{
    this->image = new Image(EngineSetup::get()->IMAGES_FOLDER + "cloud.png");

    opencl_buffer_stencil = clCreateBuffer(
        context,
        CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR,
        EngineBuffers::getInstance()->sizeBuffers * sizeof(bool),
        this->image->pixels(),
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
        opencl_buffer_video_shader,
        CL_TRUE,
        0,
        EngineBuffers::getInstance()->sizeBuffers * sizeof(Uint32),
        EngineBuffers::getInstance()->videoBuffer,
        0,
        nullptr,
        nullptr
    );

    clEnqueueWriteBuffer(
        clCommandQueue,
        opencl_buffer_stencil,
        CL_TRUE,
        0,
        EngineBuffers::getInstance()->sizeBuffers * sizeof(bool),
        object->getStencilBuffer(),
        0,
        nullptr,
        nullptr
    );

    clSetKernelArg(kernel, 0, sizeof(int), &EngineSetup::get()->screenWidth);
    clSetKernelArg(kernel, 1, sizeof(int), &EngineSetup::get()->screenHeight);
    clSetKernelArg(kernel, 2, sizeof(float), &Brakeza3D::get()->executionTime);
    clSetKernelArg(kernel, 3, sizeof(cl_mem), (void *)&opencl_buffer_video_screen);
    clSetKernelArg(kernel, 4, sizeof(cl_mem), (void *)&opencl_buffer_video_shader);
    clSetKernelArg(kernel, 5, sizeof(cl_mem), (void *)&opencl_buffer_stencil);

    // Process the entire lists
    size_t global_item_size = EngineBuffers::getInstance()->sizeBuffers;
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
        opencl_buffer_video_screen,
        CL_TRUE,
        0,
        EngineBuffers::getInstance()->sizeBuffers * sizeof(Uint32),
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