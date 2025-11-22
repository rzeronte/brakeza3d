//
// Created by edu on 7/12/23.
//

#ifndef BRAKEZA3D_SHADEROPENGLRENDER_H
#define BRAKEZA3D_SHADEROPENGLRENDER_H


#include "ShaderBaseOpenGL.h"
#include "../Objects/OpenGLShaderTypes.h"
#include "../Objects/SpotLight3D.h"
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
    std::vector<SpotLight3D*> shadowMappingLights;

    GLuint bufferUBOLightPoints = 0;
    GLuint bufferUBOSpotLights = 0;

    GLint matrixProjectionUniform = 0;
    GLint matrixViewUniform = 0;
    GLint matrixModelUniform = 0;

    GLint materialTextureDiffuseUniform = 0;
    GLint materialTextureSpecularUniform = 0;
    GLint materialShininessUniform = 0;
    GLint alphaUniform = 0;

    size_t lastPointLightsSize = 0;
    size_t lastSpotLightsSize = 0;

    static constexpr size_t MAX_POINT_LIGHTS = 64;
    static constexpr size_t MAX_SPOT_LIGHTS = 64;

    bool buffersInitialized = false;

public:
    ShaderOGLRenderForward();

    void render(
        Object3D *o,
        GLint textureID,
        GLint textureSpecularID,
        GLuint vertexbuffer,
        GLuint uvbuffer,
        GLuint normalbuffer,
        int size,
        float alpha,
        GLuint framebuffer
    ) const;

    int getNumPointLights() const;
    void destroy() override;
    void createUBOFromLights();
    void renderMesh(Mesh3D *o, GLuint framebuffer) const;
    void fillUBOLights();
    void extractLights(Object3D *o);
    void setGlobalIlluminationDirection(const Vertex3D &d);
    void setGlobalIlluminationAmbient(const Vertex3D &a);
    void setGlobalIlluminationDiffuse(const Vertex3D &d);
    void setGlobalIlluminationSpecular(const Vertex3D &s);
    void renderAnimatedMesh(Mesh3D *o, GLuint framebuffer) const;
    bool hasSpotLightsChanged() const;
    bool HasPointLightsChanged() const;
    void setLastSpotLightsSize(int v);
    void setLastPointLightsSize(int v);
    void initializeLightBuffers();
    DirLightOpenGL &getDirectionalLight();
    [[nodiscard]] int getNumSpotLights() const;
    [[nodiscard]] std::vector<SpotLight3D *> &getShadowMappingSpotLights();
    static glm::mat4 getDirectionalLightMatrix(const DirLightOpenGL& light);
    static void setVAOAttributes(GLuint vertexbuffer, GLuint uvbuffer, GLuint normalbuffer) ;
};


#endif //BRAKEZA3D_SHADEROPENGLRENDER_H
