
#include "ShaderShockWave.h"
#include "../../../include/EngineBuffers.h"
#include "../../../include/Render/Transforms.h"
#include "../../../include/ComponentsManager.h"
#include "../../../include/Brakeza3D.h"

ShaderShockWave::ShaderShockWave(
    bool active,
    Vertex3D position,
    float size,
    float speed,
    float ttl
) :
    ShaderOpenCL(active, "shockWave.opencl"),
    startSize(size),
    currentSize(size),
    waveSpeed(speed),
    ttlWave(Counter(ttl)),
    position(position)
{
}

void ShaderShockWave::onUpdate()
{
    if (!isEnabled()) return;

    ttlWave.update();

    this->executeKernelOpenCL();

    currentSize -= (Brakeza3D::get()->getDeltaTime() * startSize) / ttlWave.getStep();

    if (ttlWave.isFinished()) {
        setEnabled(false);
        ttlWave.setEnabled(false);
    }
}


void ShaderShockWave::executeKernelOpenCL()
{
    clEnqueueWriteBuffer(
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

    clEnqueueWriteBuffer(
        clCommandQueue,
        openClBufferMappedWithVideoInput,
        CL_TRUE,
        0,
        this->bufferSize * sizeof(Uint32),
        this->videoBuffer,
        0,
        nullptr,
        nullptr
    );

    Point2D focalPoint = Transforms::WorldToPoint(
        position,
        ComponentsManager::get()->getComponentCamera()->getCamera()
    );

    float iTime = this->ttlWave.getAcumulatedTime();

    clSetKernelArg(kernel, 0, sizeof(int), &EngineSetup::get()->screenWidth);
    clSetKernelArg(kernel, 1, sizeof(int), &EngineSetup::get()->screenHeight);
    clSetKernelArg(kernel, 2, sizeof(float), &iTime);
    clSetKernelArg(kernel, 3, sizeof(float), &this->waveSpeed);
    clSetKernelArg(kernel, 4, sizeof(int), &focalPoint.x);
    clSetKernelArg(kernel, 5, sizeof(int), &focalPoint.y);
    clSetKernelArg(kernel, 6, sizeof(float), &currentSize);
    clSetKernelArg(kernel, 7, sizeof(cl_mem), (void *)&openClBufferMappedWithVideoOutput);
    clSetKernelArg(kernel, 8, sizeof(cl_mem), (void *)&openClBufferMappedWithVideoInput);

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

void ShaderShockWave::setPosition(const Vertex3D &position) {
    ShaderShockWave::position = position;
}

void ShaderShockWave::reset()
{
    currentSize = startSize;
    ttlWave.setEnabled(true);
}
