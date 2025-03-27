//
// Created by edu on 12/12/23.
//

#ifndef BRAKEZA3D_SHADEROPENGLSHADING_H
#define BRAKEZA3D_SHADEROPENGLSHADING_H


#include "ShaderOpenGL.h"
#include "../Objects/Mesh3D.h"

class ShaderOpenGLShading : public ShaderOpenGL {
    GLuint VertexArrayID;
public:
    ShaderOpenGLShading();

    void renderMesh(Mesh3D *mesh, GLuint framebuffer);

    void renderMeshAnimation(Mesh3D *mesh, GLuint framebuffer);

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


#endif //BRAKEZA3D_SHADEROPENGLSHADING_H
