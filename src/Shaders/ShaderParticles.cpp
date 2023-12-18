
#include "../../include/Shaders/ShaderParticles.h"
#include "../../include/Brakeza3D.h"

ShaderParticles::ShaderParticles(bool active, Color from, Color to, OCParticlesContext particlesContext)
:
    ShaderOpenCL(active),
    particlesContext(particlesContext),
    intensity(1),
    stopAdd(false)
{
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
