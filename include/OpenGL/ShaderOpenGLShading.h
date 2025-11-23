//
// Created by edu on 12/12/23.
//

#ifndef BRAKEZA3D_SHADEROPENGLSHADING_H
#define BRAKEZA3D_SHADEROPENGLSHADING_H


#include "ShaderBaseOpenGL.h"
#include "../Objects/Mesh3D.h"

class ShaderOpenGLShading : public ShaderBaseOpenGL {
    GLuint VertexArrayID = 0;
public:
    ShaderOpenGLShading();

    void renderMesh(Mesh3D *mesh, bool useFeedbackBuffer, GLuint fbo);
    void render(
        glm::mat4 modelMatrix,
        GLuint vertexBuffer,
        GLuint uvBuffer,
        GLuint normalBuffer,
        int size,
        GLuint fbo
    );
    void destroy() override;
};


#endif //BRAKEZA3D_SHADEROPENGLSHADING_H
