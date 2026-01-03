
#ifndef BRAKEZA3D_SHADEROPENGLOUTLINE_H
#define BRAKEZA3D_SHADEROPENGLOUTLINE_H

#include "../Base/ShaderBaseOpenGL.h"
#include "../Base/ShaderBaseOpenGLQuad.h"
#include "../../3D/Mesh3D.h"
#include "../../3D/Image3D.h"
#include "../../3D/Mesh3DAnimation.h"

class ShaderOGLOutline : public ShaderBaseOpenGL, public ShaderBaseOpenGLQuad
{
    GLuint textureUniform = 0;
    GLuint lineColorUniform = 0;
    GLuint borderThicknessUniform = 0;
    void renderOutline(GLuint textureId, const Color &c, float borderThickness, GLuint fbo);

public:
    ShaderOGLOutline();
    void PrepareMainThread() override;
    void LoadUniforms() override;

    void drawOutline(Mesh3D *m, const Color &c, float borderThickness, GLuint fbo);
    void drawOutlineImage3D(Image3D *i, const Color &c, float borderThickness, GLuint fbo);
    void drawOutline(Mesh3DAnimation *m, Color c, float borderThickness, GLuint fbo);
    void Destroy() override;
};

#endif //BRAKEZA3D_SHADEROPENGLOUTLINE_H
