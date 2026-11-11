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
    GLuint uSizeStart                = 0;
    GLuint uSizeEnd                  = 0;
    GLuint uOrientation              = 0;
    GLuint uFlatRight                = 0;
    GLuint uFlatUp                   = 0;
    GLuint uCameraPos                = 0;
    GLuint uFlutterAngle             = 0;
    GLuint uFlutterSpeed             = 0;
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
        float sizeStart,
        float sizeEnd,
        int numParticles,
        int stride,
        bool depthTest = false,
        int orientation = 0,                                 // ParticleOrientation (0 = billboard, original)
        const glm::vec3 &flatRight = glm::vec3(1, 0, 0),     // ejes del quad cuando no es billboard
        const glm::vec3 &flatUp = glm::vec3(0, 0, -1),
        float flutterAngle = 0.0f,                           // balanceo en grados (0 = sin balanceo, original)
        float flutterSpeed = 0.5f
    );
    void Destroy() override;
};

#endif //BRAKEZA3D_SHADEROGLGPUPARTICLES_H
