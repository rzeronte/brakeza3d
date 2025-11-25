//
// Created by edu on 12/12/23.
//

#ifndef BRAKEZA3D_SHADEROPENGLWIREFRAME_H
#define BRAKEZA3D_SHADEROPENGLWIREFRAME_H


#include "ShaderBaseOpenGL.h"
#include "../3D/Mesh3D.h"

class ShaderOGLWire : public ShaderBaseOpenGL
{
    GLuint VertexArrayID = 0;
public:
    ShaderOGLWire();

    void renderMesh(Mesh3D *mesh, bool useFeedbackFramebuffer, GLuint fbo) const;
    void render(const glm::mat4 &modelMatrix, GLuint vertexBuffer, GLuint uvBuffer, GLuint normalBuffer, int size, GLuint fbo) const;
    void destroy() override;
};


#endif //BRAKEZA3D_SHADEROPENGLWIREFRAME_H
