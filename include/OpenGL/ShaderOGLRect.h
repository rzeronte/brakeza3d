#ifndef BRAKEZA3D_SHADEROPENGLRECT_H
#define BRAKEZA3D_SHADEROPENGLRECT_H

#include "Base/ShaderBaseOpenGL.h"
#include "../Render/Color.h"

class ShaderOGLRect : public ShaderBaseOpenGL
{
    GLuint quadVAO = 0;
    GLuint VBO = 0;

    GLuint modelMatrixUniform      = 0;
    GLuint projectionMatrixUniform = 0;
    GLuint rectColorUniform        = 0;

public:
    ShaderOGLRect();

    void CreateQuadVBO();
    void PrepareMainThread() override;
    void LoadUniforms() override;

    void renderRect(int x, int y, int w, int h, int worldW, int worldH, const Color &c, GLuint fbo) const;

    void Destroy() override;
};

#endif //BRAKEZA3D_SHADEROPENGLRECT_H
