#include "ShaderLasers.h"
#include "../../../include/EngineSetup.h"
#include "../../../include/EngineBuffers.h"
#include "../../../include/Brakeza3D.h"

ShaderLasers::ShaderLasers() : ShaderOpenCL("lasers.opencl")
{
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

void ShaderLasers::update()
{
    Shader::update();

    if (!isEnabled()) return;

    executeKernelOpenCL();
}

void ShaderLasers::executeKernelOpenCL()
{
    int numberLasers = (int) lasers.size();

    opencl_buffer_lasers = clCreateBuffer(
        context,
        CL_MEM_READ_ONLY,
        numberLasers * sizeof(OCLaser),
        nullptr,
        nullptr
    );

    clEnqueueWriteBuffer(
        clCommandQueue,
        opencl_buffer_lasers,
        CL_FALSE,
        0,
        numberLasers * sizeof(OCLaser),
        lasers.data(),
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
        EngineBuffers::getInstance()->videoBuffer,
        0,
        nullptr,
        nullptr
    );

    clSetKernelArg(kernel, 0, sizeof(int), &EngineSetup::get()->screenWidth);
    clSetKernelArg(kernel, 1, sizeof(int), &EngineSetup::get()->screenHeight);
    clSetKernelArg(kernel, 2, sizeof(float), &Brakeza3D::get()->executionTime);
    clSetKernelArg(kernel, 3, sizeof(cl_mem), (void *)&openClBufferMappedWithVideoInput);
    clSetKernelArg(kernel, 4, sizeof(cl_mem), (void *)&opencl_buffer_pixels_image);
    clSetKernelArg(kernel, 5, sizeof(cl_mem), (void *)&opencl_buffer_lasers);
    clSetKernelArg(kernel, 6, sizeof(int), &numberLasers);

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

    this->lasers.clear();

    this->debugKernel();
}

void ShaderLasers::addLaser(int x1, int y1, int x2, int y2, int r, int g, int b, float i)
{
    OCLaser laser;

    laser.x1 = x1;
    laser.y1 = y1;
    laser.x2 = x2;
    laser.y2 = y2;
    laser.r = r;
    laser.g = g;
    laser.b = b;
    laser.intensity = i;

    this->lasers.push_back(laser);
}

void ShaderLasers::addLaserFromRay(ProjectileRay *ray)
{
    auto camera = ComponentsManager::get()->getComponentCamera()->getCamera();
    auto color = ray->getColor();

    Point2D screenPoint = Transforms::WorldToPoint(ray->getPosition(), camera);

    Vertex3D end = ray->getPosition() + ray->getRay();
    Point2D middlePoint = Transforms::WorldToPoint(end, camera);

    this->addLaser(
        screenPoint.x, screenPoint.y,
        middlePoint.x, middlePoint.y,
        (int) color.r, (int) color.g, (int) color.b,
        0.1
    );
}
