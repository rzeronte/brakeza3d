//
// Created by Eduardo on 13/11/2025.
//

#ifndef BRAKEZA3D_SHADERSHADOWPASS_H
#define BRAKEZA3D_SHADERSHADOWPASS_H

#include "ShaderBaseOpenGL.h"
#include "../include/Objects/Mesh3D.h"
#include "../include/Objects/Mesh3DAnimation.h"
#include "../include/Objects/LightSpot.h"
#include "../include/Objects/OpenGLShaderTypes.h"

class ShaderOGLShadowPass : public ShaderBaseOpenGL
{
    GLuint VertexArrayID;

    GLuint spotLightsDepthMapsFBO;
    GLuint directionalLightDepthMapFBO;

    GLuint directionalLightDepthTexture;

    GLuint matrixViewUniform;
    GLuint matrixModelUniform;

    GLuint spotLightsDepthMapArray = 0;

public:
    ShaderOGLShadowPass();

    void renderMeshIntoArrayTextures(Mesh3D *o, LightSpot* light, GLuint depthArrayTextures, int indexLight, GLuint fb) const;
    void renderMeshIntoDirectionalLightTexture(Mesh3D *o, const DirLightOpenGL& light, GLuint fbo) const;
    void renderMeshAnimatedIntoArrayTextures(Mesh3DAnimation *o, LightSpot* light, GLuint depthArrayTextures, int indexLight, GLuint fb) const;
    void renderMeshAnimatedIntoDirectionalLightTexture(Mesh3DAnimation *o, const DirLightOpenGL& light, GLuint fbo) const;
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
        GLuint vertexbuffer,
        GLuint uvbuffer,
        GLuint normalbuffer,
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