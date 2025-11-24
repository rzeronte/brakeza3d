//
// Created by edu on 7/12/23.
//

#ifndef BRAKEZA3D_SHADEROPENGLRENDER_H
#define BRAKEZA3D_SHADEROPENGLRENDER_H


#include "ShaderBaseOpenGL.h"
#include "OGLShaderTypes.h"
#include "../Objects/LightSpot.h"
#include "../Objects/Mesh3D.h"
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

    static constexpr size_t MAX_POINT_LIGHTS = 64;
    static constexpr size_t MAX_SPOT_LIGHTS = 64;

    bool buffersInitialized = false;

public:
    ShaderOGLRenderForward();

    void render(
        Object3D *o,
        GLuint textureID,
        GLuint textureSpecularID,
        GLuint vertexbuffer,
        GLuint uvbuffer,
        GLuint normalbuffer,
        int size,
        float alpha,
        GLuint fbo
    ) const;

    int getNumPointLights() const;
    void destroy() override;
    void createUBOFromLights();
    void renderMesh(Mesh3D *o, bool useFeedbackBuffer, GLuint fbo) const;
    void fillUBOLights();
    void extractLights(Object3D *o);
    void setGlobalIlluminationDirection(const Vertex3D &d);
    void setGlobalIlluminationAmbient(const Vertex3D &a);
    void setGlobalIlluminationDiffuse(const Vertex3D &d);
    void setGlobalIlluminationSpecular(const Vertex3D &s);
    bool hasSpotLightsChanged() const;
    bool HasPointLightsChanged() const;
    void setLastSpotLightsSize(int v);
    void setLastPointLightsSize(int v);
    void initializeLightBuffers();
    DirLightOpenGL &getDirectionalLight();
    [[nodiscard]] int getNumSpotLights() const;
    [[nodiscard]] std::vector<LightSpot *> &getShadowMappingSpotLights();
    static glm::mat4 getDirectionalLightMatrix(const DirLightOpenGL& light);
};


#endif //BRAKEZA3D_SHADEROPENGLRENDER_H
