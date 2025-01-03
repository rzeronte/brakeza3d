//
// Created by edu on 7/12/23.
//

#ifndef BRAKEZA3D_SHADEROPENGLRENDER_H
#define BRAKEZA3D_SHADEROPENGLRENDER_H


#include "ShaderOpenGL.h"
#include "../Objects/OpenGLShaderTypes.h"
#include "../Objects/SpotLight3D.h"
#include "../Objects/Mesh3D.h"
#include <glm/ext/matrix_float4x4.hpp>
#include <vector>

class ShaderOpenGLRender: public ShaderOpenGL {
    GLuint VertexArrayID;

    DirLightOpenGL directionalLight;

    std::vector<PointLightOpenGL> pointsLights;
    std::vector<SpotLightOpenGL> spotLights;
    GLuint bufferUBOLightPoints;
    GLuint bufferUBOSpotLights;

    GLint matrixProjectionUniform;
    GLint matrixViewUniform;
    GLint matrixModelUniform;

    GLint viewPositionUniform;
    GLint numLightPointsUniform;
    GLint numSpotLightsUniform;

    GLint directionalLightDirectionUniform;
    GLint directionalLightAmbientUniform;
    GLint directionalLightDiffuseUniform;
    GLint directionalLightSpecularUniform;

    GLint materialTextureDiffuseUniform;
    GLint materialTextureSpecularUniform;
    GLint materialShininessUniform;
    GLint alphaUniform;

public:
    ShaderOpenGLRender();

    void render(Object3D *o, GLint textureID, GLint textureSpecularID, GLuint vertexbuffer, GLuint uvbuffer, GLuint normalbuffer, int size, float alpha, GLuint framebuffer);

    static void setVAOAttributes(GLuint vertexbuffer, GLuint uvbuffer, GLuint normalbuffer) ;

    void createUBOFromLights();

    DirLightOpenGL *getDirectionalLight();

    void destroy() override;

    void renderMesh(Mesh3D *o, GLuint framebuffer);

    void fillUBOLights();

    void extractLights(Object3D *o);
};


#endif //BRAKEZA3D_SHADEROPENGLRENDER_H
