#ifndef BRAKEZA3D_SHADEROPENGLTEXT_H
#define BRAKEZA3D_SHADEROPENGLTEXT_H

#define GL_GLEXT_PROTOTYPES

#include "ShaderBaseOpenGL.h"
#include "ShaderBaseOpenGLQuad.h"

class ShaderOpenGLImage : public ShaderBaseOpenGL {
    GLuint quadVAO = 0;
    GLuint VBO = 0;

    GLint modelMatrixUniform = 0;
    GLint projectionMatrixUniform = 0;
    GLint textureUniform = 0;
    GLint alphaUniform = 0;
    GLint inverseUniform = 0;
public:
    ShaderOpenGLImage();

    void renderTexture(
        GLuint TextureID,
        int x,
        int y,
        int w,
        int h,
        float alpha,
        bool inverse,
        GLuint framebuffer
    ) const;

    void destroy() override;
};


#endif //BRAKEZA3D_SHADEROPENGLTEXT_H
