//
// Created by edu on 7/12/23.
//

#ifndef BRAKEZA3D_SHADEROPENGLRENDER_H
#define BRAKEZA3D_SHADEROPENGLRENDER_H


#include "ShaderBaseOpenGL.h"
#include "CommonOGLShaderTypes.h"
#include "../3D/LightSpot.h"
#include "../3D/Mesh3D.h"
#include <glm/ext/matrix_float4x4.hpp>
#include <vector>

class ShaderOGLRenderForward: public ShaderBaseOpenGL
{
    GLuint VertexArrayID = 0;

    DirLightOpenGL directionalLight = DirLightOpenGL {
        glm::vec3(0, 0, 1),
        glm::vec3(0.3f, 0.3f, 0.3f),
        glm::vec3(0.4f, 0.4f, 0.4f),
        glm::vec3(0.5f, 0.5f, 0.5f)
    };

    std::vector<PointLightOpenGL> pointsLights;
    std::vector<SpotLightOpenGL> spotLights;
    std::vector<LightSpot*> shadowMappingLights;

    GLuint bufferUBOLightPoints = 0;
    GLuint bufferUBOSpotLights = 0;

    GLuint matrixProjectionUniform = 0;
    GLuint matrixViewUniform = 0;
    GLuint matrixModelUniform = 0;

    GLuint materialTextureDiffuseUniform = 0;
    GLuint materialTextureSpecularUniform = 0;
    GLuint materialShininessUniform = 0;
    GLuint alphaUniform = 0;

    size_t lastPointLightsSize = 0;
    size_t lastSpotLightsSize = 0;

    // refact
    GLint viewPositionUniform = 0;
    GLint numLightPointsUniform = 0;
    GLint numSpotLightsUniform = 0;

    GLint directionalLightDirectionUniform = 0;
    GLint directionalLightAmbientUniform = 0;
    GLint directionalLightDiffuseUniform = 0;
    GLint directionalLightSpecularUniform = 0;
    GLint enableLightsUniform = 0;

    //--
    GLint directionalLightMatrixUniform = 0;
    GLint dirLightShadowMapTextureUniform = 0;
    GLint numSpotLightShadowMapsUniform = 0;
    GLint debugShadowMappingUniform = 0;
    GLint shadowMappingIntensityUniform = 0;
    GLint shadowMapArrayUniform = 0;
    GLint enableDirectionalLightShadowMapUniform = 0;

    static constexpr size_t MAX_POINT_LIGHTS = 64;
    static constexpr size_t MAX_SPOT_LIGHTS = 64;

    bool buffersInitialized = false;

public:
    ShaderOGLRenderForward();
    void LoadUniforms() override;

    void PrepareMainThread() override;

    void render(
        Object3D *o,
        GLuint textureID,
        GLuint textureSpecularID,
        GLuint vertexbuffer,
        GLuint uvbuffer,
        GLuint normalbuffer,
        int size,
        GLuint fbo
    ) const;

    int getNumPointLights() const;
    void Destroy() override;
    void CreateUBOFromLights();
    void renderMesh(Mesh3D *o, bool useFeedbackBuffer, GLuint fbo) const;
    void FillUBOLights();
    void ExtractLights(Object3D *o);
    void setGlobalIlluminationDirection(const Vertex3D &d);
    void setGlobalIlluminationAmbient(const Vertex3D &a);
    void setGlobalIlluminationDiffuse(const Vertex3D &d);
    void setGlobalIlluminationSpecular(const Vertex3D &s);
    bool hasSpotLightsChanged() const;
    bool HasPointLightsChanged() const;
    void setLastSpotLightsSize(int v);
    void setLastPointLightsSize(int v);
    void InitLightBuffers();
    DirLightOpenGL &getDirectionalLight();
    [[nodiscard]] int getNumSpotLights() const;
    [[nodiscard]] std::vector<LightSpot *> &getShadowMappingSpotLights();
    static glm::mat4 getDirectionalLightMatrix(const DirLightOpenGL& light);
};


#endif //BRAKEZA3D_SHADEROPENGLRENDER_H
