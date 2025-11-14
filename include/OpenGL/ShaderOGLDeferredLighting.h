#ifndef BRAKEZA3D_SHADEROPENGLDEFERREDLIGHTING_H
#define BRAKEZA3D_SHADEROPENGLDEFERREDLIGHTING_H

#include "ShaderOpenGL.h"
#include "ShaderOpenGLRender.h"

class ShaderOGLDeferredLighting : public ShaderOpenGL, public ShaderQuadOpenGL  {

    GLint gPositionUniform;
    GLint gNormalUniform;
    GLint viewPosUniform;
    GLint numLightsUniform;
    GLint numSpotLightsUniform;

    GLint directionalLightDirectionUniform;
    GLint directionalLightAmbientUniform;
    GLint directionalLightDiffuseUniform;
    GLint directionalLightSpecularUniform;

    GLint materialTextureDiffuseUniform;
    GLint materialTextureSpecularUniform;
    GLint materialShininessUniform;

public:
    ShaderOGLDeferredLighting();

    void render(
        GLuint gPosition,
        GLuint gNormal,
        GLuint gAlbedoSpec,
        const DirLightOpenGL &directionalLight,
        int numLights,
        int numSpotLights,
        GLuint shadowMapArrayTex,
        int numShadowMaps,
        GLuint outputFramebuffer
    );

    void destroy() override;
};

#endif //BRAKEZA3D_SHADEROPENGLDEFERREDLIGHTING_H