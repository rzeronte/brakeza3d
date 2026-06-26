#ifndef BRAKEZA3D_SHADEROGLGPUPARTICLES_H
#define BRAKEZA3D_SHADEROGLGPUPARTICLES_H

#include "Base/ShaderBaseOpenGL.h"
#include "../Render/Color.h"

class ShaderOGLGPUParticles : public ShaderBaseOpenGL {
    GLuint VAO                       = 0;
    GLuint uCameraRight              = 0;
    GLuint uCameraUp                 = 0;
    GLuint uVP                       = 0;
    GLuint uTextureSampler           = 0;
    GLuint uColorFrom                = 0;
    GLuint uColorTo                  = 0;
public:
    ShaderOGLGPUParticles();
    void PrepareMainThread() override;
    void LoadUniforms() override;
    void render(
        GLuint particlesStateBuffer,
        GLuint billboardVBO,
        GLuint textureID,
        const Color& colorFrom,
        const Color& colorTo,
        int numParticles,
        int stride
    );
    void Destroy() override;
};

#endif //BRAKEZA3D_SHADEROGLGPUPARTICLES_H
