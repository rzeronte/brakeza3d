#ifndef BRAKEZA3D_SHADEROPENGLDEFERREDLIGHTING_H
#define BRAKEZA3D_SHADEROPENGLDEFERREDLIGHTING_H

#include "ShaderOpenGL.h"
#include "ShaderOpenGLRender.h"

class ShaderOGLDeferredLighting : public ShaderOpenGL, public ShaderQuadOpenGL  {

    GLint gPositionUniform;
    GLint gNormalUniform;
    GLint viewPosUniform;
    GLint numPointLightsUniform;
    GLint numSpotLightsUniform;

    GLint directionalLightDirectionUniform;
    GLint directionalLightAmbientUniform;
    GLint directionalLightDiffuseUniform;
    GLint directionalLightSpecularUniform;
    GLint directionalLightMatrixUniform;

    GLuint dirLightShadowMapTextureUniform;

    GLint materialTextureDiffuseUniform;
    GLint materialTextureSpecularUniform;
    GLint materialShininessUniform;

    GLint numSpotLightShadowMapsUniform;
    GLint debugShadowMappingUniform;
    GLint shadowMappingIntensityUniform;
    GLint enableDirectionalLightShadowMapUniform;

    GLuint bufferSpotLightsMatricesUBO;

public:
    ShaderOGLDeferredLighting();

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
        GLuint framebuffer
    );

    void destroy() override;

    void fillSpotLightsMatricesUBO();

    void setSpotLightInCameraUniforms(glm::vec3 cameraPosition, const Vertex3D &forward) const;
};

#endif //BRAKEZA3D_SHADEROPENGLDEFERREDLIGHTING_H