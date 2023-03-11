
#include "ShaderShockWave.h"
#include "../../../include/EngineBuffers.h"
#include "../../../include/Render/Transforms.h"
#include "../../../include/ComponentsManager.h"
#include "../../../include/Brakeza3D.h"

ShaderShockWave::ShaderShockWave(bool active) : ShaderOpenCL(active, "shockWave.opencl")
{
    opencl_buffer_waves = clCreateBuffer(
        context,
        CL_MEM_READ_WRITE,
        MAX_SHOCK_WAVES * sizeof(OCShockWave),
        waves.data(),
        nullptr
    );
}

void ShaderShockWave::update()
{
    Shader::update();

    if (!isEnabled()) return;

    this->executeKernelOpenCL();
}

void ShaderShockWave::executeKernelOpenCL()
{
    const int numberWaves = (int) waves.size();

    if (numberWaves <= 0) return;

    clEnqueueWriteBuffer(
        clCommandQueue,
        opencl_buffer_waves,
        CL_TRUE,
        0,
        numberWaves * sizeof(OCShockWave),
        waves.data(),
        0,
        nullptr,
        nullptr
    );

    clEnqueueWriteBuffer(
        clCommandQueue,
        openClBufferMappedWithVideoOutput,
        CL_FALSE,
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
        CL_FALSE,
        0,
        this->bufferSize * sizeof(Uint32),
        this->videoBuffer,
        0,
        nullptr,
        nullptr
    );

    clSetKernelArg(kernel, 0, sizeof(int), &EngineSetup::get()->screenWidth);
    clSetKernelArg(kernel, 1, sizeof(int), &EngineSetup::get()->screenHeight);
    clSetKernelArg(kernel, 2, sizeof(float), &Brakeza3D::get()->getExecutionTime());
    clSetKernelArg(kernel, 3, sizeof(cl_mem), (void *)&openClBufferMappedWithVideoOutput);
    clSetKernelArg(kernel, 4, sizeof(cl_mem), (void *)&openClBufferMappedWithVideoInput);
    clSetKernelArg(kernel, 5, sizeof(cl_mem), (void *)&opencl_buffer_waves);
    clSetKernelArg(kernel, 6, sizeof(int), &numberWaves);

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

    waves.clear();

    //this->debugKernel();
}

void ShaderShockWave::addShockWave(ShockWave *wave)
{
    Point2D focalPoint = Transforms::WorldToPoint(
        wave->getPosition(),
        ComponentsManager::get()->getComponentCamera()->getCamera(
    ));

    /*Logging::Log("iTime: %f | speed: %.2f | x: %d | y: %d | currentSize: %.2f",
         wave->getTtlWave().getAcumulatedTime(),
         wave->getSpeed(),
         focalPoint.x,
         focalPoint.y,
         wave->getCurrentSize()
    );*/

    this->waves.push_back(OCShockWave{
        wave->getTtlWave().getAcumulatedTime(),
        wave->getSpeed(),
        focalPoint.x,
        focalPoint.y,
        wave->getCurrentSize()
    });
}
