#define GL_GLEXT_PROTOTYPES

#ifndef BRAKEZA3D_SHADEROPENGLTEXT_H
#define BRAKEZA3D_SHADEROPENGLTEXT_H


#include <SDL2/SDL_opengl.h>
#include "ShaderOpenGL.h"

class ShaderOpenGLImage : public ShaderOpenGL {
    GLuint quadVAO;
    GLuint VBO;

    glm::mat4 projection;


    GLint modelMatrixUniform;
    GLint projectionMatrixUniform;
    GLint spriteColorUniform;
    GLint textureUniform;
    GLint alphaUniform;
public:
    ShaderOpenGLImage(const std::string &vertexFilename, const std::string &fragmentFilename);
    void renderTexture(GLuint TextureID, int x, int y, int w, int h, float alpha) const;
};


#endif //BRAKEZA3D_SHADEROPENGLTEXT_H
