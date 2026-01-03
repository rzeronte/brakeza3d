#ifndef BRAKEZA3D_SHADEROPENGLTEXT_H
#define BRAKEZA3D_SHADEROPENGLTEXT_H

#define GL_GLEXT_PROTOTYPES

#include "Base/ShaderBaseOpenGL.h"

class ShaderOGLImage : public ShaderBaseOpenGL
{
    GLuint quadVAO = 0;
    GLuint VBO = 0;

    GLuint modelMatrixUniform = 0;
    GLuint projectionMatrixUniform = 0;
    GLuint textureUniform = 0;
    GLuint alphaUniform = 0;
    GLuint inverseUniform = 0;
public:
    ShaderOGLImage();

    void CreateQuadVBO();

    void PrepareMainThread() override;
    void LoadUniforms() override;

    void renderTexture(
        GLuint textureId,
        int x,
        int y,
        int w,
        int h,
        int worldW,
        int worldH,
        float alpha,
        bool inverse,
        GLuint fbo
    ) const;

    void Destroy() override;
};


#endif //BRAKEZA3D_SHADEROPENGLTEXT_H
