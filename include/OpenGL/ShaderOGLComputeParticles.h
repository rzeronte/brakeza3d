#ifndef BRAKEZA3D_SHADEROGLCOMPUTEPARTICLES_H
#define BRAKEZA3D_SHADEROGLCOMPUTEPARTICLES_H

#include "Base/ShaderBaseCompute.h"
#include "../Render/ParticlesContext.h"
#include "../Render/Color.h"
#include <glm/vec3.hpp>

#define NUM_GPU_PARTICLES 1024
#define GPU_PARTICLES_WORK_GROUP 64

class ShaderOGLComputeParticles : public ShaderBaseCompute {
    GLuint uOrigin             = 0;
    GLuint uSmokeDirection     = 0;
    GLuint uTotalTime          = 0;
    GLuint uDeltaTime          = 0;
    GLuint uStopAdd            = 0;
    GLuint uGravity            = 0;
    GLuint uParticlesBySecond  = 0;
    GLuint uParticleLifespan   = 0;
    GLuint uSmokeAngleRange    = 0;
    GLuint uMinVelocity        = 0;
    GLuint uMaxVelocity        = 0;
    GLuint uAlphaMin           = 0;
    GLuint uAlphaMax           = 0;
    GLuint uPositionNoise      = 0;
    GLuint uVelocityNoise      = 0;
    GLuint uDecelerationFactor = 0;
public:
    ShaderOGLComputeParticles();
    void LoadUniforms() override;

    void simulate(
        GLuint ssbo,
        const glm::vec3& origin,
        const glm::vec3& direction,
        float totalTime,
        float deltaTime,
        bool stopAdd,
        const ParticlesContext& ctx
    );
};

#endif //BRAKEZA3D_SHADEROGLCOMPUTEPARTICLES_H
