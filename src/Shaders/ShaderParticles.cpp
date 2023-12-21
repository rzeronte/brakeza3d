#define GL_GLEXT_PROTOTYPES

#include "../../include/Shaders/ShaderParticles.h"
#include "../../include/Brakeza3D.h"

ShaderParticles::ShaderParticles(bool active, Color from, Color to, OCParticlesContext particlesContext)
:
    ShaderOpenCL(active),
    colorTo(to),
    colorFrom(from),
    particlesContext(particlesContext),
    intensity(1),
    stopAdd(false)
{
    auto particles = std::vector<OCParticle>(SHADERGL_NUM_PARTICLES, OCParticle{
        glm::vec4(0),
        glm::vec4(0),
        glm::vec4(0),
        0, 0, 0, 0
    });

    glGenBuffers(1, &particlesBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, particlesBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, (int) (particles.size() * sizeof(OCParticle)), &particles[0], GL_STATIC_DRAW);

    timer.start();
}

void ShaderParticles::update(Vertex3D origin, Vertex3D direction, float intensity)
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
    current_ticks = (float) timer.getTicks();
    deltaTime = current_ticks - last_ticks;
    last_ticks = current_ticks;
    executionTime += deltaTime / 1000.f;

    ComponentsManager::get()->getComponentWindow()->getShaderCustomOGLParticles()->render(
        particlesBuffer,
        SHADERGL_NUM_PARTICLES,
        origin,
        direction,
        particlesContext,
        colorFrom,
        colorTo,
        executionTime
    );
}


void ShaderParticles::setOrigin(const Vertex3D &origin) {
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
