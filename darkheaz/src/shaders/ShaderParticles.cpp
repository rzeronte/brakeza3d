
#include "ShaderParticles.h"
#include "../../../include/Brakeza3D.h"

ShaderParticles::ShaderParticles(bool active, Color from, Color to, OCParticlesContext particlesContext)
:
    ShaderOpenCL(active),
    particlesContext(particlesContext),
    intensity(1),
    stopAdd(false)
{
    openCLBufferParticles = clCreateBuffer(context, CL_MEM_READ_WRITE, MAX_OPENCL_PARTICLES * sizeof(OCParticle), nullptr, nullptr);
    clEnqueueWriteBuffer(clQueue, openCLBufferParticles, CL_TRUE, 0, MAX_OPENCL_PARTICLES * sizeof(OCParticle), EngineBuffers::get()->getParticles().data(), 0, nullptr, nullptr );

    Vertex3D vFrom(from.r, from.g, from.b);
    auto oclFrom = Tools::vertexOCL(vFrom);

    Vertex3D vTo(to.r, to.g, to.b);
    auto oclTo = Tools::vertexOCL(vTo);

    direction = Vertex3D(0, 10, 0);

    ocOrigin = Tools::pointOCL(origin);
    ocDirection = Tools::vertexOCL(direction);

    openCLBufferColorFrom = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(OCVertex3D), &oclFrom, nullptr );
    openCLBufferColorTo = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(OCVertex3D), &oclTo, nullptr );
    openCLBufferContext = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(OCParticlesContext), &particlesContext, nullptr );

    openCLBufferDirection = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(OCVertex3D), &ocDirection, nullptr );
    openCLBufferOrigin = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(OCPoint2D), &ocOrigin, nullptr );
}

void ShaderParticles::update(Point2D origin, Vertex3D direction, float intensity)
{
    Shader::update();

    setOrigin(origin);
    setDirection(direction);
    setIntensity(intensity);

    executeKernelOpenCL();
}

void ShaderParticles::update()
{
    Shader::update();
    executeKernelOpenCL();
}

void ShaderParticles::executeKernelOpenCL()
{
    ocOrigin = Tools::pointOCL(origin);
    ocDirection = Tools::vertexOCL(direction);

    clEnqueueWriteBuffer(clQueue, openCLBufferOrigin, CL_TRUE, 0, sizeof(OCPoint2D), &ocOrigin, 0, nullptr, nullptr);
    clEnqueueWriteBuffer(clQueue, openCLBufferDirection, CL_TRUE, 0, sizeof(OCVertex3D), &ocDirection, 0, nullptr, nullptr);

    auto dt = Brakeza3D::get()->getDeltaTime();

    auto add = (int) this->stopAdd;

    auto kernel = ComponentsManager::get()->getComponentRender()->getParticlesKernel();

    clSetKernelArg(kernel, 0, sizeof(int), &EngineSetup::get()->screenWidth);
    clSetKernelArg(kernel, 1, sizeof(int), &EngineSetup::get()->screenHeight);
    clSetKernelArg(kernel, 2, sizeof(float), &Brakeza3D::get()->getExecutionTime());
    clSetKernelArg(kernel, 3, sizeof(float), &dt);
    clSetKernelArg(kernel, 4, sizeof(cl_mem), (void *)&EngineBuffers::get()->videoBufferOCL);
    clSetKernelArg(kernel, 5, sizeof(cl_mem), (void *)&openCLBufferContext);
    clSetKernelArg(kernel, 6, sizeof(cl_mem), (void *)&openCLBufferParticles);
    clSetKernelArg(kernel, 7, sizeof(cl_mem), (void *)&openCLBufferColorFrom);
    clSetKernelArg(kernel, 8, sizeof(cl_mem), (void *)&openCLBufferColorTo);
    clSetKernelArg(kernel, 9, sizeof(cl_mem), (void *)&openCLBufferDirection);
    clSetKernelArg(kernel, 10, sizeof(cl_mem), (void *)&openCLBufferOrigin);
    clSetKernelArg(kernel, 11, sizeof(float), &intensity);
    clSetKernelArg(kernel, 12, sizeof(int), &add);

    size_t global_item_size = MAX_OPENCL_PARTICLES;
    size_t local_item_size = 64;

    clRet = clEnqueueNDRangeKernel(clQueue, kernel, 1, nullptr, &global_item_size, &local_item_size, 0, nullptr, nullptr);
}

ShaderParticles::~ShaderParticles()
{
    clReleaseMemObject(openCLBufferParticles);
    clReleaseMemObject(openCLBufferColorFrom);
    clReleaseMemObject(openCLBufferColorTo);
    clReleaseMemObject(openCLBufferDirection);
    clReleaseMemObject(openCLBufferOrigin);
    clReleaseMemObject(openCLBufferContext);
}

void ShaderParticles::setOrigin(const Point2D &origin) {
    ShaderParticles::origin = origin;
}

void ShaderParticles::setDirection(const Vertex3D &direction) {
    ShaderParticles::direction = direction.getNormalize();
}

void ShaderParticles::setIntensity(float intensity) {
    ShaderParticles::intensity = intensity;
}

void ShaderParticles::setStopAdd(bool stopAdd) {
    ShaderParticles::stopAdd = stopAdd;
}

OCParticlesContext &ShaderParticles::getParticlesContext(){
    return particlesContext;
}

void ShaderParticles::resetContext()
{
    particlesContext = OCParticlesContext();
    clReleaseMemObject(openCLBufferContext);
    openCLBufferContext = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(OCParticlesContext), &particlesContext, nullptr );
}
