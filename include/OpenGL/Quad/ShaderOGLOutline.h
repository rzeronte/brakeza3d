
#ifndef BRAKEZA3D_SHADEROPENGLOUTLINE_H
#define BRAKEZA3D_SHADEROPENGLOUTLINE_H

#include "../Base/ShaderBaseOpenGL.h"
#include "../Base/ShaderBaseOpenGLQuad.h"
#include "../../3D/Mesh3D.h"
#include "../../3D/Image3D.h"
#include "../../3D/Mesh3DAnimation.h"

class ShaderOGLOutline : public ShaderBaseOpenGL, public ShaderBaseOpenGLQuad
{
    // Blit quad program (programID from ShaderBaseOpenGL)
    GLuint textureUniform = 0;

    // Expand shader (second program: Pass 2 — scaled mesh with outline color)
    GLuint expandProgramID = 0;
    GLuint expandModelUniform = 0;
    GLuint expandViewUniform = 0;
    GLuint expandProjectionUniform = 0;
    GLuint expandThicknessUniform = 0;
    GLuint expandColorUniform = 0;
    GLuint expandCenterUniform = 0;

    // Shared VAO for mesh passes (mask + expand)
    GLuint expandVAO = 0;

    // Stencil FBO: color(RGBA8) + depth24/stencil8
    GLuint stencilFBO = 0;
    GLuint stencilColorBuffer = 0;
    GLuint stencilDepthStencilRBO = 0;

    void CreateStencilFBO();
    GLuint CompileExpandProgram();

    void stencilOutlineMesh(
        GLuint vertexBuf, GLuint uvBuf, GLuint normalBuf, int count,
        const glm::mat4 &model, const Color &c, float thickness, bool clearFirst,
        GLuint destFBO, const glm::vec3 &submeshCenter
    );
    void blitToFBO(GLuint destFBO);

public:
    ShaderOGLOutline();
    void PrepareMainThread() override;
    void LoadUniforms() override;

    void drawOutline(Mesh3D *m, const Color &c, float borderThickness, GLuint fbo);
    void drawOutlineSubmesh(Mesh3D *m, const std::string &submeshName, const Color &c, float borderThickness, GLuint fbo);
    void drawOutlineImage3D(Image3D *i, const Color &c, float borderThickness, GLuint fbo);
    void drawOutline(Mesh3DAnimation *m, Color c, float borderThickness, GLuint fbo);

    // Batch API: acumula N outlines en stencilFBO y hace UN solo blit al final
    void clearOutlineBatch();
    void drawOutlineSubmeshBatch(Mesh3D *m, const std::string &submeshName, const Color &c, float borderThickness);
    void flushOutlines(GLuint destFBO);

    void Destroy() override;
};

#endif //BRAKEZA3D_SHADEROPENGLOUTLINE_H
