#include "ShaderImage.h"
#include "../../../include/EngineBuffers.h"
#include "../../../include/Render/Logging.h"
#include "../../../include/Brakeza3D.h"


ShaderImage::ShaderImage() : ShaderOpenCL("image.opencl")
{
    this->useOffset = true;

    this->image = new Image(EngineSetup::get()->IMAGES_FOLDER + "cloud.png");

    opencl_buffer_pixels_image = clCreateBuffer(
        context,
        CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR,
        this->bufferSize * sizeof(Uint32),
        this->image->pixels(),
        &clRet
    );

    clEnqueueWriteBuffer(
        clCommandQueue,
        opencl_buffer_pixels_image,
        CL_TRUE,
        0,
        this->bufferSize * sizeof(Uint32),
        image->pixels(),
        0,
        nullptr,
        nullptr
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
        openClBufferMappedWithVideoInput,
        CL_TRUE,
        0,
        this->bufferSize * sizeof(Uint32),
        EngineBuffers::getInstance()->videoBuffer,
        0,
        nullptr,
        nullptr
    );

    Vertex3D vel = ComponentsManager::get()->getComponentGame()->getPlayer()->getVelocity().getScaled(0.000015);

    offsetX += vel.x;
    offsetY += vel.y;

    if (offsetY < -100) {
        offsetY = -100;
    }

    if (offsetY > 100) {
        offsetY = 100;
    }

    if (offsetX < -100) {
        offsetX = -100;
    }

    if (offsetX > 100) {
        offsetX = 100;
    }

    clSetKernelArg(kernel, 0, sizeof(int), &EngineSetup::get()->screenWidth);
    clSetKernelArg(kernel, 1, sizeof(int), &EngineSetup::get()->screenHeight);
    clSetKernelArg(kernel, 2, sizeof(float), &Brakeza3D::get()->executionTime);
    clSetKernelArg(kernel, 3, sizeof(cl_mem), (void *)&openClBufferMappedWithVideoInput);
    clSetKernelArg(kernel, 4, sizeof(cl_mem), (void *)&opencl_buffer_pixels_image);
    clSetKernelArg(kernel, 5, sizeof(int), &useOffset);
    clSetKernelArg(kernel, 6, sizeof(float), &offsetX);
    clSetKernelArg(kernel, 7, sizeof(float), &offsetY);

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
        openClBufferMappedWithVideoInput,
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

void ShaderImage::resetOffsets()
{
    offsetX = 0;
    offsetY = 0;
}

void ShaderImage::setImage(Image *value)
{
    ShaderImage::image = value;
    refreshBufferImage();
}

void ShaderImage::refreshBufferImage()
{
    clEnqueueWriteBuffer(
        clCommandQueue,
        opencl_buffer_pixels_image,
        CL_TRUE,
        0,
        this->bufferSize * sizeof(Uint32),
        image->pixels(),
        0,
        nullptr,
        nullptr
    );
}

ShaderImage::~ShaderImage()
{

}

bool ShaderImage::isUseOffset() const {
    return useOffset;
}

void ShaderImage::setUseOffset(bool value) {
    ShaderImage::useOffset = value;
}
