#include "../../include/OpenGL/ShaderOGLComputeParticles.h"
#include "../../include/Brakeza.h"
#include "../../include/Misc/Logging.h"

ShaderOGLComputeParticles::ShaderOGLComputeParticles()
    : ShaderBaseCompute(Config::get()->SHADERS_FOLDER + "ParticleGPU.cs")
{
}

void ShaderOGLComputeParticles::LoadUniforms()
{
    uOrigin             = glGetUniformLocation(programID, "origin");
    uSmokeDirection     = glGetUniformLocation(programID, "smokeDirection");
    uTotalTime          = glGetUniformLocation(programID, "totalTime");
    uDeltaTime          = glGetUniformLocation(programID, "deltaTime");
    uStopAdd            = glGetUniformLocation(programID, "stopAdd");
    uGravity            = glGetUniformLocation(programID, "gravity");
    uParticlesBySecond  = glGetUniformLocation(programID, "particlesBySecond");
    uParticleLifespan   = glGetUniformLocation(programID, "particleLifespan");
    uSmokeAngleRange    = glGetUniformLocation(programID, "smokeAngleRange");
    uMinVelocity        = glGetUniformLocation(programID, "minVelocity");
    uMaxVelocity        = glGetUniformLocation(programID, "maxVelocity");
    uAlphaMin           = glGetUniformLocation(programID, "alphaMin");
    uAlphaMax           = glGetUniformLocation(programID, "alphaMax");
    uPositionNoise      = glGetUniformLocation(programID, "positionNoise");
    uVelocityNoise      = glGetUniformLocation(programID, "velocityNoise");
    uDecelerationFactor = glGetUniformLocation(programID, "decelerationFactor");
}

void ShaderOGLComputeParticles::simulate(
    GLuint ssbo,
    const glm::vec3& origin,
    const glm::vec3& direction,
    float totalTime,
    float deltaTime,
    bool stopAdd,
    const ParticlesContext& ctx
)
{
    if (programID == 0) {
        LOG_ERROR("[ShaderOGLComputeParticles] programID=0, compute shader not compiled");
        return;
    }

    glUseProgram(programID);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);

    setVec3Uniform(uOrigin,         origin);
    setVec3Uniform(uSmokeDirection, direction);
    setFloatUniform(uTotalTime,          totalTime);
    setFloatUniform(uDeltaTime,          deltaTime);
    setBoolUniform (uStopAdd,            stopAdd);
    setFloatUniform(uGravity,            ctx.GRAVITY);
    setFloatUniform(uParticlesBySecond,  static_cast<float>(ctx.PARTICLES_BY_SECOND));
    setFloatUniform(uParticleLifespan,   ctx.PARTICLE_LIFESPAN);
    setFloatUniform(uSmokeAngleRange,    static_cast<float>(ctx.SMOKE_ANGLE_RANGE));
    setFloatUniform(uMinVelocity,        static_cast<float>(ctx.MIN_VELOCITY));
    setFloatUniform(uMaxVelocity,        static_cast<float>(ctx.MAX_VELOCITY));
    setFloatUniform(uAlphaMin,           static_cast<float>(ctx.MIN_ALPHA));
    setFloatUniform(uAlphaMax,           static_cast<float>(ctx.MAX_ALPHA));
    setFloatUniform(uPositionNoise,      static_cast<float>(ctx.POSITION_NOISE));
    setFloatUniform(uVelocityNoise,      static_cast<float>(ctx.VELOCITY_NOISE));
    setFloatUniform(uDecelerationFactor, ctx.DECELERATION_FACTOR);

    dispatch(NUM_GPU_PARTICLES / GPU_PARTICLES_WORK_GROUP);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, 0);
}
