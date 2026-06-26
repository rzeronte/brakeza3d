#ifndef BRAKEZA3D_SHADEROGL_GROUNDCIRCLE_H
#define BRAKEZA3D_SHADEROGL_GROUNDCIRCLE_H

#include "Base/ShaderBaseOpenGL.h"
#include "../Render/Color.h"

class Object3D;

class ShaderOGLGroundCircle : public ShaderBaseOpenGL
{
    GLuint vao = 0;
    GLuint vbo = 0;

    GLuint modelUniform        = 0;
    GLuint viewUniform         = 0;
    GLuint projectionUniform   = 0;
    GLuint colorUniform        = 0;
    GLuint thicknessUniform    = 0;
    GLuint gPositionUniform    = 0;
    GLuint screenSizeUniform   = 0;
    GLuint circleWorldYUniform = 0;
    GLuint filledUniform       = 0;

public:
    ShaderOGLGroundCircle();

    void PrepareMainThread() override;
    void LoadUniforms() override;
    void Destroy() override;

    void draw(Object3D* obj, const Color& color, float radius, GLuint fbo, float thickness = 0.10f, bool filled = false);
};

#endif