#ifndef BRAKEZA3D_SHADEROPENGLDEFERREDLIGHTING_H
#define BRAKEZA3D_SHADEROPENGLDEFERREDLIGHTING_H

#include "ShaderBaseOpenGL.h"
#include "ShaderOGLRenderForward.h"

class ShaderOGLLightPass : public ShaderBaseOpenGL, public ShaderBaseOpenGLQuad  {

    GLuint gPositionUniform;
    GLuint gNormalUniform;
    GLuint viewPosUniform;
    GLuint numPointLightsUniform;
    GLuint numSpotLightsUniform;

    GLuint directionalLightDirectionUniform;
    GLuint directionalLightAmbientUniform;
    GLuint directionalLightDiffuseUniform;
    GLuint directionalLightSpecularUniform;
    GLuint directionalLightMatrixUniform;

    GLuint dirLightShadowMapTextureUniform;

    GLuint materialTextureDiffuseUniform;
    GLuint materialTextureSpecularUniform;
    GLuint materialShininessUniform;

    GLuint numSpotLightShadowMapsUniform;
    GLuint debugShadowMappingUniform;
    GLuint shadowMappingIntensityUniform;
    GLuint enableDirectionalLightShadowMapUniform;

    GLuint bufferSpotLightsMatricesUBO;

public:
    ShaderOGLLightPass();

    void render(
        GLuint gPosition,
        GLuint gNormal,
        GLuint gAlbedoSpec,
        const DirLightOpenGL &directionalLight,
        GLuint dirLightShadowMapTexture,
        int numPointLights,
        int numSpotLights,
        GLuint spotLightsShadowMapTexturesArray,
        int numSpotLightsShadowMaps,
        GLuint fbo
    );

    void destroy() override;

    void fillSpotLightsMatricesUBO();
};

#endif //BRAKEZA3D_SHADEROPENGLDEFERREDLIGHTING_H