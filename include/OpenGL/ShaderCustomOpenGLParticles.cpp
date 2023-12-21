#include "ShaderCustomOpenGLParticles.h"
#include "../Brakeza3D.h"

ShaderCustomOpenGLParticles::ShaderCustomOpenGLParticles(const std::string &shaderFilename)
:
    ShaderOpenGLCustom(shaderFilename)
{
}

void ShaderCustomOpenGLParticles::render(
    GLint particlesBuffer,
    int numberParticles,
    Vertex3D origin,
    Vertex3D direction,
    OCParticlesContext context,
    Color cf,
    Color ct,
    float executionTime
) {
    glUseProgram(programID);

    setFloat("particlesContext.gravity", context.GRAVITY);
    setFloat("particlesContext.step_add_particle", context.STEP_ADD_PARTICLE);
    setFloat("particlesContext.particle_lifespan", context.PARTICLE_LIFESPAN);
    setFloat("particlesContext.smoke_angle_range", context.SMOKE_ANGLE_RANGE);
    setFloat("particlesContext.min_velocity", context.MIN_VELOCITY);
    setFloat("particlesContext.max_velocity", context.MAX_VELOCITY);
    setFloat("particlesContext.alpha_min", context.ALPHA_MIN);
    setFloat("particlesContext.alpha_max", context.ALPHA_MAX);
    setFloat("particlesContext.position_noise", context.POSITION_NOISE);
    setFloat("particlesContext.velocity_noise", context.VELOCITY_NOISE);
    setFloat("particlesContext.deceleration_factor", context.DECELERATION_FACTOR);

    setVec3("smokeDirection", glm::vec3(direction.x, direction.y, direction.z));
    setVec3("origin", glm::vec3(origin.x, origin.y, origin.z));

    setFloat("totalExecutionTimeInSeconds", executionTime);
    setFloat("deltaTimeInSeconds",  Brakeza3D::get()->getDeltaTime());
    setBool("stopAdd", false);
    setFloat("intensity", 1.0f);
    setInt("screenWidth", EngineSetup::get()->screenWidth);
    setInt("screenHeight", EngineSetup::get()->screenHeight);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, particlesBuffer);
    glDispatchCompute(SHADERGL_NUM_PARTICLES, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    ComponentsManager::get()->getComponentWindow()->getShaderOGLPoints()->render(
        particlesBuffer,
        numberParticles,
        cf,
        ct
    );
}
