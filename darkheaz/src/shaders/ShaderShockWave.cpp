
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

    clEnqueueWriteBuffer(clQueue, opencl_buffer_waves, CL_TRUE, 0, numberWaves * sizeof(OCShockWave), waves.data(), 0, nullptr, nullptr );

    clSetKernelArg(kernel, 0, sizeof(int), &EngineSetup::get()->screenWidth);
    clSetKernelArg(kernel, 1, sizeof(int), &EngineSetup::get()->screenHeight);
    clSetKernelArg(kernel, 2, sizeof(float), &Brakeza3D::get()->getExecutionTime());
    clSetKernelArg(kernel, 3, sizeof(cl_mem), (void *)&EngineBuffers::get()->openClVideoBuffer);
    clSetKernelArg(kernel, 4, sizeof(cl_mem), (void *)&EngineBuffers::get()->openClVideoBuffer);
    clSetKernelArg(kernel, 5, sizeof(cl_mem), (void *)&opencl_buffer_waves);
    clSetKernelArg(kernel, 6, sizeof(int), &numberWaves);

    size_t global_item_size = this->bufferSize;
    size_t local_item_size = 256;

    clRet = clEnqueueNDRangeKernel(clQueue, kernel, 1, nullptr, &global_item_size, &local_item_size, 0, nullptr, nullptr);

    waves.clear();

    this->debugKernel("ShaderShockWave");
}

void ShaderShockWave::addShockWave(ShockWave *wave)
{
    Point2D focalPoint = Transforms::WorldToPoint(
        wave->getPosition(),
        ComponentsManager::get()->getComponentCamera()->getCamera(
    ));

    this->waves.push_back(OCShockWave{
        wave->getTtlWave().getAcumulatedTime(),
        wave->getSpeed(),
        focalPoint.x,
        focalPoint.y,
        wave->getCurrentSize()
    });
}
