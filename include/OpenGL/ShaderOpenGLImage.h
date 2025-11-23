#ifndef BRAKEZA3D_SHADEROPENGLTEXT_H
#define BRAKEZA3D_SHADEROPENGLTEXT_H

#define GL_GLEXT_PROTOTYPES

#include "ShaderBaseOpenGL.h"
#include "ShaderBaseOpenGLQuad.h"

class ShaderOpenGLImage : public ShaderBaseOpenGL {
    GLuint quadVAO = 0;
    GLuint VBO = 0;

    GLuint modelMatrixUniform = 0;
    GLuint projectionMatrixUniform = 0;
    GLuint textureUniform = 0;
    GLuint alphaUniform = 0;
    GLuint inverseUniform = 0;
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
        GLuint fbo
    ) const;

    void destroy() override;
};


#endif //BRAKEZA3D_SHADEROPENGLTEXT_H
