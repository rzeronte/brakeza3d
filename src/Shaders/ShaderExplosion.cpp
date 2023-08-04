//
// Created by eduardo on 22/05/23.
//

#include "../../include/Shaders/ShaderExplosion.h"
#include "../../include/Misc/Tools.h"
#include "../../include/Brakeza3D.h"



ShaderExplosion::ShaderExplosion(bool active, Color from, Color to, Point2D origin, float emissionTime, OCParticlesContext oclContext)
:
    ShaderOpenCL(active),
    origin(origin),
    intensity(1.0f),
    emissionTime(emissionTime),
    particlesContext(oclContext)
{
    openCLBufferParticles = clCreateBuffer(context, CL_MEM_READ_WRITE, MAX_OPENCL_PARTICLES * sizeof(OCParticle), nullptr, nullptr);

    Vertex3D vFrom(from.r, from.g, from.b);
    auto oclFrom = Tools::vertexOCL(vFrom);

    Vertex3D vTo(to.r, to.g, to.b);
    auto oclTo = Tools::vertexOCL(vTo);

    ocOrigin = Tools::pointOCL(origin);

    openCLBufferColorFrom = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(OCVertex3D), &oclFrom, nullptr );
    openCLBufferColorTo = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(OCVertex3D), &oclTo, nullptr );
    openCLBufferContext = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(OCParticlesContext), &particlesContext, nullptr );
    openCLBufferOrigin = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(OCPoint2D), &ocOrigin, nullptr );
}

void ShaderExplosion::update()
{
    Shader::update();
    executeKernelOpenCL();
}

void ShaderExplosion::executeKernelOpenCL()
{
    auto dt = Brakeza3D::get()->getDeltaTime();

    //particlesContext = OCParticlesContext();
    //clEnqueueWriteBuffer(clQueue, openCLBufferContext, CL_TRUE, 0, sizeof(OCParticlesContext), &particlesContext, 0, nullptr, nullptr);

    auto kernel = ComponentsManager::get()->getComponentRender()->getExplosionKernel();

    clSetKernelArg(kernel, 0, sizeof(int), &EngineSetup::get()->screenWidth);
    clSetKernelArg(kernel, 1, sizeof(int), &EngineSetup::get()->screenHeight);
    clSetKernelArg(kernel, 2, sizeof(float), &executionTime);
    clSetKernelArg(kernel, 3, sizeof(float), &dt);
    clSetKernelArg(kernel, 4, sizeof(cl_mem), (void *)&EngineBuffers::get()->videoBufferOCL);
    clSetKernelArg(kernel, 5, sizeof(cl_mem), (void *)&openCLBufferContext);
    clSetKernelArg(kernel, 6, sizeof(cl_mem), (void *)&openCLBufferParticles);
    clSetKernelArg(kernel, 7, sizeof(cl_mem), (void *)&openCLBufferColorFrom);
    clSetKernelArg(kernel, 8, sizeof(cl_mem), (void *)&openCLBufferColorTo);
    clSetKernelArg(kernel, 9, sizeof(cl_mem), (void *)&openCLBufferOrigin);
    clSetKernelArg(kernel, 10, sizeof(float), &intensity);
    clSetKernelArg(kernel, 11, sizeof(float), &emissionTime);

    size_t global_item_size = MAX_OPENCL_PARTICLES;
    size_t local_item_size = 64;

    clRet = clEnqueueNDRangeKernel(clQueue, kernel, 1, nullptr, &global_item_size, &local_item_size, 0, nullptr, nullptr);

    debugKernel("ShaderExplosion");
}


void ShaderExplosion::resetContext()
{
    Logging::Message("resetContext");
    particlesContext = OCParticlesContext();
    clReleaseMemObject(openCLBufferContext);
    openCLBufferContext = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(OCParticlesContext), &particlesContext, nullptr );
}

void ShaderExplosion::setIntensity(float intensity) {
    ShaderExplosion::intensity = intensity;
}
