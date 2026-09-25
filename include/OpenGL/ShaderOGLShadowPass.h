//
// Created by Eduardo on 13/11/2025.
//

#ifndef BRAKEZA3D_SHADERSHADOWPASS_H
#define BRAKEZA3D_SHADERSHADOWPASS_H

#include "Base/ShaderBaseOpenGL.h"
#include "../3D/Mesh3D.h"
#include "../3D/Mesh3DAnimation.h"
#include "../3D/LightSpot.h"
#include "Base/SharedOpenGLStructs.h"
#include "../Render/AABB3D.h"
#include <vector>
#include <glm/mat4x4.hpp>

class ShaderOGLShadowPass : public ShaderBaseOpenGL
{
    GLuint VertexArrayID = 0;

    GLuint spotLightsDepthMapsFBO = 0;
    GLuint directionalLightDepthMapFBO = 0;

    GLuint directionalLightDepthTexture = 0;

    GLuint matrixViewUniform = 0;
    GLuint matrixModelUniform = 0;

    GLuint spotLightsDepthMapArray = 0;

    // Fase 4 (instancing, segunda rebanada): mismo patrón que ShaderOGLRenderDeferred -- buffer
    // persistente de matrices por instancia, configurado una vez en PrepareMainThread().
    GLuint instanceModelBuffer = 0;
    GLuint useInstancingUniform = 0;

    // renderSceneDirectionalLight/renderSceneSpotLight comparten este cuerpo (antes duplicado
    // byte a byte): cull por AABB contra el VP de la luz + agrupar submeshes consecutivos que
    // comparten geometría en un solo draw instanciado (ver ComponentRender::FlushOpaqueQueue,
    // mismo patrón). El shadow pass no usa textura/alpha/drawOffset -- clave de agrupación más
    // simple que en el G-Buffer.
    void drawCastersInstanced(
        const std::vector<Mesh3D*>& casters,
        const std::vector<std::vector<AABB3D>>& casterSubmeshWorldAabbs,
        const glm::mat4& lightVP
    ) const;

public:
    static constexpr int MAX_SHADOW_CASTERS = 16;

    ShaderOGLShadowPass();
    void LoadUniforms() override;

    void PrepareMainThread() override;

    void renderMeshIntoArrayTextures(Mesh3D *o, bool feedbackFBO, LightSpot* light, int indexLight) const;
    void renderMeshIntoDirectionalLightTexture(Mesh3D *o, bool feedbackFBO, const DirLightOpenGL& light) const;

    void renderSceneDirectionalLight(
        const std::vector<Mesh3D*>& casters,
        const std::vector<std::vector<AABB3D>>& casterSubmeshWorldAabbs,
        const DirLightOpenGL& light
    ) const;
    void renderSceneSpotLight(
        const std::vector<Mesh3D*>& casters,
        const std::vector<std::vector<AABB3D>>& casterSubmeshWorldAabbs,
        LightSpot* light,
        int layerIndex
    ) const;

    void renderIntoArrayDepthTextures(
        Object3D* o,
        LightSpot* light,
        GLuint vertexbuffer,
        GLuint uvbuffer,
        GLuint normalbuffer,
        int size,
        GLuint shadowMapArrayTex,
        int layer,
        GLuint fbo,
        GLuint indexBuffer = 0,
        GLsizei indexCount = 0
    ) const;

    void renderIntoDirectionalLightTexture(
        Object3D* o,
        const DirLightOpenGL& light,
        GLuint vertexBuffer,
        GLuint uvBuffer,
        GLuint normalBuffer,
        int size,
        GLuint fbo,
        GLuint indexBuffer = 0,
        GLsizei indexCount = 0
    ) const;
    void Destroy() override;
    void setupFBOSpotLights();
    void setupFBODirectionalLight();
    void createDirectionalLightDepthTexture();
    void clearDirectionalLightDepthTexture() const;
    void ResetFramebuffers();
    void createSpotLightsDepthTextures(int numLights);
    GLuint getSpotLightsShadowMapArrayTextures() const;
    [[nodiscard]] GLuint getDirectionalLightDepthTexture() const;
    [[nodiscard]] GLuint getSpotLightsDepthMapsFBO() const;
    [[nodiscard]] GLuint getDirectionalLightDepthMapFBO() const;
};


#endif //BRAKEZA3D_SHADERSHADOWPASS_H