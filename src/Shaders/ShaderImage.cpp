#include "../../include/Shaders/ShaderImage.h"
#include "../../include/EngineBuffers.h"
#include "../../include/Render/Logging.h"
#include "../../include/Brakeza3D.h"


ShaderImage::ShaderImage(const std::string& filename)
:
    ShaderOpenCL(true, "image.opencl"),
    image(Image(filename)),
    useOffset(true),
    useColors(true),
    offsetX(0),
    offsetY(0)
{
    clBufferImage = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, this->bufferSize * sizeof(Uint32), this->image.pixels(), nullptr);
    clEnqueueWriteBuffer(clQueue, clBufferImage, CL_TRUE, 0, this->bufferSize * sizeof(Uint32), image.pixels(), 0, nullptr, nullptr );
}

void ShaderImage::update(float increaseOffsetX, float increaseOffsetY)
{
    Shader::update();

    if (!this->enabled) return;

    executeKernelOpenCL(increaseOffsetX, increaseOffsetY);
}

void ShaderImage::executeKernelOpenCL(float increaseOffsetX, float increaseOffsetY)
{
    offsetX += increaseOffsetX;
    offsetY += increaseOffsetY;

    limitOffset();

    clSetKernelArg(kernel, 0, sizeof(int), &EngineSetup::get()->screenWidth);
    clSetKernelArg(kernel, 1, sizeof(int), &EngineSetup::get()->screenHeight);
    clSetKernelArg(kernel, 2, sizeof(float), &Brakeza3D::get()->getExecutionTime());
    clSetKernelArg(kernel, 3, sizeof(cl_mem), (void *)&EngineBuffers::get()->videoBufferOCL);
    clSetKernelArg(kernel, 4, sizeof(cl_mem), (void *)&clBufferImage);
    clSetKernelArg(kernel, 5, sizeof(int), &useOffset);
    clSetKernelArg(kernel, 6, sizeof(float), &offsetX);
    clSetKernelArg(kernel, 7, sizeof(float), &offsetY);
    clSetKernelArg(kernel, 8, sizeof(int), &useColors);

    //size_t global_item_size = this->bufferSize;
    //size_t local_item_size = 64;

    size_t global_item_size[2] = {640, 480}; // Tamaño global de trabajo (ancho x alto)
    size_t local_item_size[2] = {16, 16};    // Tamaño local de trabajo (puede ajustarse según la GPU)

    // Ejecutar el kernel
    //clRet = clEnqueueNDRangeKernel(clQueue, kernel, 1, nullptr, &global_item_size, &local_item_size, 0, nullptr, nullptr);
    clEnqueueNDRangeKernel(clQueue, kernel, 2, NULL, global_item_size, local_item_size, 0, NULL, NULL);

    debugKernel("ShaderImage");
}

void ShaderImage::limitOffset()
{
    const float maxOffset = 100.0f;
    const float minOffset = -100.0f;

    offsetX = fminf(maxOffset, fmaxf(minOffset, offsetX));
    offsetY = fminf(maxOffset, fmaxf(minOffset, offsetY));
}

void ShaderImage::resetOffsets()
{
    offsetX = 0;
    offsetY = 0;
}

void ShaderImage::setImage(const std::string& fileName)
{
    image.setImage(fileName);
    refreshBufferImage();
}

void ShaderImage::refreshBufferImage()
{
    clEnqueueWriteBuffer(clQueue, clBufferImage, CL_TRUE, 0, this->bufferSize * sizeof(Uint32), image.pixels(), 0, nullptr, nullptr );
}

bool ShaderImage::isUseOffset() const {
    return useOffset;
}

void ShaderImage::setUseOffset(bool value) {
    ShaderImage::useOffset = value;
}

ShaderImage::~ShaderImage() {
    clReleaseMemObject(clBufferImage);
}

void ShaderImage::setUseColors(int useColors) {
    ShaderImage::useColors = useColors;
}

