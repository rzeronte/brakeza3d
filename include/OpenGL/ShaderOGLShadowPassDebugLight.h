//
// Created by Eduardo on 14/11/2025.
//

#ifndef BRAKEZA3D_SHADERSHADOWPASSDEBUGLIGHT_H
#define BRAKEZA3D_SHADERSHADOWPASSDEBUGLIGHT_H

#include "ShaderOpenGL.h"
#include "ShaderOpenGLRender.h"

class ShaderOGLShadowPassDebugLight : public ShaderOpenGL, public ShaderQuadOpenGL  {
public:
    ShaderOGLShadowPassDebugLight();

    void render(GLuint shadowMapArrayTex, int layer, GLuint framebuffer);

    void destroy() override;
};


#endif //BRAKEZA3D_SHADERSHADOWPASSDEBUGLIGHT_H