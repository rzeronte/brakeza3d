//
// Created by edu on 17/12/23.
//

#ifndef BRAKEZA3D_SHADEROPENGLCOLOR_H
#define BRAKEZA3D_SHADEROPENGLCOLOR_H


#include "ShaderOpenGL.h"
#include "../Objects/Object3D.h"

class ShaderOpenGLColor: public ShaderOpenGL {
    GLuint VertexArrayID;

    GLuint framebuffer;
public:
    ShaderOpenGLColor();

    static void setVAOAttributes(GLuint vertexbuffer, GLuint uvbuffer, GLuint normalbuffer);

    void render(Object3D *o, GLuint vertexbuffer, GLuint uvbuffer, GLuint normalbuffer, int size, bool toFramebuffer, Color color);

    void destroy() override;

    GLuint textureColorbuffer;
};


#endif //BRAKEZA3D_SHADEROPENGLCOLOR_H
