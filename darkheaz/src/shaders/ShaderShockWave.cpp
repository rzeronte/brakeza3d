
#include "ShaderShockWave.h"
#include "../../../include/EngineBuffers.h"
#include "../../../include/Render/Transforms.h"
#include "../../../include/ComponentsManager.h"
#include "../../../include/Brakeza3D.h"

ShaderShockWave::ShaderShockWave(
        cl_device_id device_id,
        cl_context context,
        cl_command_queue command_queue,
        float size,
        float speed,
        float ttl
) {
    this->startSize = size;
    this->currentSize = size;
    this->waveSpeed = speed;
    this->ttlWave.setStep(ttl);
    this->ttlWave.setEnabled(true);

    this->clDeviceId = device_id;
    this->clCommandQueue = command_queue;

    initOpenCLProgram(device_id, context);
}

void ShaderShockWave::onUpdate(Vertex3D position)
{
    ttlWave.update();

    auto engineBuffers = EngineBuffers::getInstance();

    this->demoOpenCL(position);

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

void ShaderShockWave::initOpenCLProgram(cl_device_id &device_id, cl_context context)
{
    size_t source_size;
    char * source_str= Tools::readFile(
        EngineSetup::get()->DARKHEAZ_CL_SHADERS_FOLDER + "shockWave.opencl",
        source_size
    );

    program = clCreateProgramWithSource(
        context,
        1,
        (const char **)&source_str,
        (const size_t *)&source_size,
        &clRet
    );

    clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);

    kernel = clCreateKernel(program, "onUpdate", &clRet);

    opencl_buffer_video = clCreateBuffer(
        context,
        CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR,
        EngineBuffers::getInstance()->sizeBuffers * sizeof(Uint32),
        EngineBuffers::getInstance()->videoBuffer,
        &clRet
    );

    opencl_buffer_videoShader = clCreateBuffer(
        context,
        CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR,
        EngineBuffers::getInstance()->sizeBuffers * sizeof(Uint32),
        this->videoBuffer,
        &clRet
    );
}

void ShaderShockWave::demoOpenCL(Vertex3D position)
{
    clEnqueueWriteBuffer(
        clCommandQueue,
        opencl_buffer_video,
        CL_TRUE,
        0,
        EngineBuffers::getInstance()->sizeBuffers * sizeof(Uint32),
        EngineBuffers::getInstance()->videoBuffer,
        0,
        NULL,
        NULL
    );

    clEnqueueWriteBuffer(
        clCommandQueue,
        opencl_buffer_videoShader,
        CL_TRUE,
        0,
        EngineBuffers::getInstance()->sizeBuffers * sizeof(Uint32),
        this->videoBuffer,
        0,
        NULL,
        NULL
    );

    float iTime = this->ttlWave.getAcumulatedTime();
    Point2D focalPoint = Transforms::WorldToPoint(position, ComponentsManager::get()->getComponentCamera()->getCamera());

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
            NULL,
            &global_item_size,
            &local_item_size,
            0,
            NULL,
            NULL
    );

    clEnqueueReadBuffer(
        clCommandQueue,
        opencl_buffer_video,
        CL_TRUE,
        0,
        EngineBuffers::getInstance()->sizeBuffers * sizeof(Uint32),
        EngineBuffers::getInstance()->videoBuffer,
        0,
        NULL,
        NULL
    );

    clEnqueueReadBuffer(
        clCommandQueue,
        opencl_buffer_videoShader,
        CL_TRUE,
        0,
        EngineBuffers::getInstance()->sizeBuffers * sizeof(Uint32),
        this->videoBuffer,
        0,
        NULL,
        NULL
    );

    if (clRet != CL_SUCCESS) {
        Logging::getInstance()->Log( "Error OpenCL kernel: " + std::to_string(clRet) );

        char buffer[1024];
        clGetProgramBuildInfo(program, clDeviceId, CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, NULL);
        if (strlen(buffer) > 0 ) {
            Logging::getInstance()->Log( buffer );
        }
    }
}