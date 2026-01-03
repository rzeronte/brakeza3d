#ifndef BRAKEZA3D_SHADEROPENGLDEFERREDLIGHTING_H
#define BRAKEZA3D_SHADEROPENGLDEFERREDLIGHTING_H

#include "../Base/ShaderBaseOpenGL.h"
#include "../Base/ShaderBaseOpenGLQuad.h"
#include "../ShaderOGLRenderForward.h"

class ShaderOGLLightPass : public ShaderBaseOpenGL, public ShaderBaseOpenGLQuad  {

    GLuint gPositionUniform = 0;
    GLuint gNormalUniform = 0;
    GLuint viewPosUniform = 0;
    GLuint numPointLightsUniform = 0;
    GLuint numSpotLightsUniform = 0;
    GLuint shadowMapArrayUniform = 0;

    GLuint directionalLightDirectionUniform = 0;
    GLuint directionalLightAmbientUniform = 0;
    GLuint directionalLightDiffuseUniform = 0;
    GLuint directionalLightSpecularUniform = 0;
    GLuint directionalLightMatrixUniform = 0;

    GLuint dirLightShadowMapTextureUniform = 0;

    GLuint materialTextureDiffuseUniform = 0;
    GLuint materialTextureSpecularUniform = 0;
    GLuint materialShininessUniform = 0;

    GLuint numSpotLightShadowMapsUniform = 0;
    GLuint debugShadowMappingUniform = 0;
    GLuint shadowMappingIntensityUniform = 0;
    GLuint enableDirectionalLightShadowMapUniform = 0;

    GLuint bufferSpotLightsMatricesUBO = 0;

public:
    ShaderOGLLightPass();

    void LoadUniforms() override;
    void PrepareMainThread() override;

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

    void Destroy() override;

    void FillSpotLightsMatricesUBO();
};

#endif //BRAKEZA3D_SHADEROPENGLDEFERREDLIGHTING_H