#ifndef BRAKEZA3D_SHADEROPENGLTEXT_H
#define BRAKEZA3D_SHADEROPENGLTEXT_H

#define GL_GLEXT_PROTOTYPES

#include "Base/ShaderBaseOpenGL.h"

class ShaderOGLImage : public ShaderBaseOpenGL
{
    GLuint quadVAO = 0;
    GLuint VBO = 0;
    // Quad whose UVs are rewritten per draw (renderTextureRegion, e.g. 9-slice): the region lives in
    // the vertex data, not in a uniform, so no other user of this shared program (images, all atlas
    // text) can inherit a stale sub-rect.
    GLuint regionVAO = 0;
    GLuint regionVBO = 0;

    GLuint modelMatrixUniform = 0;
    GLuint projectionMatrixUniform = 0;
    GLuint textureUniform = 0;
    GLuint alphaUniform = 0;
    GLuint inverseUniform = 0;
public:
    ShaderOGLImage();

    void CreateQuadVBO();

    void PrepareMainThread() override;
    void LoadUniforms() override;

    void renderTexture(
        GLuint textureId,
        int x,
        int y,
        int w,
        int h,
        int worldW,
        int worldH,
        float alpha,
        bool inverse,
        GLuint fbo
    ) const;

    // Same as renderTexture (not inverted) but samples only the texture sub-rect [u0,u1] × [v0,v1]
    // (UV 0..1, v from the top of the image). Used by 9-slice panels.
    void renderTextureRegion(
        GLuint textureId,
        int x, int y, int w, int h,
        int worldW, int worldH,
        float u0, float v0, float u1, float v1,
        float alpha,
        GLuint fbo
    ) const;

    void Destroy() override;
};


#endif //BRAKEZA3D_SHADEROPENGLTEXT_H
