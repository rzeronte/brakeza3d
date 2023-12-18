
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
}
