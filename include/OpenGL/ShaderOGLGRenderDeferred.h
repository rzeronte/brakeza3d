#ifndef BRAKEZA3D_SHADEROPENGLGBUFFER_H
#define BRAKEZA3D_SHADEROPENGLGBUFFER_H

#include "ShaderBaseOpenGL.h"
#include "../Objects/Mesh3D.h"

class ShaderOGLGRenderDeferred : public ShaderBaseOpenGL {
    GLuint VertexArrayID;

    GLint matrixProjectionUniform;
    GLint matrixViewUniform;
    GLint matrixModelUniform;
    GLint drawOffsetUniform;
    GLint textureDiffuseUniform;
    GLint textureSpecularUniform;

    static void setVAOAttributes(GLuint vertexbuffer, GLuint uvbuffer, GLuint normalbuffer);

public:
    ShaderOGLGRenderDeferred();

    void renderMesh(Mesh3D *o, GLuint framebuffer);
    void renderAnimatedMesh(Mesh3D *o, GLuint framebuffer);

    void render(
        Object3D *o,
        GLint textureID,
        GLint textureSpecularID,
        GLuint vertexbuffer,
        GLuint uvbuffer,
        GLuint normalbuffer,
        int size,
        GLuint framebuffer
    );

    void destroy() override;
};

#endif //BRAKEZA3D_SHADEROPENGLGBUFFER_H