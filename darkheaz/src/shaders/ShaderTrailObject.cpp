//
// Created by eduardo on 26/11/22.
//

#include "ShaderTrailObject.h"
#include "../../../include/EngineBuffers.h"
#include "../../../include/Brakeza3D.h"

ShaderTrailObject::ShaderTrailObject() : ShaderOpenCL("trail.opencl")
{
    this->image = new Image(EngineSetup::get()->IMAGES_FOLDER + "cloud.png");

    this->color = Color::green();

    opencl_buffer_stencil = clCreateBuffer(
        context,
        CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR,
        EngineBuffers::getInstance()->sizeBuffers * sizeof(bool),
        this->image->pixels(),
        &clRet
    );

    opencl_buffer_image= clCreateBuffer(
            context,
            CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR,
            EngineBuffers::getInstance()->sizeBuffers * sizeof(Uint32),
            this->image->pixels(),
            &clRet
    );
}

void ShaderTrailObject::update()
{
    Shader::update();
    executeKernelOpenCL();
}

ShaderTrailObject::~ShaderTrailObject() {

}

void ShaderTrailObject::executeKernelOpenCL()
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
    clEnqueueWriteBuffer(
        clCommandQueue,
        opencl_buffer_image,
        CL_TRUE,
        0,
        EngineBuffers::getInstance()->sizeBuffers * sizeof(Uint32),
        image->pixels(),
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
    clSetKernelArg(kernel, 6, sizeof(cl_mem), (void *)&opencl_buffer_image);

    // Process the entire lists
    size_t global_item_size = EngineBuffers::getInstance()->sizeBuffers;
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
        opencl_buffer_video_screen,
        CL_TRUE,
        0,
        EngineBuffers::getInstance()->sizeBuffers * sizeof(Uint32),
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
        EngineBuffers::getInstance()->sizeBuffers * sizeof(Uint32),
        image->pixels(),
        0,
        nullptr,
        nullptr
    );

    this->debugKernel();
}

void ShaderTrailObject::setObject(Object3D *o)
{
    this->object = o;
}

