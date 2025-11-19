
#ifndef BRAKEZA3D_SHADEROPENGLOUTLINE_H
#define BRAKEZA3D_SHADEROPENGLOUTLINE_H

#include "ShaderBaseOpenGL.h"
#include "ShaderBaseOpenGLQuad.h"
#include "../Objects/Mesh3D.h"
#include "../Objects/Image3D.h"
#include "../Objects/Mesh3DAnimation.h"

class ShaderOpenGLOutline : public ShaderBaseOpenGL, public ShaderBaseOpenGLQuad  {
    GLint textureUniform;
    GLint lineColorUniform;
    GLint borderThicknessUniform;
    void renderOutline(GLint textureID, const Color &c, float borderThickness, GLuint framebuffer);

public:
    ShaderOpenGLOutline();

    void drawOutline(Mesh3D *m, const Color &c, float borderThickness, GLuint framebuffer);

    void destroy() override;

    void drawOutlineImage3D(Image3D *i, const Color &c, float borderThickness, GLuint framebuffer);

    void drawOutline(Mesh3DAnimation *m, Color c, float borderThickness, GLuint framebuffer);
};

#endif //BRAKEZA3D_SHADEROPENGLOUTLINE_H
