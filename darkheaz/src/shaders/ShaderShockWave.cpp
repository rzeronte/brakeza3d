
#include "ShaderShockWave.h"
#include "../../../include/EngineBuffers.h"
#include "../../../include/Render/Transforms.h"
#include "../../../include/ComponentsManager.h"
#include "../../../include/Brakeza3D.h"

ShaderShockWave::ShaderShockWave(
        cl_device_id deviceId,
        cl_context context,
        cl_command_queue commandQueue,
        const char *kernelFilename,
        float size,
        float speed,
        float ttl
) : ShaderOpenCL(deviceId, context, commandQueue, kernelFilename) {
    this->startSize = size;
    this->currentSize = size;
    this->waveSpeed = speed;
    this->ttlWave.setStep(ttl);
    this->ttlWave.setEnabled(true);
}

void ShaderShockWave::onUpdate(Vertex3D position)
{
    ttlWave.update();

    auto engineBuffers = EngineBuffers::getInstance();

    this->executeKernelOpenCL(position);

    const float sizeDecreasing = (Brakeza3D::get()->getDeltaTime() * startSize) / ttlWave.getStep();
    currentSize -= sizeDecreasing;

    auto screenBuffer = engineBuffers->videoBuffer;
    auto currentBuffer = this->videoBuffer;

    for (int i = 0; i < bufferSize; i++, ++screenBuffer, ++currentBuffer) {
        *screenBuffer = *currentBuffer;
    }

    if (ttlWave.isFinished()) {
        currentSize = startSize;
        ttlWave.setEnabled(true);
    }
}


void ShaderShockWave::executeKernelOpenCL(Vertex3D position)
{
    clEnqueueWriteBuffer(
        clCommandQueue,
        opencl_buffer_video,
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
        opencl_buffer_videoShader,
        CL_TRUE,
        0,
        EngineBuffers::getInstance()->sizeBuffers * sizeof(Uint32),
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
    clSetKernelArg(kernel, 7, sizeof(cl_mem), (void *)&opencl_buffer_video);
    clSetKernelArg(kernel, 8, sizeof(cl_mem), (void *)&opencl_buffer_videoShader);

    // Process the entire lists
    size_t global_item_size = EngineBuffers::getInstance()->sizeBuffers;
    // Divide work items into groups of 64
    size_t local_item_size = 12;

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
        opencl_buffer_video,
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
        opencl_buffer_videoShader,
        CL_TRUE,
        0,
        EngineBuffers::getInstance()->sizeBuffers * sizeof(Uint32),
        this->videoBuffer,
        0,
        nullptr,
        nullptr
    );

    if (clRet != CL_SUCCESS) {
        Logging::getInstance()->Log( "Error OpenCL kernel: " + std::to_string(clRet) );

        char buffer[1024];
        clGetProgramBuildInfo(
            program,
            clDeviceId,
            CL_PROGRAM_BUILD_LOG,
            sizeof(buffer),
            buffer,
            nullptr
        );
        if (strlen(buffer) > 0 ) {
            Logging::getInstance()->Log( buffer );
        }
    }
}