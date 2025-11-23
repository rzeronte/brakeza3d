
#ifndef BRAKEZA3D_SHADEROPENGLOUTLINE_H
#define BRAKEZA3D_SHADEROPENGLOUTLINE_H

#include "ShaderBaseOpenGL.h"
#include "ShaderBaseOpenGLQuad.h"
#include "../Objects/Mesh3D.h"
#include "../Objects/Image3D.h"
#include "../Objects/Mesh3DAnimation.h"

class ShaderOGLOutline : public ShaderBaseOpenGL, public ShaderBaseOpenGLQuad  {
    GLuint textureUniform;
    GLuint lineColorUniform;
    GLuint borderThicknessUniform;
    void renderOutline(GLuint textureId, const Color &c, float borderThickness, GLuint fbo);

public:
    ShaderOGLOutline();

    void drawOutline(Mesh3D *m, const Color &c, float borderThickness, GLuint fbo);

    void destroy() override;

    void drawOutlineImage3D(Image3D *i, const Color &c, float borderThickness, GLuint fbo);

    void drawOutline(Mesh3DAnimation *m, Color c, float borderThickness, GLuint fbo);
};

#endif //BRAKEZA3D_SHADEROPENGLOUTLINE_H
