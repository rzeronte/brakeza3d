//
// Created by Eduardo on 13/11/2025.
//

#ifndef BRAKEZA3D_SHADERSHADOWPASS_H
#define BRAKEZA3D_SHADERSHADOWPASS_H

#include "ShaderBaseOpenGL.h"
#include "../include/Objects/Mesh3D.h"
#include "../include/Objects/Mesh3DAnimation.h"
#include "../include/Objects/SpotLight3D.h"
#include "../include/Objects/OpenGLShaderTypes.h"

class ShaderOGLShadowPass : public ShaderBaseOpenGL
{
    GLuint VertexArrayID;

    GLuint spotLightsDepthMapsFBO;
    GLuint directionalLightDepthMapFBO;

    GLuint directionalLightDepthTexture;

    GLint matrixViewUniform;
    GLint matrixModelUniform;

public:
    ShaderOGLShadowPass();

    void renderMeshIntoArrayTextures(Mesh3D *o, SpotLight3D* light, GLuint depthArrayTextures, int indexLight, GLuint fb) const;

    void renderMeshIntoDirectionalLightTexture(Mesh3D *o, DirLightOpenGL& light, GLuint framebuffer) const;

    void renderMeshAnimatedIntoArrayTextures(Mesh3DAnimation *o, SpotLight3D* light, GLuint depthArrayTextures, int indexLight, GLuint fb) const;

    void renderMeshAnimatedIntoDirectionalLightTexture(Mesh3DAnimation *o, DirLightOpenGL& light, GLuint framebuffer) const;

    void renderIntoArrayDepthTextures(
        Object3D* o,
        SpotLight3D* light,
        GLuint vertexbuffer,
        GLuint uvbuffer,
        GLuint normalbuffer,
        int size,
        GLuint shadowMapArrayTex,
        int layer,
        GLuint framebuffer
    ) const;

    void renderIntoDirectionalLightTexture(
        Object3D* o,
        DirLightOpenGL& light,
        GLuint vertexbuffer,
        GLuint uvbuffer,
        GLuint normalbuffer,
        int size,
        GLuint framebuffer
    ) const;

    GLuint getSpotLightsDepthMapsFBO() const;

    GLuint getDirectionalLightDepthMapFBO() const;

    static void setVAOAttributes(GLuint vertexbuffer, GLuint uvbuffer, GLuint normalbuffer);

    void destroy() override;

    void setupFBOSpotLights();

    void setupFBODirectionalLight();

    GLuint getDirectionalLightDepthTexture() const;  // Nuevo getter
    void createDirectionalLightDepthTexture();

    void clearDirectionalLightDepthTexture() const;
};


#endif //BRAKEZA3D_SHADERSHADOWPASS_H