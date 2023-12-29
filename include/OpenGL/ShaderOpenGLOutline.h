
#ifndef BRAKEZA3D_SHADEROPENGLOUTLINE_H
#define BRAKEZA3D_SHADEROPENGLOUTLINE_H

#include "ShaderOpenGL.h"
#include "../Objects/Object3D.h"
#include "ShaderQuadOpenGL.h"
#include "../Objects/Mesh3D.h"

class ShaderOpenGLOutline : public ShaderOpenGL, public ShaderQuadOpenGL  {
    GLint textureUniform;
    GLint lineColorUniform;
    GLint borderThicknessUniform;
public:
    ShaderOpenGLOutline();

    void render(GLint textureID, Color c, float borderThickness);
    void drawOutline(Mesh3D *m, Color c, float borderThickness);

    void destroy() override;
};

#endif //BRAKEZA3D_SHADEROPENGLOUTLINE_H
