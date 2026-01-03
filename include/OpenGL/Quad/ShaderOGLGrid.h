//
// Created by Eduardo on 08/12/2025.
//

#ifndef BRAKEZA3D_SHADEROGLGRID_H
#define BRAKEZA3D_SHADEROGLGRID_H

#include "../Base/ShaderBaseOpenGL.h"
#include "../Base/ShaderBaseOpenGLQuad.h"
#include "../../Render/Color.h"

class ShaderOGLGrid : public ShaderBaseOpenGL, public ShaderBaseOpenGLQuad
{
    GLuint gridSizeUniform = 0;
    GLuint gridOpacityUniform = 0;
    GLuint gridColorUniform = 0;

public:
    ShaderOGLGrid();
    void PrepareMainThread() override;
    void LoadUniforms() override;
    void render(GLuint fbo);
    void Destroy() override;
    Color color;
    float gridSize = 0.5f;
    float opacity = 0.75f;
};

#endif //BRAKEZA3D_SHADEROGLGRID_H