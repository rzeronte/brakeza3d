#ifndef BRAKEZA3D_SHADEROPENGLGBUFFER_H
#define BRAKEZA3D_SHADEROPENGLGBUFFER_H

#include "ShaderBaseOpenGL.h"
#include "../3D/Mesh3D.h"

class ShaderOGLRenderDeferred : public ShaderBaseOpenGL
{
    GLuint VertexArrayID = 0;

    GLuint matrixProjectionUniform = 0;
    GLuint matrixViewUniform = 0;
    GLuint matrixModelUniform = 0;
    GLuint drawOffsetUniform = 0;
    GLuint textureDiffuseUniform = 0;
    GLuint textureSpecularUniform = 0;
    GLuint alphaUniform = 1.0f;
public:
    ShaderOGLRenderDeferred();
    void PrepareMainThread() override;
    void LoadUniforms() override;
    void render(Object3D *o, GLuint texId, GLuint specTexId, GLuint vertexBuffer, GLuint uvBuffer, GLuint normalBuffer, int size, float alpha, GLuint fbo) const;
    void renderMesh(Mesh3D *o, bool useFeedbackBuffer, GLuint fbo) const;
    void Destroy() override;
};

#endif //BRAKEZA3D_SHADEROPENGLGBUFFER_H