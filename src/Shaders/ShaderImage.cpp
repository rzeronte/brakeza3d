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
    clSetKernelArg(kernel, 4, sizeof(cl_mem), (void *)image.getOpenClTexture());
    clSetKernelArg(kernel, 5, sizeof(int), &useOffset);
    clSetKernelArg(kernel, 6, sizeof(float), &offsetX);
    clSetKernelArg(kernel, 7, sizeof(float), &offsetY);
    clSetKernelArg(kernel, 8, sizeof(int), &useColors);

    size_t global_item_size[2] = {(size_t) EngineSetup::get()->screenWidth, (size_t) EngineSetup::get()->screenHeight};
    size_t local_item_size[2] = {16, 16};

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

bool ShaderImage::isUseOffset() const {
    return useOffset;
}

void ShaderImage::setUseOffset(bool value) {
    ShaderImage::useOffset = value;
}

ShaderImage::~ShaderImage() {
}

void ShaderImage::setUseColors(int useColors) {
    ShaderImage::useColors = useColors;
}

Image &ShaderImage::getImage() {
    return image;
}
