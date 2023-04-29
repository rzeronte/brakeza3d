#include "ShaderProjectiles.h"
#include "../../../include/EngineSetup.h"
#include "../../../include/EngineBuffers.h"
#include "../../../include/Brakeza3D.h"

ShaderProjectiles::ShaderProjectiles() : ShaderOpenCL(true, "projectiles.opencl"), image(Image(EngineSetup::get()->IMAGES_FOLDER + "cloud.png"))
{
    opencl_buffer_pixels_image = clCreateBuffer(
        context,
        CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR,
        this->bufferSize * sizeof(Uint32),
        this->image.pixels(),
        nullptr
    );

    clEnqueueWriteBuffer(
            clQueue,
            opencl_buffer_pixels_image,
            CL_TRUE,
            0,
        this->bufferSize * sizeof(Uint32),
            image.pixels(),
            0,
            nullptr,
            nullptr
    );

    clBufferLasers = clCreateBuffer(
        context,
        CL_MEM_READ_WRITE,
        MAX_LASERS * sizeof(OCLaser),
        lasers.data(),
        nullptr
    );

    clBufferProjectiles = clCreateBuffer(
        context,
        CL_MEM_READ_WRITE,
        MAX_PROJECTILES * sizeof(OCProjectile),
        projectiles.data(),
        nullptr
    );
}

void ShaderProjectiles::update()
{
    Shader::update();

    if (!isEnabled()) return;

    executeKernelOpenCL();
}

void ShaderProjectiles::executeKernelOpenCL()
{
    const int numberLasers = (int) lasers.size();
    const int numberProjectiles = (int) projectiles.size();

    clEnqueueWriteBuffer(clQueue, clBufferLasers, CL_TRUE, 0, numberLasers * sizeof(OCLaser), lasers.data(), 0, nullptr, nullptr);
    clEnqueueWriteBuffer(clQueue, clBufferProjectiles, CL_TRUE, 0, numberProjectiles * sizeof(OCProjectile), projectiles.data(), 0, nullptr, nullptr);

    clSetKernelArg(kernel, 0, sizeof(int), &EngineSetup::get()->screenWidth);
    clSetKernelArg(kernel, 1, sizeof(int), &EngineSetup::get()->screenHeight);
    clSetKernelArg(kernel, 2, sizeof(float), &Brakeza3D::get()->getExecutionTime());
    clSetKernelArg(kernel, 3, sizeof(cl_mem), (void *)&EngineBuffers::get()->videoBufferOCL);
    clSetKernelArg(kernel, 4, sizeof(cl_mem), (void *)&opencl_buffer_pixels_image);
    clSetKernelArg(kernel, 5, sizeof(cl_mem), (void *)&clBufferLasers);
    clSetKernelArg(kernel, 6, sizeof(int), &numberLasers);
    clSetKernelArg(kernel, 7, sizeof(cl_mem), (void *)&clBufferProjectiles);
    clSetKernelArg(kernel, 8, sizeof(int), &numberProjectiles);

    size_t global_item_size = this->bufferSize;
    size_t local_item_size = 256;

    clRet = clEnqueueNDRangeKernel(clQueue, kernel, 1, nullptr, &global_item_size, &local_item_size, 0, nullptr, nullptr);

    this->lasers.clear();
    this->projectiles.clear();

    this->debugKernel("ShaderProjectiles");
}

void ShaderProjectiles::addLaser(int x1, int y1, int x2, int y2, int r, int g, int b, float i, bool startCircle, bool endCircle)
{
    this->lasers.emplace_back(OCLaser {x1, y1, x2, y2, r, g, b, i, startCircle, endCircle });
}

void ShaderProjectiles::addLaserFromRay(ProjectileRay *ray)
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
        0.05,
        false,
        false
    );
}

void ShaderProjectiles::addProjectile(Vertex3D position, Color color, float i)
{
    Point2D screenPoint = Transforms::WorldToPoint(position, ComponentsManager::get()->getComponentCamera()->getCamera());

    this->projectiles.emplace_back(OCProjectile {
        screenPoint.x,
        screenPoint.y,
        (int) color.r,
        (int) color.g,
        (int) color.b,
        i * 0.0050f
    });
}

ShaderProjectiles::~ShaderProjectiles()
{
    clReleaseMemObject(opencl_buffer_pixels_image);
    clReleaseMemObject(clBufferLasers);
    clReleaseMemObject(clBufferProjectiles);
}
