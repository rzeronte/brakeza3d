//
// Created by edu on 12/12/23.
//

#ifndef BRAKEZA3D_SHADEROPENGLWIREFRAME_H
#define BRAKEZA3D_SHADEROPENGLWIREFRAME_H


#include "ShaderOpenGL.h"

class ShaderOpenGLWireframe : public ShaderOpenGL {
    GLuint VertexArrayID;
public:
    ShaderOpenGLWireframe();

    void render(
        glm::mat4 ModelMatrix,
        GLuint vertexbuffer,
        GLuint uvbuffer,
        GLuint normalbuffer,
        int size,
        GLuint framebuffer
    );

    static void setVAOAttributes(GLuint vertexbuffer, GLuint uvbuffer, GLuint normalbuffer);

    void destroy() override;
};


#endif //BRAKEZA3D_SHADEROPENGLWIREFRAME_H
