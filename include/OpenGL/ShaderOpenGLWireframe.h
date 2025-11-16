//
// Created by edu on 12/12/23.
//

#ifndef BRAKEZA3D_SHADEROPENGLWIREFRAME_H
#define BRAKEZA3D_SHADEROPENGLWIREFRAME_H


#include "ShaderBaseOpenGL.h"
#include "../Objects/Mesh3D.h"
#include "../Objects/Mesh3DAnimation.h"

class ShaderOpenGLWireframe : public ShaderBaseOpenGL {
    GLuint VertexArrayID;
public:
    ShaderOpenGLWireframe();

    void renderMesh(Mesh3D *mesh, GLuint framebuffer);
    void renderMeshAnimation(Mesh3DAnimation *mesh, GLuint framebuffer);

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
