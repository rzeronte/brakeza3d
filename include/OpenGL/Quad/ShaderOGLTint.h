//
// Created by edu on 29/12/23.
//

#ifndef BRAKEZA3D_SHADEROPENGLTINT_H
#define BRAKEZA3D_SHADEROPENGLTINT_H


#include "../Base/ShaderBaseOpenGL.h"
#include "../Base/ShaderBaseOpenGLQuad.h"
#include "../../Render/Color.h"

class ShaderOGLTint : public ShaderBaseOpenGL, public ShaderBaseOpenGLQuad
{
    GLuint colorUniform = 0;
    GLuint alphaUniform = 0;

public:
    ShaderOGLTint();
    void LoadUniforms() override;

    void PrepareMainThread() override;

    void render(Color c, float alpha, GLuint fbo);

    void Destroy() override;
};


#endif //BRAKEZA3D_SHADEROPENGLTINT_H
