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
    GLuint cameraPosUniform = 0;
    GLuint fadeStartUniform = 0;
    GLuint fadeEndUniform = 0;

public:
    ShaderOGLGrid();
    void PrepareMainThread() override;
    void LoadUniforms() override;
    void render(GLuint fbo);
    void Destroy() override;
    Color color = Color::indigo();
    float gridSize = 0.5f;
    float opacity = 0.85f;
    float fadeStart = 85.0f;
    float fadeEnd = 150.0f;
};

#endif //BRAKEZA3D_SHADEROGLGRID_H
