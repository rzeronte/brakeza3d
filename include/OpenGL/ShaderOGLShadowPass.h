//
// Created by Eduardo on 13/11/2025.
//

#ifndef BRAKEZA3D_SHADERSHADOWPASS_H
#define BRAKEZA3D_SHADERSHADOWPASS_H

#include "ShaderBaseOpenGL.h"
#include "../3D/Mesh3D.h"
#include "../3D/LightSpot.h"
#include "CommonOGLShaderTypes.h"

class ShaderOGLShadowPass : public ShaderBaseOpenGL
{
    GLuint VertexArrayID = 0;

    GLuint spotLightsDepthMapsFBO = 0;
    GLuint directionalLightDepthMapFBO = 0;

    GLuint directionalLightDepthTexture = 0;

    GLuint matrixViewUniform = 0;
    GLuint matrixModelUniform = 0;

    GLuint spotLightsDepthMapArray = 0;

public:
    ShaderOGLShadowPass();

    void renderMeshIntoArrayTextures(Mesh3D *o, bool feedbackFBO, LightSpot* light, int indexLight) const;
    void renderMeshIntoDirectionalLightTexture(Mesh3D *o, bool feedbackFBO, const DirLightOpenGL& light) const;
    void renderIntoArrayDepthTextures(
        Object3D* o,
        LightSpot* light,
        GLuint vertexbuffer,
        GLuint uvbuffer,
        GLuint normalbuffer,
        int size,
        GLuint shadowMapArrayTex,
        int layer,
        GLuint fbo
    ) const;

    void renderIntoDirectionalLightTexture(
        Object3D* o,
        const DirLightOpenGL& light,
        GLuint vertexBuffer,
        GLuint uvBuffer,
        GLuint normalBuffer,
        int size,
        GLuint fbo
    ) const;
    void destroy() override;
    void setupFBOSpotLights();
    void setupFBODirectionalLight();
    void createDirectionalLightDepthTexture();
    void clearDirectionalLightDepthTexture() const;
    void resetFramebuffers();
    void createSpotLightsDepthTextures(int numLights);
    GLuint getSpotLightsShadowMapArrayTextures() const;
    [[nodiscard]] GLuint getDirectionalLightDepthTexture() const;
    [[nodiscard]] GLuint getSpotLightsDepthMapsFBO() const;
    [[nodiscard]] GLuint getDirectionalLightDepthMapFBO() const;
};


#endif //BRAKEZA3D_SHADERSHADOWPASS_H