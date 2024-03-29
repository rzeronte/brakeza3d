#define GL_GLEXT_PROTOTYPES

#ifndef BRAKEZA3D_SHADEROPENGLTEXT_H
#define BRAKEZA3D_SHADEROPENGLTEXT_H


#include "ShaderOpenGL.h"
#include "ShaderQuadOpenGL.h"

class ShaderOpenGLImage : public ShaderOpenGL {
    GLuint quadVAO;
    GLuint VBO;

    GLint modelMatrixUniform;
    GLint projectionMatrixUniform;
    GLint textureUniform;
    GLint alphaUniform;
    GLint inverseUniform;
public:
    ShaderOpenGLImage();

    void renderTexture(GLuint TextureID, int x, int y, int w, int h, float alpha, bool inverse, GLuint framebuffer) const;

    void destroy() override;
};


#endif //BRAKEZA3D_SHADEROPENGLTEXT_H
