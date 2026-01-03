//
// Created by edu on 26/12/23.
//

#ifndef BRAKEZA3D_SHADEROPENGLFOG_H
#define BRAKEZA3D_SHADEROPENGLFOG_H


#include "../Base/ShaderBaseOpenGL.h"
#include "../../Render/Color.h"
#include "../Base/ShaderBaseOpenGLQuad.h"

class ShaderOGLFog : public ShaderBaseOpenGL, public ShaderBaseOpenGLQuad
{
    GLuint resultFramebuffer = 0;
    GLuint textureResult = 0;

    GLuint intensityUniform = 0;
    GLuint fogMaxDistUniform = 0;
    GLuint fogMinDistUniform = 0;
    GLuint fogColourUniform = 0;
    GLuint farPlaneUniform = 0;

    GLuint depthTextureUniform = 0;
    GLuint sceneTextureUniform = 0;
public:
    ShaderOGLFog();
    void LoadUniforms() override;

    void PrepareMainThread() override;

    void render(GLuint sceneTexture, GLuint depthTexture);
    void CreateFramebuffer();

    float fogMaxDist;
    float fogMinDist;
    float intensity;
    Color fogColor;

    [[nodiscard]] GLuint getTextureResult() const;
    void Destroy() override;
};


#endif //BRAKEZA3D_SHADEROPENGLFOG_H
