//
// Created by eduardo on 14/11/22.
//

#include "ShaderBackgroundImage.h"
#include "../../../include/EngineBuffers.h"
#include "../../../include/Render/Logging.h"
#include "../../../include/Brakeza3D.h"


ShaderImage::ShaderImage() : ShaderOpenCL("image.opencl")
{
    this->image = new Image(EngineSetup::get()->IMAGES_FOLDER + "cloud.png");

    opencl_buffer_pixels_image = clCreateBuffer(
        context,
        CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR,
        EngineBuffers::getInstance()->sizeBuffers * sizeof(Uint32),
        this->image->pixels(),
        &clRet
    );
}

void ShaderImage::update()
{
    Shader::update();

    if (!this->enabled) return;

    executeKernelOpenCL();
}

void ShaderImage::executeKernelOpenCL()
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
            opencl_buffer_pixels_image,
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
    clSetKernelArg(kernel, 5, sizeof(cl_mem), (void *)&opencl_buffer_pixels_image);

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

void ShaderImage::setImage(Image *image)
{
    ShaderImage::image = image;

}

ShaderImage::~ShaderImage()
{

}
