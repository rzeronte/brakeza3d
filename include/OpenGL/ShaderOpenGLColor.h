//
// Created by edu on 17/12/23.
//

#ifndef BRAKEZA3D_SHADEROPENGLCOLOR_H
#define BRAKEZA3D_SHADEROPENGLCOLOR_H


#include "ShaderBaseOpenGL.h"
#include "../Objects/Object3D.h"

class ShaderOpenGLColor: public ShaderBaseOpenGL {
    GLuint VertexArrayID;

    GLuint framebuffer;
    GLuint textureColorbuffer;
public:
    ShaderOpenGLColor();

    static void setVAOAttributes(GLuint vertexbuffer, GLuint uvbuffer, GLuint normalbuffer);

    void render(glm::mat4 modelView, GLuint vertexbuffer, GLuint uvbuffer, GLuint normalbuffer, int size, bool toFramebuffer, Color color);

    void destroy() override;

    void deleteTexture();

    GLuint getTextureColorBuffer() const;
};


#endif //BRAKEZA3D_SHADEROPENGLCOLOR_H
