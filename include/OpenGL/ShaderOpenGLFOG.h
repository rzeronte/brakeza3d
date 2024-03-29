//
// Created by edu on 26/12/23.
//

#ifndef BRAKEZA3D_SHADEROPENGLFOG_H
#define BRAKEZA3D_SHADEROPENGLFOG_H


#include "ShaderOpenGL.h"
#include "../Misc/Color.h"
#include "ShaderQuadOpenGL.h"

class ShaderOpenGLFOG : public ShaderOpenGL, public ShaderQuadOpenGL {
    GLuint resultFramebuffer;
    GLuint textureResult;

    GLint intensityUniform;
    GLint fogMaxDistUniform;
    GLint fogMinDistUniform;
    GLint fogColourUniform;
    GLint farPlaneUniform;

    GLint depthTextureUniform;
    GLint sceneTextureUniform;

public:
    ShaderOpenGLFOG();

    void render(GLuint sceneTexture, GLuint depthTexture);
    void createFramebuffer();

    float fogMaxDist;
    float fogMinDist;
    float intensity;
    Color fogColor;

    [[nodiscard]] GLuint getTextureResult() const;
    void destroy() override;
};


#endif //BRAKEZA3D_SHADEROPENGLFOG_H
