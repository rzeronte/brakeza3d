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
    GLuint colorUniform = 0;
public:
    ShaderOGLWire();
    void PrepareMainThread() override;
    void LoadUniforms() override;

    void renderMesh(Mesh3D *mesh, bool useFeedbackFramebuffer, const Color &c, GLuint fbo) const;
    void render(
        const glm::mat4 &modelMatrix,
        GLuint vertexBuffer,
        GLuint uvBuffer,
        GLuint normalBuffer,
        int size,
        const Color &c,
        GLuint fbo
    ) const;
    void Destroy() override;
};


#endif //BRAKEZA3D_SHADEROPENGLWIREFRAME_H
