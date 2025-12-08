//
// Created by Eduardo on 08/12/2025.
//

#ifndef BRAKEZA3D_SHADEROGLGRID_H
#define BRAKEZA3D_SHADEROGLGRID_H

#include "ShaderBaseOpenGL.h"
#include "ShaderBaseOpenGLQuad.h"
#include "../Render/Color.h"

class ShaderOGLGrid : public ShaderBaseOpenGL, public ShaderBaseOpenGLQuad
{
    GLuint gridSizeUniform;
    GLuint gridOpacityUniform;
    GLuint gridColorUniform;

public:
    ShaderOGLGrid();
    void render(GLuint fbo);
    void destroy() override;
    Color color;
    float gridSize = 0.5f;
    float opacity = 0.75f;
};

#endif //BRAKEZA3D_SHADEROGLGRID_H