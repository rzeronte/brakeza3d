#include "ShaderImage.h"
#include "../../../include/EngineBuffers.h"
#include "../../../include/Render/Logging.h"
#include "../../../include/Brakeza3D.h"


ShaderImage::ShaderImage(const std::string& filename)
:
    ShaderOpenCL(true, "image.opencl"),
    image(Image(filename)),
    useOffset(true),
    offsetX(0),
    offsetY(0)
{
    clBufferImage = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, this->bufferSize * sizeof(Uint32), this->image.pixels(), nullptr);
    clEnqueueWriteBuffer(clQueue, clBufferImage, CL_TRUE, 0, this->bufferSize * sizeof(Uint32), image.pixels(), 0, nullptr, nullptr );
}

void ShaderImage::update()
{
    Shader::update();

    if (!this->enabled) return;

    executeKernelOpenCL();
}

void ShaderImage::executeKernelOpenCL()
{
    Vertex3D vel = ComponentsManager::get()->getComponentGame()->getPlayer()->getVelocity().getScaled(0.000015);

    offsetX += vel.x;
    offsetY += vel.y;

    limitOffset();

    clSetKernelArg(kernel, 0, sizeof(int), &EngineSetup::get()->screenWidth);
    clSetKernelArg(kernel, 1, sizeof(int), &EngineSetup::get()->screenHeight);
    clSetKernelArg(kernel, 2, sizeof(float), &Brakeza3D::get()->getExecutionTime());
    clSetKernelArg(kernel, 3, sizeof(cl_mem), (void *)&EngineBuffers::get()->openClVideoBuffer);
    clSetKernelArg(kernel, 4, sizeof(cl_mem), (void *)&clBufferImage);
    clSetKernelArg(kernel, 5, sizeof(int), &useOffset);
    clSetKernelArg(kernel, 6, sizeof(float), &offsetX);
    clSetKernelArg(kernel, 7, sizeof(float), &offsetY);

    size_t global_item_size = this->bufferSize;
    size_t local_item_size = 128;

    clRet = clEnqueueNDRangeKernel(clQueue, kernel, 1, nullptr, &global_item_size, &local_item_size, 0, nullptr, nullptr);

    debugKernel("ShaderImage");
}

void ShaderImage::limitOffset() {
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
}

void ShaderImage::resetOffsets()
{
    offsetX = 0;
    offsetY = 0;
}

void ShaderImage::setImage(const std::string& fileName)
{
    image.loadTGA(fileName);
    refreshBufferImage();
}

void ShaderImage::refreshBufferImage()
{
    clEnqueueWriteBuffer(
            clQueue,
            clBufferImage,
            CL_TRUE,
            0,
        this->bufferSize * sizeof(Uint32),
            image.pixels(),
            0,
            nullptr,
            nullptr
    );
}

bool ShaderImage::isUseOffset() const {
    return useOffset;
}

void ShaderImage::setUseOffset(bool value) {
    ShaderImage::useOffset = value;
}
